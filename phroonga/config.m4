dnl
dnl phroonga: the PHP groonga extension
dnl

PHP_ARG_ENABLE(phroonga, [whether to enable groonga functions],
[  --enable-phroonga       Enable groonga functions support], yes, yes)
PHP_ARG_WITH(groonga, [groonga installation prefix],
[  --with-groonga=PATH     groonga installation prefix (default: use pkg-config)], yes)

if test "$PHP_PHROONGA" != "no"; then

  if test -z "$AWK"; then
    AC_PATH_PROGS(AWK, awk gawk nawk, [no])
  fi
  if test -z "$SED"; then
    AC_PATH_PROGS(SED, sed gsed, [no])
  fi

  dnl
  dnl Check PHP version
  dnl
  export OLD_CPPFLAGS="$CPPFLAGS"
  export CPPFLAGS="$CPPFLAGS $INCLUDES"
  AC_MSG_CHECKING([PHP version])
  AC_TRY_COMPILE([#include <php_version.h>], [
#if !defined(PHP_VERSION_ID) || PHP_VERSION_ID < 50300
#error this extension requires at least PHP version 5.3.0
#endif
],
    [AC_MSG_RESULT([ok])],
    [AC_MSG_ERROR([need at least PHP 5.3.0])])
  export CPPFLAGS="$OLD_CPPFLAGS"

  dnl
  dnl Check with pkg-config
  dnl
  if test "$PHP_GROONGA" = "yes"; then
    dnl
    dnl Check the location of pkg-config
    dnl
    if test -z "$PKG_CONFIG"; then
      AC_PATH_PROGS(PKG_CONFIG, pkg-config, [no])
    fi

    dnl
    dnl Get the version number, CFLAGS and LIBS by pkg-config
    dnl
    AC_MSG_CHECKING([for groonga pkgconfig])

    GROONGA_VERSION=`"$PKG_CONFIG" --modversion groonga 2> /dev/null`
    GROONGA_INCLINE=`"$PKG_CONFIG" --cflags-only-I groonga 2> /dev/null`
    GROONGA_LIBLINE=`"$PKG_CONFIG" --libs groonga 2> /dev/null`

    if test -z "$GROONGA_VERSION"; then
      AC_MSG_ERROR([groonga.pc not found in PKG_CONFIG_PATH])
    fi
  else
    dnl
    dnl Check the location of groonga
    dnl
    if test -x "$PHP_GROONGA/bin/groonga"; then
      GROONGA_BIN="$PHP_GROONGA/bin/groonga"
    else
      AC_MSG_ERROR([groonga executable not found in $PHP_GROONGA/bin])
    fi
    if test -f "$PHP_GROONGA/include/groonga/groonga.h"; then
      GROONGA_INCLINE="-I$PHP_GROONGA/include/groonga"
    elif test -f "$PHP_GROONGA/include/groonga.h"; then
      GROONGA_INCLINE="-I$PHP_GROONGA/include"
    else
      AC_MSG_ERROR([groonga.h header not found in $PHP_GROONGA/include])
    fi
    GROONGA_LIBLINE="-L$PHP_GROONGA/lib -lgroonga"
    GROONGA_VERSION=`"$GROONGA_BIN" --version | head -n1 | "$SED" -E 's/^groonga[ ](.*)[ ].*/\1/'`
  fi

  dnl
  dnl Check the version number
  dnl
  AC_MSG_CHECKING([for groonga version])
  GROONGA_VERSION_ID=`echo "$GROONGA_VERSION" | "$AWK" -F. '{ printf "%d", $1 * 1000000 + $2 * 1000 + $3 }'`
  if test "$GROONGA_VERSION_ID" -lt 2000009; then
    AC_MSG_RESULT([$GROONGA_VERSION])
    AC_MSG_ERROR([groonga version 2.0.9 or later is required to compile php with groonga support])
  fi

  AC_DEFINE_UNQUOTED(PRN_GROONGA_VERSION_ID, $GROONGA_VERSION_ID, [groonga library version number])
  AC_DEFINE_UNQUOTED(PRN_GROONGA_VERSION, "$GROONGA_VERSION", [groonga library version string])
  AC_MSG_RESULT([$GROONGA_VERSION (ok)])

  dnl
  dnl Check the headers and types
  dnl
  export OLD_CPPFLAGS="$CPPFLAGS"
  export CPPFLAGS="$CPPFLAGS $GROONGA_INCLINE"
  AC_CHECK_HEADER([groonga.h], [], AC_MSG_ERROR([groonga.h header not found]))
  export CPPFLAGS="$OLD_CPPFLAGS"

  dnl
  dnl Check the library
  dnl
  PHP_CHECK_LIBRARY(groonga, grn_get_version,
    [
      PHP_EVAL_INCLINE($GROONGA_INCLINE)
      PHP_EVAL_LIBLINE($GROONGA_LIBLINE, PHROONGA_SHARED_LIBADD)
    ],[
      AC_MSG_ERROR([wrong groonga library version or lib not found. Check config.log for more information])
    ],[
      $GROONGA_LIBLINE
    ])

  PHROONGA_SOURCES="phroonga.c prn_constants.c prn_ini.c prn_resource.c"
  PHROONGA_SOURCES="$PHROONGA_SOURCES ctx.c obj.c"
  PHROONGA_SOURCES="$PHROONGA_SOURCES geo.c log.c expr.c"
  PHROONGA_SOURCES="$PHROONGA_SOURCES hash.c array.c pat.c dat.c"

  PHP_SUBST(PHROONGA_SHARED_LIBADD)
  PHP_NEW_EXTENSION(phroonga, $PHROONGA_SOURCES, $ext_shared)
fi
