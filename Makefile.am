SUBDIRS = m4 src

EXTRA_DIST = Doxyfile.in alsa-patch-bay.desktop jack-patch-bay.desktop

CLEANFILES = stamp-doxygen

dox: stamp-doxygen

stamp-doxygen: $(shell find -name "*.cpp" -or -name "*.h")
	$(DOXYGEN) Doxyfile
	touch stamp-doxygen

distclean-local:
	rm -rf doc

desktopdir = $(APB_DESKTOP_PREFIX)/applications
desktop_DATA = alsa-patch-bay.desktop jack-patch-bay.desktop
