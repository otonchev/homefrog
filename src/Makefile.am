bin_PROGRAMS = homefrog

#DBUS_GENERATED_C = radio-dbus-gen.c 1wire-dbus-gen.c shp-dbus-gen.c
#DBUS_GENERATED_H = radio-dbus-gen.h 1wire-dbus-gen.h shp-dbus-gen.h

GENERATED_C = #$(DBUS_GENERATED_C)
GENERATED_H = #$(DBUS_GENERATED_H)

BUILT_SOURCES = $(GENERATED_C) $(GENERATED_H)

#%-dbus-gen.c %-dbus-gen.h: %.xml
#	gdbus-codegen --interface-prefix com.shp. --c-namespace ShpDBus \
#		--generate-c-code $(basename $@) $<

homefrog_SOURCES = \
	main.c \
	shp-bus.c \
	shp-complextype-factory.c \
	shp-component.c \
	shp-controller.c \
	shp-condition.c \
	shp-group.c \
	shp-message.c \
	shp-plugin.c \
	shp-plugin-factory.c \
	shp-rule.c \
	shp-scene.c \
	shp-slave-plugin.c \
	shp-complextype.c \
	shp-complextype-compare.c

nodist_shp_SOURCES = $(GENERATED_C)
noinst_HEADERS = $(GENERATED_H)

homefrog_CFLAGS = \
	$(GLIB_CFLAGS) \
	$(GIOUNIX_CFLAGS) \
	$(GOBJECT_CFLAGS) \
	$(GMODULE_CFLAGS) \
	-Werror -Wunused -Wall -g -O0
homefrog_LDADD = \
	$(GLIB_LIBS) \
	$(GIOUNIX_LIBS) \
	$(GOBJECT_LIBS) \
        $(GMODULE_LIBS)
SUBDIRS = plugins
