uiplugindir = $(pkglibdir)/ui

if FLTK
uiplugin_LTLIBRARIES = fltk.la
endif #FLTK

fltk_la_SOURCES = \
	apb-fltk.cpp apb-fltk.h

fltk_la_CXXFLAGS = \
	$(APB_CXXFLAGS) \
	$(FLTK_CXXFLAGS) \
	$(LADCCA_CFLAGS) \
	-fpic

fltk_la_LDFLAGS = \
	$(FLTK_LDFLAGS) \
	-shared \
	-module


PLUGINS = 
