#
# Copyright (c) 2000-2001,2003 Silicon Graphics, Inc.  All Rights Reserved.
# 
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation; either version 2 of the License, or (at your
# option) any later version.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
# for more details.
#

TOPDIR = ../../..
include $(TOPDIR)/src/include/builddefs

HFILES = dsread.h
CFILES	= roomtemp.c dsread.c
HFILES	= dsread.h
CMDTARGET = pmdaroomtemp
LLDFLAGS = -L.
LLDLIBS = -lmlan $(PCP_PMDALIB)
LCFLAGS = -I.
DFILES	= README help
LSRCFILES = Install Remove pmns $(DFILES) root

SUBDIRS = mlan

IAM	= roomtemp
DOMAIN	= ROOMTEMP
PMDADIR	= $(PCP_PMDAS_DIR)/$(IAM)

LDIRT	= domain.h $(IAM).log pmda$(IAM) pmda_$(IAM).so libmlan.a

default: build-me

include $(BUILDRULES)

.NOTPARALLEL:

ifeq "$(findstring $(TARGET_OS),solaris linux)" ""
build-me:
	$(SUBDIRS_MAKERULE)

install:
else
build-me: domain.h libmlan.a $(CMDTARGET)

install: default 
	$(INSTALL) -m 755 -d $(PMDADIR)
	$(INSTALL) -m 755 Install Remove $(CMDTARGET) $(PMDADIR)
	$(INSTALL) -m 644 $(DFILES) root pmns domain.h $(PMDADIR)
endif

domain.h: ../../pmns/stdpmid
	$(DOMAIN_MAKERULE)

libmlan.a : $(SUBDIRS)
	$(SUBDIRS_MAKERULE)
	rm -f libmlan.a
	ln -sf mlan/libmlan.a .

default_pcp : default

install_pcp : install
