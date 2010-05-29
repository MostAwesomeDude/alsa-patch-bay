AC_INIT(ALSA Patch Bay, 1.0.0)
AC_CONFIG_SRCDIR(Doxyfile.in)
AM_INIT_AUTOMAKE(1.7)
AM_CONFIG_HEADER(config.h)
AM_MAINTAINER_MODE
AC_LANG([C++])

AC_PROG_CXX
AC_PROG_LIBTOOL


# *********************
# ******* fltk ********
# *********************
apb_enable_fltk="yes"
AC_ARG_ENABLE(fltk,
[  --disable-fltk          disable fltk user interface],[
  case "$enableval" in
    "yes")
      ;;
    "no")
      apb_enable_fltk="no"
      ;;
    *)
      AC_MSG_ERROR([must use --enable-fltk(=yes/no) or --disable-fltk])
      ;;
  esac
])

if test "$apb_enable_fltk" = "yes"; then
  AC_LIB_FLTK(1.1)
  if test "$FLTK_FOUND" = "yes"; then
    AC_DEFINE(HAVE_FLTK, 1, [whether or not we have fltk])
  fi
else
  FLTK_FOUND="no"
fi
AM_CONDITIONAL(FLTK, test "$FLTK_FOUND" = "yes")

# **********************
# ******* GTKmm ********
# **********************
apb_enable_gtkmm="yes"
AC_ARG_ENABLE(gtkmm,
[  --disable-gtkmm         disable gtkmm user interface],[
  case "$enableval" in
    "yes")
      ;;
    "no")
      apb_enable_gtkmm="no"
      ;;
    *)
      AC_MSG_ERROR([must use --enable-gtkmm(=yes/no) or --disable-gtkmm])
      ;;
  esac
])

if test "$apb_enable_gtkmm" = "no"; then
  if test "$apb_enable_fltk" = "no"; then
    AC_MSG_ERROR([don't be silly; you can't disable both UIs])
  fi
fi

if test "$apb_enable_gtkmm" = "yes"; then
  PKG_CHECK_MODULES(GTKMM, gtkmm-2.0 >= 2.0.0, [
    GTKMM_FOUND="yes"
    ], [
    if test "$FLTK_FOUND" = "no"; then
      AC_MSG_ERROR([either FLTK >= 1.1 or GTKmm >= 2.0.0 is needed])
    fi
    GTKMM_FOUND="no"
  ])
  AC_SUBST(GTKMM_CFLAGS)
  AC_SUBST(GTKMM_LIBS)

  if test "$GTKMM_FOUND" = "yes"; then
    AC_DEFINE(HAVE_GTKMM, 1, [whether or not we have gtkmm])
  fi
else
  GTKMM_FOUND="no"
fi
AM_CONDITIONAL(GTKMM, test "$GTKMM_FOUND" = "yes")


dnl *********************
dnl ******* ALSA ********
dnl *********************
apb_enable_alsa="yes"
AC_ARG_ENABLE(alsa,
[  --disable-alsa          disable alsa driver],[
  case "$enableval" in
    "yes")
      ;;
    "no")
      apb_enable_alsa="no"
      ;;
    *)
      AC_MSG_ERROR([must use --enable-alsa(=yes/no) or --disable-alsa])
      ;;
  esac
])

if test "$apb_enable_alsa" = "yes"; then
  AC_CHECK_HEADER(alsa/asoundlib.h,
    AC_CHECK_LIB(asound, snd_seq_open, ALSA_FOUND="yes", ALSA_FOUND="no", -lm -ldl -lpthread),
    ALSA_FOUND="no")

  if test "$ALSA_FOUND" = "yes"; then
    AC_DEFINE(HAVE_ALSA, 1, [whether or not we have alsa])
    ALSA_CFLAGS=""
    ALSA_LIBS="-lasound -lm -ldl -lpthread"
    AC_SUBST(ALSA_CFLAGS)
    AC_SUBST(ALSA_LIBS)
  fi
else
  ALSA_FOUND="no"
fi
AM_CONDITIONAL(ALSA, test "$ALSA_FOUND" = "yes")


dnl *********************
dnl ******* JACK ********
dnl *********************
apb_enable_jack="yes"
AC_ARG_ENABLE(jack,
[  --disable-jack          disable jack driver],[
  case "$enableval" in
    "yes")
      ;;
    "no")
      apb_enable_jack="no"
      ;;
    *)
      AC_MSG_ERROR([must use --enable-jack(=yes/no) or --disable-jack])
      ;;
  esac
])


if test "$apb_enable_alsa" = "no"; then
  if test "$apb_enable_jack" = "no"; then
    AC_MSG_ERROR([don't be silly; you can't disable both drivers])
  fi
fi

if test "$apb_enable_jack" = "yes"; then
  PKG_CHECK_MODULES(JACK, jack >= 0.40.0, [
    JACK_FOUND="yes"
    ], [
    if test "$ALSA_FOUND" = "no"; then
      AC_MSG_ERROR([either ALSA or JACK >= 0.40.0 is needed])
    fi
    JACK_FOUND="no"
  ])
  AC_SUBST(JACK_CFLAGS)
  AC_SUBST(JACK_LIBS)

  if test "$JACK_FOUND" = "yes"; then
    AC_DEFINE(HAVE_JACK, 1, [whether or not we have jack])
  fi
else
  JACK_FOUND="no"
fi
AM_CONDITIONAL(JACK, test x$JACK_FOUND = xyes)


dnl ***********************
dnl ******* LADCCA ********
dnl ***********************
apb_enable_ladcca="yes"
AC_ARG_ENABLE(ladcca,
[  --disable-ladcca        disable LADCCA support],[
  case "$enableval" in
    "yes")
      ;;
    "no")
      apb_enable_ladcca="no"
      ;;
    *)
      AC_MSG_ERROR([must use --enable-ladcca(=yes/no) or --disable-ladcca])
      ;;
  esac
])

if test "$apb_enable_ladcca" = "yes"; then
  PKG_CHECK_MODULES(LADCCA, ladcca-1.0 >= 0.4.0, LADCCA_FOUND="yes", LADCCA_FOUND="no")
  AC_SUBST(LADCCA_CFLAGS)
  AC_SUBST(LADCCA_LIBS)

  if test "$LADCCA_FOUND" = "yes"; then
     AC_DEFINE(HAVE_LADCCA, 1, [whether or not we are supporting ladcca])
  fi
else
  LADCCA_FOUND="no"
fi
AM_CONDITIONAL(LADCCA, test "$LADCCA_FOUND" = "yes")


dnl use the source, luke
AC_CHECK_LIB(corporation, morality)

dnl ************************
dnl ******* doxygen ********
dnl ************************
AC_ARG_VAR(DOXYGEN, Doxygen)
AC_PATH_PROG(DOXYGEN, doxygen, no)

dnl *************************
dnl ******* graphviz ********
dnl *************************
AC_ARG_VAR(DOT, [The "dot" program from graphviz])
AC_PATH_PROG(DOT, dot, no)
if test "$DOT" != "no"; then
  HAVEDOT=YES
  DOTPATH=$( dirname "$DOT" )
else
  HAVEDOT=NO
fi
AC_SUBST(HAVEDOT)
AC_SUBST(DOTPATH)

dnl *************************
dnl ******* graphviz ********
dnl *************************
AC_ARG_VAR(PERL, Perl)
AC_PATH_PROG(PERL, perl, no)
if test "$PERL" == "no"; then
  AC_MSG_WARN([you don't have perl installed!  wierdo!  (and doxygen might not like this)])
fi

dnl ***********************
dnl *** desktop install ***
dnl ***********************
apb_desktop_install="/usr/share"
AC_ARG_ENABLE(desktop-inst,
[  --disable-desktop-inst  don't install pixmap and .desktop to /usr/share],[
  case "$enableval" in
    "yes")
      ;;
    "no")
      apb_desktop_install="no"
      ;;
    *)
      AC_MSG_ERROR([must use --enable-desktop-inst(=yes/no) or --disable-desktop-inst])
      ;;
  esac
])

if test "x$apb_desktop_install" = xno; then
  if test "x$prefix" != xNONE; then
    APB_DESKTOP_PREFIX="$( eval echo $datadir )"
  else
    APB_DESKTOP_PREFIX="/usr/local/share"
  fi
else
  APB_DESKTOP_PREFIX="$apb_desktop_install"
fi
AC_SUBST(APB_DESKTOP_PREFIX)


APB_CXXFLAGS='-g -D_REENTRANT -fexceptions -I$(top_srcdir)/src -Wall'
AC_SUBST(APB_CXXFLAGS)

APB_LDFLAGS=""
AC_SUBST(APB_LDFLAGS)


AC_OUTPUT([
Makefile
m4/Makefile
src/Makefile
src/driver/Makefile
src/driver/alsa/Makefile
src/driver/jack/Makefile
src/gui/Makefile
src/gui/gtkmm/Makefile
src/gui/fltk/Makefile
Doxyfile
])

AC_MSG_NOTICE([

  ALSA Patch Bay configured

  GTKmm interface:      $GTKMM_FOUND
  FLTK interface:       $FLTK_FOUND
  ALSA driver:          $ALSA_FOUND
  JACK driver:          $JACK_FOUND
  LADCCA support:       $LADCCA_FOUND

  Installation prefix:  $prefix
  Desktop files prefix: $APB_DESKTOP_PREFIX

])
