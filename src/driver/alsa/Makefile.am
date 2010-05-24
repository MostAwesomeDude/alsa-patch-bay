driverplugindir = $(pkglibdir)/driver

if ALSA
driverplugin_LTLIBRARIES = alsa.la
endif # ALSA

alsa_la_SOURCES = \
	alsa-driver.cpp alsa-driver.h \
	alsa-addr.cpp alsa-addr.h

alsa_la_CXXFLAGS = \
	$(APB_CXXFLAGS) \
	$(LADCCA_CFLAGS) \
	-I$(top_srcdir)/src

alsa_la_LDFLAGS = \
	$(ALSA_LIBS) \
	$(LADCCA_LIBS) \
	-lpthread \
	-shared \
	-module
