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
 * This module extracts temperature readings from 1-wire network attached
 * directly to Raspberry Pi's GPIOTEMP.
 *
 * cd /sys/bus/w1/devices
 * cd 10-000801b5*
 * cat w1_slave
 * 0f 00 4b 46 ff ff 06 10 0c : crc=0c YES
 * 0f 00 4b 46 ff ff 06 10 0c t=7375
 *
 * This is the configuration specification for this plugin:
 *
 * Config spec:
 *
 * Properties inherited from baseplugin:
 *
 * device-id=<string> the name of the device from the /dev filesystem:
 *                    /sys/bus/w1/devices/<device_id>/w1_slave
 *
 * Properties for this plugin:
 *
 * <None>
 */

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <errno.h>

#include "../shp-plugin-factory.h"
#include "shp-raspberrygpiotemp.h"

#define NAME "raspberrygpiotemp"
#define DEVICE_PATH "/sys/bus/w1/devices"
#define DEVICE_NAME "w1_slave"

static gfloat shp_raspberrygpiotemp_read_sensor_data (ShpBase1wire * self,
    const gchar * id);

G_DEFINE_TYPE (ShpRaspberrygpiotemp, shp_raspberrygpiotemp, SHP_BASE1WIRE_TYPE);

static void
shp_raspberrygpiotemp_class_init (ShpRaspberrygpiotempClass * klass)
{
  ShpBase1wireClass *base1wire_class;

  base1wire_class = SHP_BASE1WIRE_CLASS (klass);

  base1wire_class->read_sensor_data = shp_raspberrygpiotemp_read_sensor_data;
}

static void
shp_raspberrygpiotemp_init (ShpRaspberrygpiotemp * self)
{
}

static gfloat
shp_raspberrygpiotemp_read_sensor_data (ShpBase1wire * self, const gchar * id)
{
  FILE *file;
  ssize_t read;
  gchar *line = NULL;
  size_t len = 0;
  gchar *tmp;
  gchar *end;
  gfloat reading;

  gchar *name = g_strdup_printf ("%s%s/%s", DEVICE_PATH, id, DEVICE_NAME);
  g_debug ("device file name: %s", name);

  file = fopen (name, "r");
  if (file == NULL) {
    g_warning ("unable to open file: %s", name);
    g_free (name);
    return SHP_BASE1WIRE_INVALID_READING;
  }
  g_free (name);

  read = getline (&line, &len, file);
  if (read == -1) {
    g_warning ("unable to read from file: %s", id);
    g_free (line);
    return SHP_BASE1WIRE_INVALID_READING;
  }
  if (g_strrstr_len (line, len, "YES") == NULL) {
    g_warning ("not a valid reading");
    g_free (line);
    return SHP_BASE1WIRE_INVALID_READING;
  }

  g_free (line);
  len = 0;

  read = getline (&line, &len, file);
  if (read == -1) {
    g_warning ("unable to read from file: %s", id);
    g_free (line);
    return SHP_BASE1WIRE_INVALID_READING;
  }

  tmp = g_strrstr_len (line, len, "t=");
  if (tmp == NULL) {
    g_warning ("failed to parse reading: %s", id);
    g_free (line);
    return SHP_BASE1WIRE_INVALID_READING;
  }
  reading = strtod (tmp + 2, &end);
  if (tmp + 2 == end) {
    g_warning ("failed to parse reading");
    g_free (line);
    return SHP_BASE1WIRE_INVALID_READING;
  }
  g_debug ("sensor reading: %f", reading / 1000);

  g_free (line);

  if (!fclose (file)) {
    g_warning ("could not close file, reason: %s", g_strerror (errno));
  }

  return reading / 1000;
}

void
shp_plugin_register (void)
{
  shp_plugin_factory_register (NAME, SHP_RASPBERRYGPIOTEMP_TYPE);
}
