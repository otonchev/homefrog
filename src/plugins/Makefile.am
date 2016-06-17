lib_LTLIBRARIES = \
	libds1820digitemp.la \
	libtelldus.la \
	libtimer.la
#	libmysql.la \
#	libraspberrygpio.la \
#	libgrovedust.la \
#	libdummy.la
#
libtelldus_la_SOURCES = \
	shp-telldus.c
libtelldus_la_CFLAGS = \
        $(GLIB_CFLAGS) \
        $(GOBJECT_CFLAGS) \
        -Werror -Wunused -Wall
libtelldus_la_LIBADD=-ltelldus-core
libtelldus_la_LDFLAGS=-module -avoid-version -shared
#
#libmysql_la_SOURCES = \
#        shp-mysql.c
#libmysql_la_CFLAGS = \
#        $(GLIB_CFLAGS) \
#        $(GOBJECT_CFLAGS) \
#        $(MYSQL_CONFIG_CFLAGS) \
#        -Werror -Wunused -Wall
#libmysql_la_LIBADD=$(MYSQL_CONFIG_LIBS)
#libmysql_la_LDFLAGS=-module -avoid-version -shared

libtimer_la_SOURCES = \
        shp-timer.c
libtimer_la_CFLAGS = \
        $(GLIB_CFLAGS) \
        $(GOBJECT_CFLAGS) \
        -Werror -Wunused -Wall
libtimer_la_LIBADD=
libtimer_la_LDFLAGS=-module -avoid-version -shared

#libraspberrygpio_la_SOURCES = \
#	shp-base1wire.c \
#        shp-raspberrygpio.c
#libraspberrygpio_la_CFLAGS = \
#        $(GLIB_CFLAGS) \
#        $(GOBJECT_CFLAGS) \
#        -Werror -Wunused -Wall
#libraspberrygpio_la_LIBADD=$(MYSQL_CONFIG_LIBS)
#libraspberrygpio_la_LDFLAGS=-module -avoid-version -shared
#
libds1820digitemp_la_SOURCES = \
	shp-base1wire.c \
        shp-ds1820digitemp.c
libds1820digitemp_la_CFLAGS = \
        $(GLIB_CFLAGS) \
        $(GOBJECT_CFLAGS) \
        -Werror -Wunused -Wall
libds1820digitemp_la_LIBADD=
libds1820digitemp_la_LDFLAGS=-module -avoid-version -shared

#libgrovedust_la_SOURCES = \
#	air_utils.c \
#	lngpio.c \
#        shp-grovedust.c
#libgrovedust_la_CFLAGS = \
#        $(GLIB_CFLAGS) \
#        $(GOBJECT_CFLAGS) \
#        -Werror -Wunused -Wall
#libgrovedust_la_LIBADD=-lm
#libgrovedust_la_LDFLAGS=-module -avoid-version -shared
#
#libdummy_la_SOURCES = \
#        shp-dummy.c
#libdummy_la_CFLAGS = \
#        $(GLIB_CFLAGS) \
#        $(GOBJECT_CFLAGS) \
#        -Werror -Wunused -Wall
#libdummy_la_LIBADD=
#libdummy_la_LDFLAGS=-module -avoid-version -shared
