#
# fltk.m4
#
# You have permission to use this file under GNU's General Public License,
# version 2 or later
#
# Copyright (C) 2002 Robert Ham (node@users.sourceforge.net)
#

AC_DEFUN([AC_LIB_FLTK],[
  AC_LANG_PUSH([C++])
  AC_ARG_ENABLE(
    [fltktest],
    AC_HELP_STRING([--disable-fltktest],
                   [assume fltk is installed and skip the test]),
    [
      case "$enableval" in
        "yes")
          FLTK_TEST="yes"
          ;;
        "no")
          FLTK_TEST="no"
          ;;
        *)
          AC_MSG_ERROR([must use --enable-fltktest(=yes/no) or --disable-fltktest])
          ;;
      esac
    ],
    [ FLTK_TEST="yes"
    ])
                
  if test "$FLTK_TEST" = "no"; then
    AC_MSG_WARN([fltk test disabled: assuming FLTK_LDFLAGS="-lfltk" and FLTK_CXXFLAGS=""])
    FLTK_LDFLAGS="-lfltk"
    FLTK_CXXFLAGS=""
    FLTK_FOUND="yes"
  else
    AC_CACHE_VAL(
      [fltk_cv_prog_fltkconfig],
      [
        AC_ARG_WITH(
          [fltk-config],
          AC_HELP_STRING([--with-fltk-config=DIR],
                         [the directory containing the fltk-config program]),
          [
            case "$withval" in
              "yes")
                if test -d yes; then
                  FLTK_CONFIG_PATH="yes"
                else
                  AC_MSG_ERROR([you must use --with-fltk-config=DIR with DIR as a directory name])
                fi
                ;;
              "no")
                AC_MSG_ERROR([you must use --with-fltk-config=DIR with DIR as a directory name])
                ;;
              *)
                FLTK_CONFIG_PATH="$withval"
                ;;
            esac
          ])

        if test "$FLTK_CONFIG_PATH" = ""; then
          AC_PATH_PROG([FLTK_CONFIG], [fltk-config], [no])
        else
          AC_PATH_PROG([FLTK_CONFIG], [fltk-config], [no], "${FLTK_CONFIG_PATH}:${PATH}")
        fi

        if test "$FLTK_CONFIG" = "no"; then
          AC_MSG_WARN([could not find the fltk-config program - try using --with-fltk-config=DIR])
          FLTK_FOUND="no"
        else
          fltk_needed_version=ifelse([$1], ,no,$1)
          if test x$fltk_needed_version != xno; then
            fltk_config_version="$( "$FLTK_CONFIG" --api-version )"
            AC_MSG_CHECKING([for fltk == $fltk_config_version])
            if test x$fltk_config_version = x$fltk_needed_version; then
              AC_MSG_RESULT([$fltk_config_version (ok)])
              FLTK_FOUND="yes"
            else
              AC_MSG_RESULT([$fltk_config_version (not ok)])
              FLTK_FOUND="no"
            fi
          fi
        fi

        fltk_cv_prog_fltkconfig="$FLTK_CONFIG"
      ])

    FLTK_CXXFLAGS="$( "$fltk_cv_prog_fltkconfig" --cxxflags )"
    FLTK_LDFLAGS="$( "$fltk_cv_prog_fltkconfig" --ldflags )"
    FLTK_FOUND="yes"
    
  fi
  AC_LANG_POP([C++])

  AC_SUBST(FLTK_CXXFLAGS)
  AC_SUBST(FLTK_LDFLAGS)
])
