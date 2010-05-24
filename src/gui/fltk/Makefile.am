
EXTRA_PROGRAMS = fltk.so

fltk_so_SOURCES = \
	apb-fltk.cpp apb-fltk.h

fltk_so_CXXFLAGS = \
	$(APB_CXXFLAGS) \
	$(FLTK_CXXFLAGS) \
	$(LADCCA_CFLAGS)

fltk_so_LDFLAGS = \
	$(FLTK_LDFLAGS) \
	-shared

uiplugindir = $(pkglibdir)/ui


PLUGINS = 

if FLTK
uiplugin_PROGRAMS = fltk.so
endif #FLTK
