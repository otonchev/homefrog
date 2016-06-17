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
#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

#include "shp-plugin-factory.h"

int
main (int argc, char *argv[])
{
  GKeyFile *file;
  gchar *plugin_dir;
  gchar *file_name;

  if (argc != 2) {
    g_critical ("usage: %s path_to_shp_conf_file", argv[0]);
    exit (1);
  }

  file_name = argv[1];

  g_debug ("using config: %s", file_name);

  /* read plugin dir and load all available plugins */
  file = g_key_file_new ();
  if (!g_key_file_load_from_file (file, file_name, G_KEY_FILE_NONE, NULL)) {
    g_warning ("unable to load config file %s", file_name);
    g_key_file_free (file);
    exit (1);
  }

  plugin_dir = g_key_file_get_string (file, "program", "PluginDir", NULL);
  g_debug ("plugin dir from configuration file: %s", plugin_dir);

  if (!shp_plugin_factory_setup (plugin_dir)) {
    g_critical ("unable to set up plugin factory");
    exit (1);
  }

  g_key_file_free (file);

  return 0;
}
