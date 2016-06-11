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
#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <gmodule.h>

#include "shp-plugin-factory.h"

typedef void (* PluginRegisterFunc) ();

/* load single plugin */
static gboolean
load_plugin (gchar * path)
{
  PluginRegisterFunc plugin_register;
  GModule *module;

  g_debug ("loading module: %s", path);

  module = g_module_open (path, G_MODULE_BIND_LAZY);
  if (!module) {
    g_critical ("unable to load module %s", path);
    return FALSE;
  }
  if (!g_module_symbol (module, "shp_plugin_register",
      (gpointer *)&plugin_register)) {
    g_critical ("unable to find shp_register_plugin symbol in %s", path);
    return FALSE;
  }
  if (!plugin_register) {
    g_critical ("unable to find shp_register_plugin symbol in %s", path);
    return FALSE;
  }

  plugin_register ();
  return TRUE;
}

/* load all available plugins from plugin_dir */
static gboolean
load_plugins (gchar * plugin_dir)
{
  GDir *dir;
  GError *error;
  const gchar *filename;
  guint num_plugins = 0;

  dir = g_dir_open (plugin_dir, 0, &error);
  if (dir == NULL) {
    g_critical ("failed to open plugin directory: %s, reason: %s", plugin_dir,
        error->message);
    g_clear_error (&error);
    return FALSE;
  }

  while ((filename = g_dir_read_name (dir))) {
    gchar *path = g_strdup_printf ("%s%s", plugin_dir, filename);

    if (!g_str_has_suffix (path, ".so")) {
      g_free (path);
      continue;
    }

    if (!load_plugin (path)) {
      g_critical ("could not load plugin: %s", path);
      g_dir_close (dir);
      return FALSE;
    } else
      num_plugins++;

    g_free (path);
  }

  if (num_plugins == 0) {
    g_critical ("could not load any plugins");
    g_dir_close (dir);
    return FALSE;
  }

  g_debug ("number of plugins loaded: %d", num_plugins);
  g_dir_close (dir);

  return TRUE;
}

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

  if (!shp_plugin_factory_setup ()) {
    g_critical ("unable to set up plugin factory");
    exit (1);
  }

  /* read plugin dir and load all available plugins */
  file = g_key_file_new ();
  if (!g_key_file_load_from_file (file, file_name, G_KEY_FILE_NONE, NULL)) {
    g_warning ("unable to load config file %s", file_name);
    g_key_file_free (file);
    exit (1);
  }

  plugin_dir = g_key_file_get_string (file, "program", "PluginDir", NULL);
  g_debug ("plugin dir from configuration file: %s", plugin_dir);

  if (!load_plugins (plugin_dir)) {
    g_critical ("unable to load plugins");
    g_key_file_free (file);
    exit (1);
  }

  g_key_file_free (file);

  return 0;
}
