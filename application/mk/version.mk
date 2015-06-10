#
# Patterned after Mike Ferrara's g2edk.mk 
#    A makefile for array filling,
#    from Centi-days of yore, 
#    when life was harsh 
#    and cursed by gore,
#    but all the women were willing  ;-]
#
# create a set of global QA variables based on Git, time, host, etc.
#   and generate a header file which can be included by any other file

# $Id$
# $Author$

# NOTE:  Our development branch model assumes release branches will occur off master.
#        When building on branches, if COUNT_ALL_ANCESTORS is set to "no", then
#        the commit-count will be zero at the branch point.
#        If the COUNT_ALL_ANCESTORS is set to "yes", then the build count simply sums
#        the commit-count from tip (most recent) to the beginning of time (first commit)
#
#        These assumptions affect default behavior when some arguments are absent:
#             1.  all branches diverged from master
#             2.  if BUILD_COUNT_POLICY is not specified, assume ALL_ANCESTORS
#             3.  if debug is  not specified, it is set to zero
#
# In our makefiles, BUILD_BRANCH is the branch being built.  The variable GIT_BRANCH is
# ambiguous:  hereafter it is taken to mean the ancestor branch of BUILD_BRANCH.
# I have herewith introduced ANCESTOR_BR as an alias for GIT_BRANCH.
#
# This script creates a file, version.h, which will use BUILD_BRANCH for documenting
# the builds of all code, whether on master or some branch.  ANCESTOR_BR will  only be 
# used when COUNT_ALL_ANCESTORS is set to "no" and BUILD_BRANCH is not master.

#GIT_BRANCH:=master
ANCESTOR_BR?=master

# if not set, default to count all ancestors
ifeq ($(COUNT_ALL_ANCESTORS),)
  COUNT_ALL_ANCESTORS = yes
endif

# Get the branch name from the environment or command line, else compute it from local repo
ifeq ($(origin BUILDBRANCH), undefined)
BUILD_BRANCH:=$(shell git branch 2> /dev/null | sed -e '/^[^*]/d' -e 's/* \(.*\)/\1/')
endif

# Determine if build is occurring in a super-repo context
#    - will be true if the current repo was cloned from LE320_All, and
#      the directory structure will be:
#       ./LE320_All/+
#                   |--LE320_FW/application (directory from which Makefile runs, not version.mk)
#                   |--LE320_SW/
#                   |--.git/
#                   |--.gitmodules
# 
#   - therefore, go up 2 levels and look for two dirs
#           LE320_FW/  and LE320_SW/
#   - as a sanity-check, the .gitmodules file MUST list the same two paths
#
SU_SWDIR:=../../LE320_SW
SU_FWDIR:=../../LE320_FW
SU_SUBM:=../../.gitsubmodules
SU_PWD:=$(shell pwd)
# export $(SU_SWDIR)
# export $(SU_FWDIR)
# GIT_SU=$(shell if [[ -d $(SU_SWDIR) ]] && [[ -d $(SU_FWDIR)" ]] && [[ -f "../../../.gitmodules" ]]; then echo "SU";fi )
#GIT_SU:=$(shell if [[ -d "../../LE320_SW" ]] && [[ -d "../../LE320_FW" ]]; then echo "SU";fi )
GIT_SU:=$(shell if [[ -f "../../.gitmodules" ]]; then echo "SU"; fi )
ifeq (SU,$(GIT_SU))
REPO_TYPE:=SUPER
# compute GIT_SERIAL == BUILD_NUM
#  jimj 1/23/2014: keep the FW build-count independent of the SW
#                  but the SW version in the SUPER-REPO at this instant is still valuable
#                  so it is now part of the global arrays emitted, and documented in version.h

# compute build-count for software:
BRANCH_SW:=$(shell cd $(SU_SWDIR); git branch 2> /dev/null | sed -e '/^[^*]/d' -e 's/* \(.*\)/\1/')
ifneq ($(BUILD_BRANCH),$(BRANCH_SW))
$(warning "WARNING: firmware and software submodules are on different branches")
# $(error "ERROR: firmware and software submodules are on different branches")
endif
BUILD_NUM_SW:=$(shell ../tools/get_buildcnt $(SU_SWDIR) $(SW_BRANCH) $(REF_BR) )
GIT_HASH_SW:=$(shell cd $(SU_SWDIR); git rev-list --abbrev-commit --abbrev=7 HEAD | head -1)
CHANGESET_SW:=$(BUILD_NUM_SW)_$(GIT_HASH_SW)

ifeq ($(COUNT_ALL_ANCESTORS),yes)
   BUILD_NUM_SW:=$(shell ../tools/get_buildcnt -d $(SU_SWDIR) -b $(BRANCH_SW) )
else
   BUILD_NUM_SW:=$(shell ../tools/get_buildcnt -d $(SU_SWDIR) -b $(BRANCH_SW) -a $(ANCESTOR_BR) )
endif
GIT_HASH_SW:=$(shell cd $(SU_SWDIR); git rev-list --abbrev-commit --abbrev=7 HEAD | head -1)
CHANGESET_SW:=$(BUILD_NUM_SW)_$(GIT_HASH_SW)

# Compute build-count for firmware
ifeq ($(COUNT_ALL_ANCESTORS),yes)
	 BUILD_NUM_FW:=$(shell ../tools/get_buildcnt -d $(SU_FWDIR) -b $(BUILD_BRANCH) )
else
	 BUILD_NUM_FW:=$(shell ../tools/get_buildcnt -d $(SU_FWDIR) -b $(BUILD_BRANCH) -a $(ANCESTOR_BR) )
endif
#BUILD_NUM:=$(shell echo $(BUILD_NUM_FW) + $(BUILD_NUM_SW) | bc)

else
# This is a simple-repo (not a super-repo)
REPO_TYPE:=SIMPLE
BUILD_NUM_SW:=0000
BRANCH_SW:=unknown
GIT_HASH_SW:=unknown
CHANGESET_SW:=$(BUILD_NUM_SW)_$(GIT_HASH_SW)

# Compute build-count for firmware
ifeq ($(COUNT_ALL_ANCESTORS),yes)
	BUILD_NUM_FW:=$(shell ../tools/get_buildcnt -d $(SU_PWD) -b $(BUILD_BRANCH) )
else
	BUILD_NUM_FW:=$(shell ../tools/get_buildcnt -d $(SU_PWD) -b $(BUILD_BRANCH) -a $(ANCESTOR_BR) )
endif

endif
BUILD_NUM:=$(BUILD_NUM_FW)

GIT_HASH_FULL:=$(shell git rev-parse HEAD)
HASH_FULL_SZ:=41    # need a global array this large, to hold the entire hash
GIT_HASH_CLEAN:=$(shell git rev-list --abbrev-commit --abbrev=7 HEAD | head -1)
GIT_HASH_DIRTY:=$(shell git rev-list --abbrev-commit --abbrev=6 HEAD | head -1)
GIT_DIRTY:=$(shell if git diff-files --quiet --ignore-submodules --;then echo "clean";else echo "dirty";fi)
ifeq (dirty, $(GIT_DIRTY))
GIT_HASH_DIRTY:=+$(GIT_HASH_DIRTY)
GIT_HASH=$(GIT_HASH_DIRTY)
else
GIT_HASH:=$(GIT_HASH_CLEAN)
endif

CHANGESET:=$(BUILD_NUM)_$(GIT_HASH)
BFILENAM:=$(CHANGESET)
CHGSET_SZ:=12
# append  _SU if SHA1 if from a super-repo
ifeq (SU,$(GIT_SU))
CHANGESET:=$(CHANGESET)_$(GIT_SU)
CHANGESET_SW:=$(CHANGESET_SW)_$(GIT_SU)
CHGSET_SZ:=15
endif

BUILD_DATE:=$(shell date +%Y%m%d)
BUILD_TIME:=$(shell date +%H%M%S)

# Make a build-facts file, useful on the web-portal to self-document the bits
BFILE:=build_$(BFILENAM).txt
RM_INFO:=$(shell rm -fv "build_*.txt")
BUSER:=$(shell id -u -n)

$(BFILE):
	@rm -f build_*.txt
	@echo "BLD-DOC FILE   $(BFILE)"
	@echo "REPO TYPE      $(REPO_TYPE)"
	@echo "BLD CONFIG     $(BLD_CONFIG)"
	@echo "BRANCH    FW   $(BUILD_BRANCH)"
	@echo "BUILD-NUM FW   $(BUILD_NUM_FW)"
	@echo "BUILD-SHA FW   $(GIT_HASH_CLEAN)"
	@echo "CHANGESET FW   $(CHANGESET)"
	@echo ""
	@echo "BRANCH    SW   $(BRANCH_SW)"
	@echo "BUILD-NUM SW   $(BUILD_NUM_SW)"
	@echo "BUILD-SHA SW   $(GIT_HASH_SW)"
	@echo "CHANGESET SW   $(CHANGESET_SW)"
	@echo "FW build Info:"                             >> $(BFILE)
	@echo "    release state      $(BLD_CONFIG)"       >> $(BFILE)
	@echo "    repo type          $(REPO_TYPE)"        >> $(BFILE) 
	@echo "    branch    FW       $(BUILD_BRANCH)"     >> $(BFILE)
	@echo "    build num FW       $(BUILD_NUM_FW)"     >> $(BFILE)
	@echo "    git hash  FW full  $(GIT_HASH_FULL)"    >> $(BFILE) 
	@echo "    git hash  FW short $(GIT_HASH)"         >> $(BFILE) 
	@echo "    changeset FW       $(CHANGESET)"        >> $(BFILE)
	@echo "    by user            $(BUSER)"            >> $(BFILE)
	@echo "    on date            $(BUILD_DATE)"       >> $(BFILE)
	@echo "    at time            $(BUILD_TIME)"       >> $(BFILE)
	@echo "    on host machine    $(HOSTNAME)"         >> $(BFILE)
	@echo " "    >> $(BFILE)
	@echo "SW build info, valid if in super-repo:"     >> $(BFILE)
	@echo "    branch    SW       $(BRANCH_SW)"        >> $(BFILE)
	@echo "    build num SW       $(BUILD_NUM_SW)"     >> $(BFILE)
	@echo "    git hash  SW       $(GIT_HASH_SW)"      >> $(BFILE) 
	@echo "    changeset SW       $(CHANGESET_SW)"     >> $(BFILE)

.PHONEY:	buildinfo
buildinfo:	$(BFILE)

# Make the version file dependent on _every_ C and C++ source file
VFILE:=version.h
$(VFILE):	USER:=$(shell id -u -n)
$(VFILE):	$(CSRC) $(CPPSRC)
	@echo make $(VFILE)
	@rm -f $(VFILE)
	@echo "//////////////////////////////////////////////////////////////////////////////" >> $(VFILE)
	@echo "//"                                                                             >> $(VFILE)
	@echo "//           Copyright (C) 2012-2013 Tektronix, all rights reserved"            >> $(VFILE)
	@echo "//"                                                                             >> $(VFILE)
	@echo "//                     3841 Brickway Blvd. Suite 210"                           >> $(VFILE)
	@echo "//                     Santa Rosa, CA 95403"                                    >> $(VFILE)
	@echo "//                     Tel:(707)595-4770"                                       >> $(VFILE)
	@echo "//"                                                                             >> $(VFILE)
	@echo "//    Filename:        version.h"                                               >> $(VFILE)
	@echo "//    For product:     LE320/160"                                               >> $(VFILE)
	@echo "//"                                                                             >> $(VFILE)
	@echo "//    This is an auto-generated header file which will be overwritten by"       >> $(VFILE)
	@echo "//    the sub-makefile, version.mk, at the next compile."                       >> $(VFILE)
	@echo "//"                                                                             >> $(VFILE)
	@echo "//    This header defines four (4) global strings useful for QA    "            >> $(VFILE)
	@echo "//    when displayed in user-facing or remote interfaces           "            >> $(VFILE)
	@echo "//"                                                                             >> $(VFILE)
	@echo "//          NAME              Example                              "            >> $(VFILE)
	@echo "//          -----------       ----------------------------------   "            >> $(VFILE)
	@echo "//          build_info        \"built at $(BUILD_DATE) at $(BUILD_TIME)"        >> $(VFILE)
	@echo "//          build_sha1        \"$(CHANGESET)\"                     "            >> $(VFILE)
	@echo "//          build_branch      \"$(BUILD_BRANCH)\"                  "            >> $(VFILE)
	@echo "//          build_rls_state   \"DEBUG\" or \"RELEASE\"             "            >> $(VFILE)
	@echo "//"                                                                             >> $(VFILE)
	@echo "//    Auto-generated:  date: $(BUILD_DATE), time: $(BUILD_TIME)"                >> $(VFILE)
	@echo "//    by user:         $(USER)"                                                 >> $(VFILE)
	@echo "//    on host machine: $(HOSTNAME)"                                             >> $(VFILE)
	@echo "//"                                                                             >> $(VFILE)
	@echo "//"                                                                             >> $(VFILE)
	@echo "//////////////////////////////////////////////////////////////////////////////" >> $(VFILE)
	@echo "#ifndef _VERSION_INCLUDED"                                                >> $(VFILE)
	@echo "#define _VERSION_INCLUDED"                                                >> $(VFILE)
	@echo "#ifdef GLOBAL_VERSION"                                                    >> $(VFILE)
	@echo "#define VERSIONGLOBAL"                                                    >> $(VFILE)
	@echo "#define VERSIONPRESET(A) = (A)"                                           >> $(VFILE)
	@echo "#else"                                                                    >> $(VFILE)
	@echo "#define VERSIONPRESET(A)"                                                 >> $(VFILE)
	@echo "#ifdef __cplusplus"                                                       >> $(VFILE)
	@echo "#define VERSIONGLOBAL extern \"C\""                                       >> $(VFILE)
	@echo "#else"                                                                    >> $(VFILE)
	@echo "#define VERSIONGLOBAL extern"                                             >> $(VFILE)
	@echo "#endif     /*__cplusplus*/"                                               >> $(VFILE)
	@echo "#endif     /* end GLOBAL_VERSION */"                                      >> $(VFILE)
	@echo ""                                                                         >> $(VFILE)
	@echo "#ifndef VERSIONPRINT"                                                     >> $(VFILE)
	@echo "#ifdef SECLOAD_BUILD"  							 >> $(VFILE)
	@echo "#undef RELEASE"  							 >> $(VFILE)
	@echo "#else"									 >> $(VFILE)
	@echo "#include \"instr_debug.h\"  /* this controls RELEASE_STATE */ "           >> $(VFILE)
	@echo "#endif"									 >> $(VFILE)
	@echo "#endif"                                                                   >> $(VFILE)
	@echo ""                                                                         >> $(VFILE)
	@echo "#ifdef RELEASE"                                                           >> $(VFILE)
	@echo "#define BUILD_INFO \"built $(BUILD_DATE) at $(BUILD_TIME), changeset $(CHANGESET) of branch $(BUILD_BRANCH), by $(USER) on $(HOSTNAME)\""  >> $(VFILE)
	@echo "#define RELEASE_STATE \"RELEASE\""                                        >> $(VFILE)
	@echo "#else"                                                                    >> $(VFILE)
	@echo "#define BUILD_INFO \"DEBUG, built $(BUILD_DATE) at $(BUILD_TIME), changeset $(CHANGESET) of branch $(BUILD_BRANCH), by $(USER) on $(HOSTNAME)\""  >> $(VFILE)
	@echo "#define RELEASE_STATE \"DEBUG\""                                          >> $(VFILE)
	@echo "#endif      /* end RELEASE_STATE */"                                      >> $(VFILE)
	@echo "#define BUILD_SHA1   \"$(CHANGESET)\""                                    >> $(VFILE)
	@echo "#define FULL_SHA1    \"$(GIT_HASH_FULL)\""                                >> $(VFILE)
	@echo "#define BUILD_BRANCH \"$(BUILD_BRANCH)\""                                 >> $(VFILE)
	@echo "VERSIONGLOBAL const char build_info     [] VERSIONPRESET(BUILD_INFO);"    >> $(VFILE)
	@echo "VERSIONGLOBAL const char build_sha1     [] VERSIONPRESET(BUILD_SHA1);"    >> $(VFILE)
	@echo "VERSIONGLOBAL const char build_branch   [] VERSIONPRESET(BUILD_BRANCH);"  >> $(VFILE)
	@echo "VERSIONGLOBAL const char build_rls_state[] VERSIONPRESET(RELEASE_STATE);" >> $(VFILE)
	@echo "VERSIONGLOBAL const char build_sha1_full[] VERSIONPRESET(FULL_SHA1);"     >> $(VFILE)
	@echo "  "                                                                       >> $(VFILE)
	@echo "#define SZ_BUILD_CHANGESET $(CHGSET_SZ)"                                  >> $(VFILE)
	@echo "#define SZ_BUILD_SHA1 7"                                                  >> $(VFILE)
	@echo "#define SZ_FULL_SHA1 $(HASH_FULL_SZ))"                                    >> $(VFILE)
	@echo "#endif    // end _VERSION_INCLUDED"                                       >> $(VFILE)

version:	$(VFILE) $(BFILE)
# end version.mk

