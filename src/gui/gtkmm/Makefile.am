
EXTRA_PROGRAMS = gtkmm.so

gtkmm_so_SOURCES = \
	apb-gtkmm.cpp apb-gtkmm.h \
	button-array.cpp button-array.h \
	choice.cpp choice.h \
	port-button.cpp port-button.h \
	colour.cpp colour.h \
	main-window.cpp main-window.h \
	logbox.cpp logbox.h

gtkmm_so_CXXFLAGS = \
	$(APB_CXXFLAGS) \
	$(GTKMM_CFLAGS) \
	$(LADCCA_CFLAGS) \
	-DAPB_DESKTOP_PREFIX=\"$(APB_DESKTOP_PREFIX)\"

gtkmm_so_LDFLAGS = \
	$(GTKMM_LIBS) \
	-shared

uiplugindir = $(pkglibdir)/ui


if GTKMM
uiplugin_PROGRAMS = gtkmm.so
endif # GTKMM
