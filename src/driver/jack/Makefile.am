
EXTRA_PROGRAMS = jack.so

jack_so_SOURCES = \
	jack-driver.cpp jack-driver.h \
	jack-addr.cpp jack-addr.h

jack_so_CXXFLAGS = \
	$(APB_CXXFLAGS) \
	$(LADCCA_CFLAGS) \
	$(JACK_CFLAGS) \
	-I$(top_srcdir)/src

jack_so_LDFLAGS = \
	$(JACK_LIBS) \
	$(LADCCA_LIBS) \
	-lpthread \
	-shared

driverplugindir = $(pkglibdir)/driver

if JACK
driverplugin_PROGRAMS = jack.so
endif # JACK
