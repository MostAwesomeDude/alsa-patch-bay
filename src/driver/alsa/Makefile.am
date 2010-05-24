
EXTRA_PROGRAMS = alsa.so

alsa_so_SOURCES = \
	alsa-driver.cpp alsa-driver.h \
	alsa-addr.cpp alsa-addr.h

alsa_so_CXXFLAGS = \
	$(APB_CXXFLAGS) \
	$(LADCCA_CFLAGS) \
	-I$(top_srcdir)/src

alsa_so_LDFLAGS = \
	$(ALSA_LIBS) \
	$(LADCCA_LIBS) \
	-lpthread \
	-shared

driverplugindir = $(pkglibdir)/driver


if ALSA
driverplugin_PROGRAMS = alsa.so
endif # ALSA

