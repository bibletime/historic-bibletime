INCLUDES = $(all_includes)
libfrontend_a_METASOURCES = AUTO
noinst_LIBRARIES = libfrontend.a

libfrontend_a_SOURCES = btinstallmgr.cpp cbtconfig.cpp cinputdialog.cpp \
		manageindicesform.ui cmanageindiceswidget.cpp 	coptionsdialog.cpp \
	cswordsetupdialog.cpp cprofilemgr.cpp cprofile.cpp cprofilewindow.cpp 	cexportmanager.cpp \
	cmdiarea.cpp kstartuplogo.cpp cdragdropmgr.cpp cinfodisplay.cpp 	cprinter.cpp \
	crossrefrendering.cpp cswordsetupmodulelistview.cpp \
		cswordsetupinstallsourcesdialog.cpp cmoduleindexdialog.cpp

all_headers = cmdiarea.h \
btinstallmgr.h \
cbtconfig.h \
cinputdialog.h \
coptionsdialog.h \
cmanageindiceswidget.h \
cswordsetupdialog.h \
cprofilemgr.h \
cprofile.h \
cprofilewindow.h \
cexportmanager.h \
cmdiarea.h \
kstartuplogo.h \
cdragdropmgr.h \
cinfodisplay.h \
cprinter.h \
crossrefrendering.h \
cswordsetupmodulelistview.h \
cswordsetupinstallsourcesdialog.h

EXTRA_DIST = $(libfrontend_a_SOURCES) $(all_headers)

SUBDIRS = util searchdialog mainindex keychooser displaywindow display
noinst_HEADERS = cmoduleindexdialog.h
