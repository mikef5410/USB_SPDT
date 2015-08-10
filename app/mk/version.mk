#
#
# This script creates a file, version.h

BUILD_BRANCH:=$(shell git symbolic-ref -q HEAD | sed -e "s|refs/heads/||")
ifeq ($(BUILD_BRANCH),"")
BUILD_BRANCH:="DETACHED_HEAD"
endif

GIT_SERIAL:=$(shell git rev-list HEAD | wc -l)
GIT_HASH:=$(shell git rev-list --abbrev-commit HEAD | head -1)
GIT_HASH_FULL:=$(shell git rev-list HEAD | head -1)
GIT_DIRTY:=$(shell if git diff-files --quiet --ignore-submodules --;then echo "clean";else echo "dirty";fi)
GIT_DESC:=$(shell git describe --tags HEAD)

ifeq (dirty, $(GIT_DIRTY))
GIT_HASH:=$(GIT_HASH)+
GIT_HASH_FULL:=$(GIT_HASH_FULL)+
GIT_DESC:=$(GIT_DESC)+
endif

CHANGESET:=$(GIT_SERIAL)_$(GIT_HASH)

BUILD_DATE:=$(shell date +%Y%m%d)
BUILD_TIME:=$(shell date +%H%M%S)

# Make the version file dependent on _every_ C and C++ source file
VFILE:=version.h
$(VFILE):	USER:=$(shell id -u -n)
$(VFILE):	$(CSRC) $(CPPSRC)
	@echo make $(VFILE)
	@rm -f $(VFILE)
	@echo "//////////////////////////////////////////////////////////////////////////////" >> $(VFILE)
	@echo "//"                                                                             >> $(VFILE)
	@echo "//           Copyright (C) 2015, Michael Ferrara all rights reserved"           >> $(VFILE)
	@echo "//"                                                                             >> $(VFILE)
	@echo "//    Filename:        version.h"                                               >> $(VFILE)
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
	@echo ""                                                                         >> $(VFILE)
	@echo "#ifdef RELEASE"                                                           >> $(VFILE)
	@echo "#define BUILD_INFO \"built $(BUILD_DATE) at $(BUILD_TIME), changeset $(CHANGESET) ($(GIT_DESC)) of branch $(BUILD_BRANCH), by $(USER) on $(HOSTNAME)\""  >> $(VFILE)
	@echo "#define RELEASE_STATE \"RELEASE\""                                        >> $(VFILE)
	@echo "#else"                                                                    >> $(VFILE)
	@echo "#define BUILD_INFO \"DEBUG, built $(BUILD_DATE) at $(BUILD_TIME), changeset $(CHANGESET) ($(GIT_DESC)) of branch $(BUILD_BRANCH), by $(USER) on $(HOSTNAME)\""  >> $(VFILE)
	@echo "#define RELEASE_STATE \"DEBUG\""                                          >> $(VFILE)
	@echo "#endif      /* end RELEASE_STATE */"                                      >> $(VFILE)
	@echo "#define BUILD_SHA1   \"$(CHANGESET)\""                                    >> $(VFILE)
	@echo "#define FULL_SHA1    \"$(GIT_HASH_FULL)\""                                >> $(VFILE)
	@echo "#define BUILD_BRANCH \"$(BUILD_BRANCH)\""                                 >> $(VFILE)
	@echo "#define GIT_DESC \"$(GIT_DESC)\""                                         >> $(VFILE)
	@echo "VERSIONGLOBAL const char build_info     [] VERSIONPRESET(BUILD_INFO);"    >> $(VFILE)
	@echo "VERSIONGLOBAL const char build_sha1     [] VERSIONPRESET(BUILD_SHA1);"    >> $(VFILE)
	@echo "VERSIONGLOBAL const char build_branch   [] VERSIONPRESET(BUILD_BRANCH);"  >> $(VFILE)
	@echo "VERSIONGLOBAL const char build_rls_state[] VERSIONPRESET(RELEASE_STATE);" >> $(VFILE)
	@echo "VERSIONGLOBAL const char build_sha1_full[] VERSIONPRESET(FULL_SHA1);"     >> $(VFILE)
	@echo "VERSIONGLOBAL const char build_git_desc [] VERSIONPRESET(GIT_DESC);"      >> $(VFILE)
	@echo "  "                                                                       >> $(VFILE)
	@echo "#endif    // end _VERSION_INCLUDED"                                       >> $(VFILE)

version:	$(VFILE) $(BFILE)
# end version.mk

