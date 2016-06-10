/* The SmartHomeProject.org
 * Copyright (C) 2015  Ognyan Tonchev <otonchev at gmail.com >
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * This plugin is used for configuring schedules based on day of week and time.
 *
 * Configure file specification:
 *
 * Properties inherited from baseplugin:
 *
 * interval=<integer>
 *
 * Properties for this plugin:
 *
 * hour=<integer>
 * minutes=<integer>
 * monday=<boolean>
 * tuesday=<boolean>
 * wednesday=<boolean>
 * thursday=<boolean>
 * friday=<boolean>
 * saturday=<boolean>
 * sunday=<boolean>
 */

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

#include "../shp-data.h"
#include "../shp-plugin-factory.h"
#include "shp-dayschedule.h"

#define NAME "dayschedule"

#define DEFAULT_HOUR 0
#define DEFAULT_MINUTES 0
#define DEFAULT_MONDAY FALSE
#define DEFAULT_TUESDAY FALSE
#define DEFAULT_WEDNESDAY FALSE
#define DEFAULT_THURSDAY FALSE
#define DEFAULT_FRIDAY FALSE
#define DEFAULT_SATURDAY FALSE
#define DEFAULT_SUNDAY FALSE

enum
{
  PROP_0,
  PROP_HOUR,
  PROP_MINUTES,
  PROP_MONDAY,
  PROP_TUESDAY,
  PROP_WEDNESDAY,
  PROP_THURSDAY,
  PROP_FRIDAY,
  PROP_SATURDAY,
  PROP_SUNDAY,
  PROP_LAST
};

static gboolean shp_dayschedule_start (ShpPlugin * plugin);
static void shp_dayschedule_stop (ShpPlugin * plugin);
static gboolean shp_dayschedule_ready (ShpPlugin * plugin);

static void shp_dayschedule_get_property (GObject * object, guint propid,
    GValue * value, GParamSpec * pspec);
static void shp_dayschedule_set_property (GObject * object, guint propid,
    const GValue * value, GParamSpec * pspec);

G_DEFINE_TYPE (ShpDayschedule, shp_dayschedule, SHP_PLUGIN_TYPE);

static void
shp_dayschedule_class_init (ShpDayscheduleClass * klass)
{
  GObjectClass *gobject_class;

  gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->set_property = shp_dayschedule_set_property;
  gobject_class->get_property = shp_dayschedule_get_property;

  g_object_class_install_property (gobject_class, PROP_HOUR,
      g_param_spec_int ("hour", "Hour", "Hour", G_MININT,
          G_MAXINT, DEFAULT_HOUR, G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_MINUTES,
      g_param_spec_int ("minutes", "Minutes", "Minutes", G_MININT,
          G_MAXINT, DEFAULT_MINUTES, G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_MONDAY,
      g_param_spec_boolean ("monday", "Trigger on Mondays",
          "Trigger on Mondays", DEFAULT_MONDAY, G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_TUESDAY,
      g_param_spec_boolean ("tuesday", "Trigger on Tuesdays",
          "Trigger on Tuesdayss", DEFAULT_TUESDAY, G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_WEDNESDAY,
      g_param_spec_boolean ("wednesday", "Trigger on Wednesdays",
          "Trigger on Wednesdays", DEFAULT_WEDNESDAY, G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_THURSDAY,
      g_param_spec_boolean ("thursday", "Trigger on Thursday",
          "Trigger on Thursdays", DEFAULT_THURSDAY, G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_FRIDAY,
      g_param_spec_boolean ("friday", "Trigger on Fridays",
          "Trigger on Fridays", DEFAULT_FRIDAY, G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_SATURDAY,
      g_param_spec_boolean ("saturday", "Trigger on Saturdays",
          "Trigger on Saturdays", DEFAULT_SATURDAY, G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_SUNDAY,
      g_param_spec_boolean ("sunday", "Trigger on Sundays",
          "Trigger on Sundays", DEFAULT_SUNDAY, G_PARAM_READWRITE));

  SHP_PLUGIN_CLASS (klass)->start = shp_dayschedule_start;
  SHP_PLUGIN_CLASS (klass)->stop = shp_dayschedule_stop;
}

static void
shp_dayschedule_init (ShpDayschedule * self)
{
}

static void
shp_dayschedule_get_property (GObject * object, guint propid, GValue * value,
    GParamSpec * pspec)
{
  ShpDayschedule *self = SHP_DAYSCHEDULE (object);

  switch (propid) {
    case PROP_MINUTES:
      g_value_set_int (value, self->minutes);
      break;
    case PROP_HOUR:
      g_value_set_int (value, self->hour);
      break;
    case PROP_MONDAY:
      g_value_set_boolean (value, self->monday);
      break;
    case PROP_TUESDAY:
      g_value_set_boolean (value, self->tuesday);
      break;
    case PROP_WEDNESDAY:
      g_value_set_boolean (value, self->wednesday);
      break;
    case PROP_THURSDAY:
      g_value_set_boolean (value, self->thursday);
      break;
    case PROP_FRIDAY:
      g_value_set_boolean (value, self->friday);
      break;
    case PROP_SATURDAY:
      g_value_set_boolean (value, self->saturday);
      break;
    case PROP_SUNDAY:
      g_value_set_boolean (value, self->sunday);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, propid, pspec);
  }
}

static void
shp_dayschedule_set_property (GObject * object, guint propid,
    const GValue * value, GParamSpec * pspec)
{
  ShpDayschedule *self = SHP_DAYSCHEDULE (object);

  switch (propid) {
    case PROP_MINUTES:
      self->minutes = g_value_get_int (value);
      break;
    case PROP_HOUR:
      self->hour = g_value_get_int (value);
      break;
    case PROP_MONDAY:
      self->monday = g_value_get_boolean (value);
      break;
    case PROP_TUESDAY:
      self->tuesday = g_value_get_boolean (value);
      break;
    case PROP_WEDNESDAY:
      self->wednesday = g_value_get_boolean (value);
      break;
    case PROP_THURSDAY:
      self->thursday = g_value_get_boolean (value);
      break;
    case PROP_FRIDAY:
      self->friday = g_value_get_boolean (value);
      break;
    case PROP_SATURDAY:
      self->saturday = g_value_get_boolean (value);
      break;
    case PROP_SUNDAY:
      self->sunday = g_value_get_boolean (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, propid, pspec);
  }
}

static gboolean
dispatch_cb (gpointer user_data)
{
  ShpPlugin *plugin = SHP_PLUGIN (user_data);

  if (shp_dayschedule_ready (plugin)) {
    ShpData *data = shp_data_new ();
    shp_plugin_send (plugin, data);
  }

  return TRUE;
}

static gboolean
shp_dayschedule_start (ShpPlugin * plugin)
{
  ShpDayschedule *self = SHP_DAYSCHEDULE (plugin);

  self->dispatch_source = g_timeout_source_new_seconds (15);
  g_source_set_callback (self->dispatch_source, dispatch_cb,
      g_object_ref (plugin), g_object_unref);
  g_source_attach (self->dispatch_source, NULL);

  return TRUE;
}

static void
shp_dayschedule_stop (ShpPlugin * plugin)
{
  ShpDayschedule *self = SHP_DAYSCHEDULE (plugin);

  g_source_destroy (self->dispatch_source);
  g_source_unref (self->dispatch_source);
  self->dispatch_source = NULL;
}

static gboolean
shp_dayschedule_ready (ShpPlugin * plugin)
{
  time_t t = time (NULL);
  struct tm *tm_struct = localtime (&t);
  gint hour;
  gint minutes;
  ShpDayschedule *self = SHP_DAYSCHEDULE (plugin);

  /* chain up to parent first */
  if (!shp_plugin_check (plugin)) {
    return FALSE;
  }

  g_debug ("%d:%d %d", tm_struct->tm_hour, tm_struct->tm_min,
      tm_struct->tm_wday);

  switch (tm_struct->tm_wday) {
    case 0:
      if (!self->sunday)
        goto done;
      break;
    case 1:
      if (!self->monday)
        goto done;
      break;
    case 2:
      if (!self->tuesday)
        goto done;
      break;
    case 3:
      if (!self->wednesday)
        goto done;
      break;
    case 4:
      if (!self->thursday)
        goto done;
      break;
    case 5:
      if (!self->friday)
        goto done;
      break;
    case 6:
      if (!self->saturday)
        goto done;
      break;
    default:
      g_assert_not_reached ();
      break;
  };

  hour = self->hour;
  minutes = self->minutes;

  if (hour == tm_struct->tm_hour && minutes == tm_struct->tm_min) {
    if (!self->active) {
      self->active = TRUE;
      return TRUE;
    }
    g_debug ("current trigger already signalled");
    return FALSE;
  }

done:
  self->active = FALSE;
  return FALSE;
}

void
shp_plugin_register (void)
{
  shp_plugin_factory_register (NAME, SHP_DAYSCHEDULE_TYPE);
}
