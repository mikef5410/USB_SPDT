package AttenSwitch;
use Device::USB;
use Moose;
use Moose::Exporter;
use MooseX::ClassAttribute;
use namespace::autoclean;
## no critic (BitwiseOperators)
## no critic (ValuesAndExpressions::ProhibitAccessOfPrivateData)
#
# Class Attributes
class_has 'validVids' => (
  is      => 'ro',
  default => sub { [ 0x4161, ] }
);
class_has 'validPids' => (
  is      => 'ro',
  default => sub { [ 0x00ff, 0x0003, 0x0001, 0x0002 ] }
);
class_has 'SUCCESS'    => ( is => 'ro', default => 0 );
class_has 'FAIL'       => ( is => 'ro', default => -1 );
class_has 'CMD_OUT_EP' => ( is => 'rw', default => 0x1 );
class_has 'CMD_IN_EP'  => ( is => 'rw', default => 0x81 );
#
# Instance Attributes
has 'dev' => (
  is        => 'rw',
  isa       => 'Device::USB::Device',
  predicate => 'connected',
  builder   => 'connect',
  lazy      => 1,
);
has 'usb' => (
  is      => 'rw',
  isa     => 'Device::USB',
  default => sub { Device::USB->new(); }
);
has 'VID' => (
  is        => 'rw',
  isa       => 'Int',
  predicate => 'has_VID',
);
has 'PID' => (
  is        => 'rw',
  isa       => 'Int',
  predicate => 'has_PID',
);
has 'PRODINFO' => (
  is  => 'rw',
  isa => 'AttenSwitch::ProdInfo'
);
has 'verbose' => (
  is      => 'rw',
  isa     => 'Bool',
  default => 0
);
has 'timeout_ms' => (
  is      => 'rw',
  isa     => 'Int',
  default => 500
);

sub connect {
  my $self = shift;
  my @vids = $self->has_VID() ? $self->VID() : @{ AttenSwitch->validVids() };
  my @pids = $self->has_PID() ? $self->PID() : @{ AttenSwitch->validPids() };
  my $vid;
  my $pid;
  my $dev;
  foreach $vid (@vids) {
    foreach $pid (@pids) {
      $dev = $self->usb->find_device( $vid, $pid );
      if ( defined $dev ) {
        goto FOUND;
      }
    }
  }
  if ( !defined $dev ) {
    print "ERROR: could not find any AttenSwitch devices \n";
    return AttenSwitch->FAIL;
  }
FOUND:
  $self->VID( $dev->idVendor() );
  $self->PID( $dev->idProduct() );
  $dev->open();
  if ( $self->verbose ) {
    printf( "Manufacturer   %s, %s \n",                $dev->manufacturer(), $dev->product() );
    printf( "Device         VID: %04X   PID: %04X \n", $self->VID,           $self->PID );
  }
  my $cfg   = $dev->config()->[0];
  my $numIf = $cfg->bNumInterfaces();
  my $inter = $cfg->interfaces()->[0]->[0];
  for ( my $if = 0 ; $if < $numIf ; $if++ ) {
    $inter = $cfg->interfaces()->[$if]->[0];
    my $numEp = $inter->bNumEndpoints();
    if ( $self->verbose() ) {
      printf( "Interface      0x%x,  index %d \n", $inter, $if );
      printf("Endpoints      ");
    }
    for ( my $epnum = 0 ; $epnum < $numEp ; $epnum++ ) {
      my $ep = $inter->endpoints()->[$epnum];
      printf( "0x%02x   ", $ep->bEndpointAddress() ) if ( $self->verbose() );
    }
    printf("\n") if ( $self->verbose() );
  }
  my $claim = $dev->claim_interface(0x2);
  printf("Claim returns  $claim \n") if ( $self->verbose() );
  $self->dev($dev);

  # $dev->close();
  return AttenSwitch->SUCCESS;
}

sub disconnect {
  my $self = shift;
  $self->dev->release_interface(0x2);
  undef( $self->{dev} );
}

sub send_packet {
  my $self     = shift;
  my $packet   = shift;                        #AttenSwitch::Packet;
  my $rxPacket = AttenSwitch::Packet->new();
  if ( defined( $self->dev ) ) {
    if ( ref($packet) && $packet->isa("AttenSwitch::Packet") ) {

      #First, send out the packet ....
      my $txTot = 0;
      my $bytes = $packet->packet;
      my $notSent;
      do {
        my $ret =
          $self->dev->bulk_write( AttenSwitch->CMD_OUT_EP, $bytes, length($bytes),
          $self->timeout_ms );
        $txTot += $ret;
        $notSent = length( $packet->packet ) - $txTot;
        $bytes = substr( $packet->packet, $txTot );
      } while ( $notSent > 0 );

      #Now, get the response packet
      my $rxbuf = "";
      my $ret;
      $ret = $self->dev->bulk_read( AttenSwitch->CMD_IN_EP, $rxbuf, 1024, $self->timeout_ms );
      $rxPacket->from_bytes($rxbuf);
      if ( $rxPacket->command->is_ack ) {
        return ( AttenSwitch->SUCCESS, $rxPacket );
      }
    }
  }
  return ( AttenSwitch->FAIL, $rxPacket );
}

sub sp8t {
  my $self   = shift;
  my $sel    = shift;                      #AttenSwitch::SP8TSETTING
  my $outPkt = AttenSwitch::Packet->new(
    command => AttenSwitch::COMMAND->SP8T,
    payload => pack( "C", $sel->ordinal )
  );
  my ( $res, $rxPacket ) = $self->send_packet($outPkt);
  return ($res);
}

sub spdt {
  my $self   = shift;
  my $sw     = shift;                      #AttenSwitch::SPDTSEL
  my $set    = shift;                      #AttenSwitch::SPDTSETTING
  my $outPkt = AttenSwitch::Packet->new(
    command => AttenSwitch::COMMAND->SPDT,
    payload => pack( "CC", $sw->ordinal, $set->ordinal )
  );
  my ( $res, $rxPacket ) = $self->send_packet($outPkt);
  return ($res);
}

sub atten {
  my $self   = shift;
  my $sel    = shift;                      #AttenSwitch::ATTEN
  my $outPkt = AttenSwitch::Packet->new(
    command => AttenSwitch::COMMAND->ATT,
    payload => pack( "C", $sel->ordinal )
  );
  my ( $res, $rxPacket ) = $self->send_packet($outPkt);
  return ($res);
}

sub stacklightNotify {
  my $self    = shift;
  my $color   = uc(shift);
  my $onTime  = shift || 0;
  my $offTime = shift || 0;
  my $count   = shift || 0;
  my $col     = 0;
  $col |= 0x1 if ( $color =~ /R/ );
  $col |= 0x2 if ( $color =~ /Y/ );
  $col |= 0x4 if ( $color =~ /G/ );
  my $outPkt = AttenSwitch::Packet->new(
    command => AttenSwitch::COMMAND->NOTIFY,
    payload => pack( "CVVV", $col, $onTime, $offTime, $count )
  );
  my ( $res, $rxPacket ) = $self->send_packet($outPkt);
  return ($res);
}

sub readEE {
  my $self   = shift;
  my $addr   = shift;
  my $nbytes = shift;
  my $ret    = "";
  my $k      = 0;
  my $outPkt;
  for ( $k = 0 ; $k < $nbytes ; $k++ ) {
    $outPkt = AttenSwitch::Packet->new(
      command => AttenSwitch::COMMAND->READEE,
      payload => pack( "v", $addr + $k )
                                      );
    my ( $res, $rxPacket ) = $self->send_packet($outPkt);
    my $x = substr( $rxPacket->payload, 2, 1 );
    $ret .= $x;
  }
  return ($ret);
}

sub writeEE {
  my $self = shift;
  my $addr = shift;
  my $val  = shift;    # string of bytes
  my $k    = 0;
  my $outPkt;
  for ( $k = 0 ; $k < length($val) ; $k++ ) {
    $outPkt = AttenSwitch::Packet->new(
      command => AttenSwitch::COMMAND->WRITEEE,
      payload => pack( "v", $addr + $k) . substr( $val, $k, 1 )
    );
    my ( $res, $rxPacket ) = $self->send_packet($outPkt);
  }
}

sub eraseAllEE {
  my $self   = shift;
  my $outPkt = AttenSwitch::Packet->new(
    command => AttenSwitch::COMMAND->ERASEALL,
    payload => ""
  );
  my ( $res, $rxPacket ) = $self->send_packet($outPkt);
  return ($res);
}

sub identify {
  my $self = shift;
  
  my $outPkt = AttenSwitch::Packet->new(
    command => AttenSwitch::COMMAND->ID,
    payload => ""
   );

  my ( $res, $rxPacket ) = $self->send_packet($outPkt);
  my $info=AttenSwitch::ProdInfo->new();
  $info->fromIDPacket($rxPacket);
  return($info);
}

__PACKAGE__->meta->make_immutable;
1;

package AttenSwitch::Packet;
use Moose;
use namespace::autoclean;
has proto_version => (
    is      => 'rw',
    isa     => 'Int',
    default => 1
  );
has command => (
    is        => 'rw',
    isa       => "AttenSwitch::COMMAND",
    predicate => 'has_command'
  );
has payload => (
    is        => 'rw',
    isa       => 'Str',
    predicate => 'has_payload'
  );
has packet => (
    is  => 'rw',
    isa => 'Str'
  );

#Called right after object construction so we can say:
# $obj=AttenSwitch::Packet->new(command=>$command,payload=>$payload);
sub BUILD {
    my $self = shift;
    $self->make() if ( $self->has_command() && $self->has_payload() );
}

sub make {
    my $self    = shift;
    my $command = shift;    #AttenSwitch::COMMAND
    my $payload = shift;    #String of bytes
    if ( defined($command)
      && ref($command)
      && $command->isa("AttenSwitch::COMMAND") )
    {
      $self->command($command);
    }
    if ( defined($payload) ) {
      $self->payload($payload);
    }
    if ( $self->has_command() && $self->has_payload() ) {
      $self->packet( pack( "CvC", 1, length( $self->payload ) + 6, $self->command->ordinal ) );
      $self->packet( $self->packet . pack( "v", $self->cksum_simple( $self->payload ) ) );
      $self->packet( $self->packet . $self->payload );
    }
}

sub from_bytes {
    my $self   = shift;
    my $packet = shift;
    if ( defined($packet) ) {
      $self->packet($packet);
    }
    my $ver = unpack( 'C', substr( $self->packet, 0, 1 ) );
    my $len = unpack( 'v', substr( $self->packet, 1, 2 ) );
    my $cmd = unpack( 'C', substr( $self->packet, 3, 1 ) );
    my $sum = unpack( 'v', substr( $self->packet, 4, 2 ) );
    $self->payload( substr( $self->packet, 6 ) );
    $self->command( AttenSwitch::COMMAND->from_ordinal($cmd) );
}

sub cksum_simple {
    my $self    = shift;
    my $payload = shift;
    my $sum     = 0;
    for my $ch ( unpack( 'C*', $payload ) ) {
      $sum += $ch;
      $sum &= 0xffff;
    }
    return ($sum);
}

sub dump {
    my $self  = shift;
    my $pkt   = $self->payload;
    my $ascii = "";
    my $ver   = unpack( 'C', substr( $self->packet, 0, 1 ) );
    my $len   = unpack( 'v', substr( $self->packet, 1, 2 ) );
    my $sum   = unpack( 'v', substr( $self->packet, 4, 2 ) );
    printf( "Ver: %d\n",   $ver );
    printf( "Len: %d\n",   $len );
    printf( "Cmd: %s\n",   $self->command->name );
    printf( "Sum: 0x%x\n", $sum );
    printf("Payload:\n");
    printf( "%04x - ", 0 );
    my $j = 0;

    for ( $j = 0 ; $j < length($pkt) ; $j++ ) {
      my $val = unpack( "C", substr( $pkt, $j, 1 ) );
      if ( $j && !( $j % 16 ) ) {
        printf("   $ascii");
        $ascii = "";
        printf( "\n%04x - ", $j );
      }
      if ( ( $val < 0x20 ) || ( $val > 0x7E ) ) {
        $ascii .= '.';
      } else {
        $ascii .= chr($val);
      }
      printf( "%02x ", $val );
    }
    my $adj = 16 - ( $j % 16 );
    print '   ' x $adj, "   ", $ascii, "\n";
}
__PACKAGE__->meta->make_immutable;
1;

package AttenSwitch::ProdInfo;
use Moose;
use namespace::autoclean;
has 'productID' => (
    is  => 'rw',
    isa => 'AttenSwitch::PRODUCTID'
  );
has 'protocolVersion' => (
    is  => 'rw',
    isa => 'Int'
  );
has 'fwRevMajor' => (
    is  => 'rw',
    isa => 'Int'
  );
has 'fwRevMinor' => (
    is  => 'rw',
    isa => 'Int'
  );
has 'fwRevBuild' => (
    is  => 'rw',
    isa => 'Int'
  );
has 'fwSHA1' => (
    is  => 'rw',
    isa => 'Str'
  );
has 'fwBldInfo' => (
    is  => 'rw',
    isa => 'Str'
  );
has 'SN' => (
    is  => 'rw',
    isa => 'Str'
   );


sub fromIDPacket {
  my $self = shift;
  my $packet = shift;

  my $pl = $packet->payload;
  my ($prod,$proto,$fwMajor,$fwMinor,$fwBuild,$bldSha)=unpack("CCCCvC/a",$pl);
  $self->productID(AttenSwitch::PRODUCTID->from_ordinal($prod));
  $self->protocolVersion($proto);
  $self->fwRevMajor($fwMajor);
  $self->fwRevMinor($fwMinor);
  $self->fwRevBuild($fwBuild);
  $self->fwSHA1($bldSha);
}


__PACKAGE__->meta->make_immutable;
1;


#
# BEGIN ENUMERATION CLASSES
#
package AttenSwitch::COMMAND;
use Class::Enum qw(ACK NAK RESET ID ECHO SSN DIAG SP8T
  AUXOUT AUXIN ATT LIGHT NOTIFY READEE
  WRITEEE SPDT ERASEALL
);
1;

package AttenSwitch::ATTEN;
use Class::Enum qw(ATT_0DB ATT_10DB ATT_20DB ATT_30DB ATT_40DB
  ATT_50DB ATT_60DB ATT_70DB);
1;

package AttenSwitch::VERSION;
use Class::Enum qw(REV_UNKNOWN REVA );
1;

package AttenSwitch::SPDTSETTING;
use Class::Enum qw(J1SEL J2SEL);
1;

package AttenSwitch::SPDTSEL;
use Class::Enum qw(SW1 SW2);
1;

package AttenSwitch::SP8TSETTING;
use Class::Enum qw(J1 J2 J3 J4 J5 J6 J7 J8);
1;

package AttenSwitch::PRODUCTID;
use Class::Enum (PROD_UNKNOWN => { ordinal=>0xff },
                 PROD_STACKLIGHT => {ordinal => 1},
                 PROD_MAPLEOLT => {ordinal => 2},
                 PROD_ATTEN70 => { ordinal=>3},
                );
1;
