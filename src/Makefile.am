SUBDIRS = driver gui


bin_PROGRAMS = alsa-patch-bay

alsa_patch_bay_SOURCES = \
	main.cpp \
	apb.cpp apb.h \
	subscription.cpp subscription.h \
	addr.cpp addr.h \
	driver.cpp driver.h \
	ui.cpp ui.h \
	misc.cpp misc.h \
	plugins.cpp plugins.h \
	plugin.h \
	driver/jack/jack-addr.cpp

alsa_patch_bay_CXXFLAGS = \
	$(APB_CXXFLAGS) \
	$(LADCCA_CFLAGS) \
	-DPKGLIBDIR=\"$(pkglibdir)\"

alsa_patch_bay_LDFLAGS = \
	$(APB_LDFLAGS) \
	-export-dynamic

alsa_patch_bay_LDADD = \
	$(LADCCA_LIBS) \
	-ldl


pixmapdir = $(APB_DESKTOP_PREFIX)/pixmaps
pixmap_DATA = 5-pin-din.png

EXTRA_DIST = 5-pin-din.png 5-pin-din.xbm

install-exec-local:
	$(LN_S) -f $(bindir)/alsa-patch-bay $(bindir)/jack-patch-bay
