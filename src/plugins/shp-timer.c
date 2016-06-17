/* homefrog
 * Copyright (C) 2016  Ognyan Tonchev <otonchev at gmail.com >
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
 * This is a base class for plugins obtaining readings from a 1-Wire Serial
 * network
 */

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <time.h>

#include "../shp-message.h"
#include "../shp-component.h"
#include "../shp-plugin-factory.h"
#include "shp-timer.h"

#define NAME "timer"

static void status_update (ShpPlugin * plugin);

static void shp_timer_finalize (GObject * object);

static void shp_timer_get_property (GObject * object, guint propid,
    GValue * value, GParamSpec * pspec);
static void shp_timer_set_property (GObject * object, guint propid,
    const GValue * value, GParamSpec * pspec);

static gboolean shp_timer_start (ShpComponent * component);
static gboolean shp_timer_stop (ShpComponent * component);

G_DEFINE_TYPE (ShpTimer, shp_timer, SHP_PLUGIN_TYPE);

static void
shp_timer_class_init (ShpTimerClass * klass)
{
  GObjectClass *gobject_class;

  gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->finalize = shp_timer_finalize;
  gobject_class->set_property = shp_timer_set_property;
  gobject_class->get_property = shp_timer_get_property;

  SHP_COMPONENT_CLASS (klass)->start = shp_timer_start;
  SHP_COMPONENT_CLASS (klass)->stop = shp_timer_stop;
}

static void
shp_timer_get_property (GObject * object, guint propid, GValue * value,
    GParamSpec * pspec)
{
  //ShpTimer *self = SHP_TIMER (object);

  switch (propid) {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, propid, pspec);
  }
}

static void
shp_timer_set_property (GObject * object, guint propid,
    const GValue * value, GParamSpec * pspec)
{
  //ShpTimer *self = SHP_TIMER (object);

  switch (propid) {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, propid, pspec);
  }
}

static void
shp_timer_init (ShpTimer * self)
{
  g_signal_connect (G_OBJECT (self), "status-update",
      G_CALLBACK (status_update), NULL);
}

static void
shp_timer_finalize (GObject * object)
{
}

static gboolean
send_status (ShpTimer * self)
{
  ShpComponent *component = SHP_COMPONENT (self);
  ShpMessage *msg;
  time_t t = time (NULL);
  struct tm *tm_struct = localtime (&t);

  g_debug ("timer: sending status update");

  msg = shp_message_new (shp_component_get_path (component));
  shp_message_add_integer (msg, "hour", tm_struct->tm_hour);
  shp_message_add_integer (msg, "minutes", tm_struct->tm_min);
  shp_message_add_integer (msg, "seconds", tm_struct->tm_sec);
  shp_message_add_integer (msg, "year", tm_struct->tm_year + 1900);
  shp_message_add_integer (msg, "month", tm_struct->tm_mon + 1);
  shp_message_add_integer (msg, "day", tm_struct->tm_mday);
  shp_message_add_integer (msg, "week_day", tm_struct->tm_wday + 1);

  if (!shp_component_post_message (component, msg)) {
    g_warning ("timer: could not post message on bus");
    return FALSE;
  }

  return TRUE;
}

static void
status_update (ShpPlugin * plugin)
{
  if (!send_status (SHP_TIMER (plugin)))
    g_warning ("timer: unable to send status update");
}

static gboolean
dispatch_cb (gpointer user_data)
{
  if (!send_status (SHP_TIMER (user_data)))
    g_warning ("timer: unable to send status update");

  return TRUE;
}

static gboolean
shp_timer_start (ShpComponent * component)
{
  ShpTimer *self = SHP_TIMER (component);

  self->dispatch_source = g_timeout_source_new_seconds (1);
  g_source_set_callback (self->dispatch_source, dispatch_cb,
      g_object_ref (self), g_object_unref);
  g_source_attach (self->dispatch_source, NULL);

  return TRUE;
}

static gboolean
shp_timer_stop (ShpComponent * component)
{
  ShpTimer *self = SHP_TIMER (component);

  g_source_destroy (self->dispatch_source);
  g_source_unref (self->dispatch_source);
  self->dispatch_source = NULL;

  return TRUE;
}

static void
plugin_register (void)
{
  g_debug ("%s: loading plugin", NAME);
  shp_plugin_factory_register (NAME, SHP_TIMER_TYPE);
}

SHP_PLUGIN_REGISTER (plugin_register);
