/* The SmartHomeProject.org
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
 * Asynchronous message bus subsystem
 */

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

#include "shp-bus.h"

G_DEFINE_TYPE (ShpBus, shp_bus, G_TYPE_OBJECT);

static void shp_bus_finalize (GObject * object);

static void
shp_bus_class_init (ShpBusClass * klass)
{
  GObjectClass *gobject_class;

  gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->finalize = shp_bus_finalize;
}

static void
shp_bus_init (ShpBus * self)
{
}

static void
shp_bus_finalize (GObject * object)
{
  ShpBus *bus = SHP_BUS (object);
  if (bus->data != NULL && bus->notify != NULL) {
    bus->notify (bus->data);
    bus->data = NULL;
  }
}

ShpBus*
shp_bus_new ()
{
  return g_object_new (SHP_BUS_TYPE, NULL);
}

gboolean
shp_bus_post (ShpBus *bus, ShpMessage *message)
{
  g_debug ("posted message: %s", shp_message_get_name (message));

  if (bus->func == NULL) {
    g_object_unref (message);
    return FALSE;
  }

  bus->func (bus, message, bus->data);
  g_object_unref (message);

  return TRUE;
}

void
shp_bus_set_sync_handler (ShpBus *bus, ShpBusSyncHandler func,
    gpointer user_data, GDestroyNotify notify)
{
  if (bus->data != NULL && bus->notify != NULL) {
    bus->notify (bus->data);
    bus->data = NULL;
  }
  bus->func = func;
  bus->data = user_data;
  bus->notify = notify;
}
