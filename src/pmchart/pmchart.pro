TEMPLATE	= app
LANGUAGE	= C++
HEADERS		= console.h pmchart.h main.h \
		  aboutdialog.h chartdialog.h exportdialog.h \
		  hostdialog.h infodialog.h openviewdialog.h \
		  recorddialog.h samplesdialog.h saveviewdialog.h \
		  searchdialog.h seealsodialog.h settingsdialog.h \
		  tab.h tabdialog.h \
		  chart.h colorbutton.h colorscheme.h statusbar.h \
		  fileiconprovider.h namespace.h qcolorpicker.h \
		  tabwidget.h timeaxis.h timebutton.h timecontrol.h \
		  groupcontrol.h gadget.h sampling.h tracing.h
SOURCES		= console.cpp pmchart.cpp main.cpp \
		  aboutdialog.cpp chartdialog.cpp exportdialog.cpp \
		  hostdialog.cpp infodialog.cpp openviewdialog.cpp \
		  recorddialog.cpp samplesdialog.cpp saveviewdialog.cpp \
		  searchdialog.cpp seealsodialog.cpp settingsdialog.cpp \
		  tab.cpp tabdialog.cpp \
		  chart.cpp colorbutton.cpp colorscheme.cpp statusbar.cpp \
		  fileiconprovider.cpp namespace.cpp qcolorpicker.cpp \
		  tabwidget.cpp timeaxis.cpp timebutton.cpp timecontrol.cpp \
		  groupcontrol.cpp gadget.cpp sampling.cpp tracing.cpp \
		  view.cpp
FORMS		= aboutdialog.ui chartdialog.ui console.ui exportdialog.ui \
		  hostdialog.ui infodialog.ui pmchart.ui openviewdialog.ui \
		  recorddialog.ui samplesdialog.ui saveviewdialog.ui \
		  searchdialog.ui seealsodialog.ui settingsdialog.ui \
		  tabdialog.ui
ICON		= pmchart.icns
RC_FILE		= pmchart.rc
RESOURCES	= pmchart.qrc
INCLUDEPATH	+= ../include ../libpcp_qmc/src ../libpcp_qwt/src
CONFIG		+= qt warn_on
release:DESTDIR	= build/debug
debug:DESTDIR	= build/release
LIBS		+= -L../libpcp/src
LIBS		+= -L../libpcp_qmc/src -L../libpcp_qmc/src/$$DESTDIR
LIBS		+= -L../libpcp_qwt/src -L../libpcp_qwt/src/$$DESTDIR
LIBS		+= -lpcp_qmc -lpcp_qwt -lpcp
win32:LIBS	+= -lwsock32
QT		+= network svg
QMAKE_INFO_PLIST = pmchart.info
QMAKE_CXXFLAGS	+= $$(PCP_CFLAGS)
