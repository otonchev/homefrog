lib_LTLIBRARIES = \
	libds1820digitemp.la \
	librest.la \
	libtelldus.la \
	libscenectl.la \
	libtimer.la \
	libraspberrygpiotemp.la \
	libgrovedust.la

libtelldus_la_SOURCES = \
	shp-telldus.c
libtelldus_la_CFLAGS = \
        $(GLIB_CFLAGS) \
        $(GOBJECT_CFLAGS) \
        -Werror -Wunused -Wall
libtelldus_la_LIBADD=-ltelldus-core
libtelldus_la_LDFLAGS=-module -avoid-version -shared

libgrovedust_la_SOURCES = \
	air_utils.c \
	lngpio.c \
        shp-grovedust.c
libgrovedust_la_CFLAGS = \
        $(GLIB_CFLAGS) \
        $(GOBJECT_CFLAGS) \
        -Werror -Wunused -Wall
libgrovedust_la_LIBADD=
libgrovedust_la_LDFLAGS=-module -avoid-version -shared

librest_la_SOURCES = \
        shp-rest.c
librest_la_CFLAGS = \
        $(GLIB_CFLAGS) \
        $(GOBJECT_CFLAGS) \
        -Werror -Wunused -Wall
librest_la_LIBADD=
librest_la_LDFLAGS=-module -avoid-version -shared

libscenectl_la_SOURCES = \
        shp-scenectl.c
libscenectl_la_CFLAGS = \
        $(GLIB_CFLAGS) \
        $(GOBJECT_CFLAGS) \
        -Werror -Wunused -Wall
libscenectl_la_LIBADD=
libscenectl_la_LDFLAGS=-module -avoid-version -shared

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
	shp-timer-complextype.c \
        shp-timer.c
libtimer_la_CFLAGS = \
        $(GLIB_CFLAGS) \
        $(GOBJECT_CFLAGS) \
        -Werror -Wunused -Wall
libtimer_la_LIBADD=
libtimer_la_LDFLAGS=-module -avoid-version -shared

libraspberrygpiotemp_la_SOURCES = \
	shp-base1wire.c \
        shp-raspberrygpiotemp.c
libraspberrygpiotemp_la_CFLAGS = \
        $(GLIB_CFLAGS) \
        $(GOBJECT_CFLAGS) \
        -Werror -Wunused -Wall
libraspberrygpiotemp_la_LIBADD=
libraspberrygpiotemp_la_LDFLAGS=-module -avoid-version -shared

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
