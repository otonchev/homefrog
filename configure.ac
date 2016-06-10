AC_INIT([Smart Home Project], 1.0)
AM_INIT_AUTOMAKE
AC_PROG_CC

AC_PROG_RANLIB

AM_INIT_AUTOMAKE([subdir-objects])

PKG_PROG_PKG_CONFIG
PKG_CHECK_MODULES([GLIB], [glib-2.0])
PKG_CHECK_MODULES([GOBJECT], [gobject-2.0])
PKG_CHECK_MODULES([GIOUNIX], [gio-unix-2.0])
PKG_CHECK_MODULES([GMODULE], [gmodule-2.0])

LT_INIT

AC_DEFUN([CHECK_MYSQL_LIB],
[
AC_CHECK_PROGS(
	MYSQL_CONFIG,
	mysql_config
)

	#In case it fails to find mysql exit configure
	if test "x${MYSQL_CONFIG}" != xmysql_config; then
		echo "------------------------------------------"
		echo " The mysql library and header file is      "
		echo " required to build this project. Stopping "
		echo " Check 'config.log' for more information. "
		echo "------------------------------------------"
		(exit 1); exit 1;
	else
		MYSQL_CONFIG_CFLAGS=`$MYSQL_CONFIG --cflags`
		MYSQL_CONFIG_CPPFLAGS=`$MYSQL_CONFIG --include`
		MYSQL_CONFIG_LIBS=`$MYSQL_CONFIG --libs`
		AC_SUBST([MYSQL_CONFIG_CFLAGS])
		AC_SUBST([MYSQL_CONFIG_CPPFLAGS])
		AC_SUBST([MYSQL_CONFIG_LIBS])
	fi
])

CHECK_MYSQL_LIB

AC_OUTPUT(Makefile src/Makefile src/plugins/Makefile)
