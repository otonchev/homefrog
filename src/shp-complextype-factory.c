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
#include <gmodule.h>

#include "shp-complextype-factory.h"

static GHashTable *complextypes;
static GMutex mutex;

/**
 * shp_complextype_factory_create:
 * @name: name of the #ShpComplextype
 *
 * Creates new #ShpComplextype of type @name.
 *
 * Returns: A new #ShpComplextype instance of type @name. Free with
 * g_object_unref() when no longer needed.
 */
ShpComplextype*
shp_complextype_factory_create (gchar * name, gchar * path)
{
  ShpComplextype *complextype = NULL;
  gpointer tmp;
  GType object_type;

  g_return_val_if_fail (name != NULL, FALSE);

  g_mutex_lock (&mutex);
  tmp = g_hash_table_lookup (complextypes, name);
  g_mutex_unlock (&mutex);

  if (tmp == NULL) {
    g_warning ("type '%s' has not been registered", name);
    return NULL;
  }

  object_type = GPOINTER_TO_UINT (tmp);

  complextype = g_object_new (object_type, "name", name, "path", path, NULL);

  return complextype;
}

/**
 * shp_complextype_factory_register:
 * @object_type: a #GType
 *
 * Registers new #ShpComplextype type, type is represented by @object_type.
 *
 * Returns: %TRUE on success and %FALSE otherwise
 */
gboolean
shp_complextype_factory_register (gchar * name, GType object_type)
{
  g_return_val_if_fail (name != NULL, FALSE);

  if (!g_type_is_a (object_type, SHP_COMPLEXTYPE_TYPE)) {
    g_error ("the '%s' type is not a descendant of ShpComplextype.", name);
    return FALSE;
  }

  g_mutex_lock (&mutex);

  if (g_hash_table_lookup (complextypes, name)) {
    g_error ("type '%s' has already been registered.", name);
    return FALSE;
  }

  g_hash_table_insert (complextypes, g_strdup (name),
      GUINT_TO_POINTER (object_type));

  g_mutex_unlock (&mutex);

  return TRUE;
}

/**
 * shp_complextype_factory_get_complextype_list:
 *
 * Returns a %NULL terminated string array with all available complextypes.
 *
 * Returns: A newly allocated string array with all available complextypes, free whit
 * g_strfreev() when nolonger needed.
 */
gchar**
shp_complextype_factory_get_complextype_list ()
{
  GHashTableIter iter;
  gpointer key, value;
  guint num_complextypes;
  gchar **result;
  guint index;

  g_return_if_fail (complextypes != NULL);

  g_mutex_lock (&mutex);

  num_complextypes = g_hash_table_size (complextypes);
  if (num_complextypes == 0) {
    g_mutex_unlock (&mutex);
    return NULL;
  }

  result = (gchar **)g_malloc0 ((num_complextypes + 1) * sizeof (gchar*));

  index = 0;

  g_hash_table_iter_init (&iter, complextypes);
  while (g_hash_table_iter_next (&iter, &key, &value)) {
    result[index++] = g_strdup ((gchar *)key);
  }

  g_mutex_unlock (&mutex);

  result[index] = NULL;
  g_assert (index == num_complextypes);

  return result;
}

typedef void (* ComplextypeRegisterFunc) ();

/* load single complextype */
static gboolean
load_complextype (gchar * path)
{
  ComplextypeRegisterFunc complextype_register;
  GModule *module;

  g_debug ("loading module: %s", path);

  module = g_module_open (path, G_MODULE_BIND_LAZY);
  if (!module) {
    g_critical ("unable to load module %s", path);
    return FALSE;
  }
  if (!g_module_symbol (module, "shp_complextype_register",
      (gpointer *)&complextype_register)) {
    g_critical ("unable to find shp_register_complextype symbol in %s", path);
    return FALSE;
  }
  if (!complextype_register) {
    g_critical ("unable to find shp_register_complextype symbol in %s", path);
    return FALSE;
  }

  complextype_register ();
  return TRUE;
}

/* load all available complextypes from complextype_dir */
static gboolean
load_complextypes (const gchar * complextype_dir)
{
  GDir *dir;
  GError *error;
  const gchar *filename;
  guint num_complextypes = 0;

  dir = g_dir_open (complextype_dir, 0, &error);
  if (dir == NULL) {
    g_critical ("failed to open complextype directory: %s, reason: %s", complextype_dir,
        error->message);
    g_clear_error (&error);
    return FALSE;
  }

  while ((filename = g_dir_read_name (dir))) {
    gchar *path = g_strdup_printf ("%s%s", complextype_dir, filename);

    if (!g_str_has_suffix (path, ".so")) {
      g_free (path);
      continue;
    }

    if (!load_complextype (path)) {
      g_critical ("could not load complextype: %s", path);
      g_dir_close (dir);
      return FALSE;
    } else
      num_complextypes++;

    g_free (path);
  }

  if (num_complextypes == 0) {
    g_critical ("could not load any complextypes");
    g_dir_close (dir);
    return FALSE;
  }

  g_debug ("number of complextypes loaded: %d", num_complextypes);
  g_dir_close (dir);

  return TRUE;
}

/**
 * shp_complextype_factory_setup:
 * @complextype_dir: directory where complextype files are located
 *
 * Sets up the complextype factory.
 *
 * Returns: %TRUE on success and %FALSE otherwise
 */
gboolean
shp_complextype_factory_setup (const gchar * complextype_dir)
{
  complextypes = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);
  g_mutex_init (&mutex);

  if (!load_complextypes (complextype_dir)) {
    g_warning ("unable to load complextypes");
    g_hash_table_unref (complextypes);
    complextypes = NULL;
    g_mutex_clear (&mutex);
    return FALSE;
  }

  return TRUE;
}

/**
 * shp_complextype_factory_cleanup:
 *
 * Cleans up the complextype factory and frees all resources previously allocated
 * with shp_complextype_factory_setup().
 */
void
shp_complextype_factory_cleanup ()
{
  /* FIXME: unload complextypes */
  g_hash_table_unref (complextypes);
  complextypes = NULL;
  g_mutex_clear (&mutex);
}
