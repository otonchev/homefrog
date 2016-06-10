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
#include "shp-plugin-factory.h"

static GHashTable *plugins;
static GMutex mutex;

/**
 * shp_plugin_factory_create:
 * @name: name of the #ShpPlugin
 *
 * Creates new #ShpPlugin of type @name.
 *
 * Returns: A new #ShpPlugin instance of type @name. Free with
 * g_object_unref() when no longer needed.
 */
ShpPlugin*
shp_plugin_factory_create (gchar * name)
{
  ShpPlugin *plugin = NULL;
  gpointer tmp;
  GType object_type;

  g_return_val_if_fail (name != NULL, FALSE);

  g_mutex_lock (&mutex);
  tmp = g_hash_table_lookup (plugins, name);
  g_mutex_unlock (&mutex);

  if (tmp == NULL) {
    g_warning ("type '%s' has not been registered", name);
    return NULL;
  }

  object_type = GPOINTER_TO_UINT (tmp);

  plugin = g_object_new (object_type, NULL);

  return plugin;
}

/**
 * shp_plugin_factory_register:
 * @object_type: a #GType
 *
 * Registers new #ShpPlugin type, type is represented by @object_type.
 *
 * Returns: %TRUE on success and %FALSE otherwise
 */
gboolean
shp_plugin_factory_register (gchar * name, GType object_type)
{
  g_return_val_if_fail (name != NULL, FALSE);

  if (!g_type_is_a (object_type, SHP_PLUGIN_TYPE)) {
    g_error ("the '%s' type is not a descendant of ShpPlugin.", name);
    return FALSE;
  }

  g_mutex_lock (&mutex);

  if (g_hash_table_lookup (plugins, name)) {
    g_error ("type '%s' has already been registered.", name);
    return FALSE;
  }

  g_hash_table_insert (plugins, g_strdup (name),
      GUINT_TO_POINTER (object_type));

  g_mutex_unlock (&mutex);

  return TRUE;
}

/**
 * shp_plugin_factory_get_plugin_list:
 *
 * Returns a %NULL terminated string array with all available plugins.
 *
 * Returns: A newly allocated string array with all available plugins, free whit
 * g_strfreev() when nolonger needed.
 */
gchar**
shp_plugin_factory_get_plugin_list ()
{
  GHashTableIter iter;
  gpointer key, value;
  guint num_plugins;
  gchar **result;
  guint index;

  g_return_if_fail (plugins != NULL);

  g_mutex_lock (&mutex);

  num_plugins = g_hash_table_size (plugins);
  if (num_plugins == 0) {
    g_mutex_unlock (&mutex);
    return NULL;
  }

  result = (gchar **)g_malloc0 ((num_plugins + 1) * sizeof (gchar*));

  index = 0;

  g_hash_table_iter_init (&iter, plugins);
  while (g_hash_table_iter_next (&iter, &key, &value)) {
    result[index++] = g_strdup ((gchar *)key);
  }

  g_mutex_unlock (&mutex);

  result[index] = NULL;
  g_assert (index == num_plugins);

  return result;
}

/**
 * shp_plugin_factory_setup:
 *
 * Sets up the plugin factory.
 *
 * Returns: %TRUE on success and %FALSE otherwise
 */
gboolean
shp_plugin_factory_setup ()
{
  plugins = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);
  g_mutex_init (&mutex);
  return TRUE;
}

/**
 * shp_plugin_factory_cleanup:
 *
 * Cleans up the plugin factory and frees all resources previously allocated
 * with shp_plugin_factory_setup().
 */
void
shp_plugin_factory_cleanup ()
{
  g_hash_table_unref (plugins);
  plugins = NULL;
  g_mutex_clear (&mutex);
}
