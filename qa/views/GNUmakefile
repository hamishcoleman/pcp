TOPDIR = ../..
include $(TOPDIR)/src/include/builddefs

TESTDIR = $(PCP_VAR_DIR)/testsuite/views
VIEWFILES = 006-01 006-02 006-03 006-04 006-05 \
	    AntiAliasing NoSuchHost RotateColors ScaleManual
LSRCFILES = $(VIEWFILES) GNUmakefile.install

default setup:

install: $(VIEWFILES)
	$(INSTALL) -m 755 -d $(TESTDIR)
	$(INSTALL) -m 644 $(VIEWFILES) $(TESTDIR)
	$(INSTALL) -m 644 GNUmakefile.install $(TESTDIR)/GNUmakefile

include $(BUILDRULES)

default_pcp:

install_pcp:
