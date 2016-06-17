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
 * ShpStructure is the basic unit of passing data from plugins.
 */

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

#include "shp-structure.h"

G_DEFINE_TYPE (ShpStructure, shp_structure, G_TYPE_OBJECT);

struct _ShpStructurePrivate {
  GHashTable *values;
  gchar *name;
  gchar *source_path;
};

static void shp_structure_finalize (GObject * object);
static void shp_structure_get_property (GObject * object, guint propid,
    GValue * value, GParamSpec * pspec);
static void shp_structure_set_property (GObject * object, guint propid,
    const GValue * value, GParamSpec * pspec);

typedef struct {
  GValue value;
} _ShpValue;

static void
shp_structure_class_init (ShpStructureClass * klass)
{
  GObjectClass *gobject_class;

  gobject_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (ShpStructurePrivate));

  gobject_class->finalize = shp_structure_finalize;
  gobject_class->set_property = shp_structure_set_property;
  gobject_class->get_property = shp_structure_get_property;
}

static void
shp_structure_get_property (GObject * object, guint propid, GValue * value,
    GParamSpec * pspec)
{
  ShpStructurePrivate *priv;
  ShpStructure *structure = SHP_STRUCTURE (object);

  priv = structure->priv;
  if (priv);

  switch (propid) {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, propid, pspec);
  }
}

static void
shp_structure_set_property (GObject * object, guint propid, const GValue * value,
    GParamSpec * pspec)
{
  ShpStructurePrivate *priv;
  ShpStructure *structure = SHP_STRUCTURE (object);

  priv = structure->priv;
  if (priv);

  switch (propid) {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, propid, pspec);
  }
}

static void
free_value_cb (gpointer data)
{
  _ShpValue *value = data;
  g_value_unset (&value->value);
  g_free (value);
}

static void
shp_structure_init (ShpStructure * self)
{
  ShpStructurePrivate *priv;

  self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self,
                                            SHP_STRUCTURE_TYPE,
                                            ShpStructurePrivate);

  priv = self->priv;

  priv->values = g_hash_table_new_full (g_str_hash, g_str_equal, g_free,
      free_value_cb);
}

static void
shp_structure_finalize (GObject * object)
{
  ShpStructurePrivate *priv;
  ShpStructure *self = SHP_STRUCTURE (object);

  priv = self->priv;

  g_hash_table_unref (priv->values);
  priv->values = NULL;
  g_free (priv->name);
  priv->name = NULL;
  g_free (priv->source_path);
  priv->source_path = NULL;
}

/**
 * shp_structure_new:
 *
 * Creates a new instance of #ShpStructure. Free with g_object_unref()
 * when no-longer needed.
 *
 * Returns: a new instance of #ShpStructure
 */
ShpStructure*
shp_structure_new ()
{
  return g_object_new (SHP_STRUCTURE_TYPE, NULL);
}

/**
 * shp_structure_add_string:
 * @msg: a #ShpStructure
 * @name: data's field name
 * @val: value
 *
 * Add a new key-value pair to a #ShpStructure, value is of type string
 */
void
shp_structure_add_string (ShpStructure * msg, const gchar * name,
    const gchar * val)
{
  ShpStructurePrivate *priv;

  g_return_if_fail (IS_SHP_STRUCTURE (msg));
  g_return_if_fail (name != NULL);

  priv = msg->priv;

  _ShpValue *value = g_new0 (_ShpValue, 1);
  g_value_init (&value->value, G_TYPE_STRING);
  g_value_set_string (&value->value, val);
  g_hash_table_insert (priv->values, g_strdup (name), value);
}

/**
 * shp_structure_add_integer:
 * @msg: a #ShpStructure
 * @name: data's field name
 * @val: value
 *
 * Add a new key-value pair to a #ShpStructure, value is of type integer
 */
void
shp_structure_add_integer (ShpStructure * msg, const gchar * name, gint val)
{
  ShpStructurePrivate *priv;

  g_return_if_fail (IS_SHP_STRUCTURE (msg));
  g_return_if_fail (name != NULL);

  priv = msg->priv;

  _ShpValue *value = g_new0 (_ShpValue, 1);
  g_value_init (&value->value, G_TYPE_INT);
  g_value_set_int (&value->value, val);
  g_hash_table_insert (priv->values, g_strdup (name), value);
}

/**
 * shp_structure_add_double:
 * @msg: a #ShpStructure
 * @name: data's field name
 * @val: value
 *
 * Add a new key-value pair to a #ShpStructure, value is of type double
 */
void
shp_structure_add_double (ShpStructure * msg, const gchar * name, gdouble val)
{
  ShpStructurePrivate *priv;

  g_return_if_fail (IS_SHP_STRUCTURE (msg));
  g_return_if_fail (name != NULL);

  priv = msg->priv;

  _ShpValue *value = g_new0 (_ShpValue, 1);
  g_value_init (&value->value, G_TYPE_DOUBLE);
  g_value_set_double (&value->value, val);
  g_hash_table_insert (priv->values, g_strdup (name), value);
}

/**
 * shp_structure_add_boolean:
 * @msg: a #ShpStructure
 * @name: data's field name
 * @val: value
 *
 * Add a new key-value pair to a #ShpStructure, value is of type boolean
 */
void
shp_structure_add_boolean (ShpStructure * msg, const gchar * name, gboolean val)
{
  ShpStructurePrivate *priv;

  g_return_if_fail (IS_SHP_STRUCTURE (msg));
  g_return_if_fail (name != NULL);

  priv = msg->priv;

  _ShpValue *value = g_new0 (_ShpValue, 1);
  g_value_init (&value->value, G_TYPE_BOOLEAN);
  g_value_set_boolean (&value->value, val);
  g_hash_table_insert (priv->values, g_strdup (name), value);
}

/**
 * shp_structure_add_long:
 * @msg: a #ShpStructure
 * @name: data's field name
 * @val: value
 *
 * Add a new key-value pair to a #ShpStructure, value is of type long
 */
void
shp_structure_add_long (ShpStructure * msg, const gchar * name, glong val)
{
  ShpStructurePrivate *priv;

  g_return_if_fail (IS_SHP_STRUCTURE (msg));
  g_return_if_fail (name != NULL);

  priv = msg->priv;

  _ShpValue *value = g_new0 (_ShpValue, 1);
  g_value_init (&value->value, G_TYPE_LONG);
  g_value_set_long (&value->value, val);
  g_hash_table_insert (priv->values, g_strdup (name), value);
}

/**
 * shp_structure_has value:
 * @msg: a #ShpStructure
 * @name: data's field name
 * @type: msg type
 *
 * Check whether there is a field with name @name and of type @type in the
 * data. This function is normally called before calling any of the
 * shp_structure_get_*() set of functions.
 *
 * Returns: TRUE if such a field exists and FALSE otherwise
 */
gboolean
shp_structure_has_value (ShpStructure * msg, const gchar * name, GType type)
{
  ShpStructurePrivate *priv;
  _ShpValue *value;

  g_return_val_if_fail (IS_SHP_STRUCTURE (msg), FALSE);
  g_return_val_if_fail (name != NULL, FALSE);
  g_return_val_if_fail (G_TYPE_IS_FUNDAMENTAL (type), FALSE);

  priv = msg->priv;

  value = g_hash_table_lookup (priv->values, name);
  if (value == NULL)
    return FALSE;

  if (G_VALUE_TYPE (value) == type)
    return TRUE;

  return FALSE;
}

/**
 * shp_structure_get_string:
 * @msg: a #ShpStructure
 * @name: data's field name
 *
 * Returns the value of type string corresponding to @name
 *
 * Returns: a value of type string
 */
const gchar*
shp_structure_get_string (ShpStructure * msg, const gchar * name)
{
  ShpStructurePrivate *priv;
  _ShpValue *value;
  const gchar *ret;

  g_return_val_if_fail (IS_SHP_STRUCTURE (msg), NULL);
  g_return_val_if_fail (name != NULL, NULL);

  priv = msg->priv;

  value = g_hash_table_lookup (priv->values, name);
  if (!value) {
    g_warning ("incomplete data, missing %s", name);
    return NULL;
  }
  if (G_VALUE_TYPE (&value->value) != G_TYPE_STRING) {
    g_warning ("invalid type of data, expected: %s, got: %s",
        g_type_name (G_TYPE_STRING), G_VALUE_TYPE_NAME (&value->value));
    return NULL;
  }
  ret = g_value_get_string (&value->value);
  return ret;
}

/**
 * shp_structure_get_integer:
 * @msg: a #ShpStructure
 * @name: data's field name
 *
 * Returns the value of type integer corresponding to @name
 *
 * Returns: a value of type integer
 */
gboolean
shp_structure_get_integer (ShpStructure * msg, const gchar * name, gint * result)
{
  ShpStructurePrivate *priv;
  _ShpValue *value;

  g_return_val_if_fail (IS_SHP_STRUCTURE (msg), FALSE);
  g_return_val_if_fail (name != NULL, FALSE);
  g_return_val_if_fail (result != NULL, FALSE);

  priv = msg->priv;

  value = g_hash_table_lookup (priv->values, name);
  if (!value) {
    g_warning ("incomplete data, missing %s", name);
    return FALSE;
  }
  if (G_VALUE_TYPE (&value->value) != G_TYPE_INT) {
    g_warning ("invalid type of data, expected: %s, got: %s",
        g_type_name (G_TYPE_INT), G_VALUE_TYPE_NAME (&value->value));
    return FALSE;
  }
  *result = g_value_get_int (&value->value);
  return TRUE;
}

/**
 * shp_structure_get_double:
 * @msg: a #ShpStructure
 * @name: data's field name
 *
 * Returns the value of type double corresponding to @name
 *
 * Returns: a value of type double
 */
gboolean
shp_structure_get_double (ShpStructure * msg, const gchar * name, gdouble * result)
{
  ShpStructurePrivate *priv;
  _ShpValue *value;

  g_return_val_if_fail (IS_SHP_STRUCTURE (msg), FALSE);
  g_return_val_if_fail (name != NULL, FALSE);
  g_return_val_if_fail (result != NULL, FALSE);

  priv = msg->priv;

  value = g_hash_table_lookup (priv->values, name);
  if (!value) {
    g_warning ("incomplete data, missing %s", name);
    return FALSE;
  }
  if (G_VALUE_TYPE (&value->value) != G_TYPE_DOUBLE) {
    g_warning ("invalid type of data, expected: %s, got: %s",
        g_type_name (G_TYPE_DOUBLE), G_VALUE_TYPE_NAME (&value->value));
    return FALSE;
  }
  *result = g_value_get_double (&value->value);
  return TRUE;
}

/**
 * shp_structure_get_boolean:
 * @msg: a #ShpStructure
 * @name: data's field name
 *
 * Returns the value of type boolean corresponding to @name
 *
 * Returns: a value of type boolean
 */
gboolean
shp_structure_get_boolean (ShpStructure * msg, const gchar * name,
    gboolean * result)
{
  ShpStructurePrivate *priv;
  _ShpValue *value;

  g_return_val_if_fail (IS_SHP_STRUCTURE (msg), FALSE);
  g_return_val_if_fail (name != NULL, FALSE);
  g_return_val_if_fail (result != NULL, FALSE);

  priv = msg->priv;

  value = g_hash_table_lookup (priv->values, name);
  if (!value) {
    g_warning ("incomplete data, missing %s", name);
    return FALSE;
  }
  if (G_VALUE_TYPE (&value->value) != G_TYPE_BOOLEAN) {
    g_warning ("invalid type of data, expected: %s, got: %s",
        g_type_name (G_TYPE_BOOLEAN), G_VALUE_TYPE_NAME (&value->value));
    return FALSE;
  }
  *result = g_value_get_boolean (&value->value);
  return TRUE;
}

/**
 * shp_structure_get_long:
 * @msg: a #ShpStructure
 * @name: data's field name
 *
 * Returns the value of type long corresponding to @name
 *
 * Returns: a value of type long
 */
gboolean
shp_structure_get_long (ShpStructure * msg, const gchar * name, glong * result)
{
  ShpStructurePrivate *priv;
  _ShpValue *value;

  g_return_val_if_fail (IS_SHP_STRUCTURE (msg), FALSE);
  g_return_val_if_fail (name != NULL, FALSE);
  g_return_val_if_fail (result != NULL, FALSE);

  priv = msg->priv;

  value = g_hash_table_lookup (priv->values, name);
  if (!value) {
    g_warning ("incomplete data, missing %s", name);
    return FALSE;
  }
  if (G_VALUE_TYPE (&value->value) != G_TYPE_LONG) {
    g_warning ("invalid type of data, expected: %s, got: %s",
        g_type_name (G_TYPE_LONG), G_VALUE_TYPE_NAME (&value->value));
    return FALSE;
  }
  *result = g_value_get_long (&value->value);
  return TRUE;
}

typedef struct {
  ShpStructureFunc func;
  gpointer user_data;
} _ShpFuncData;

static void
foreach_cb (gpointer key, gpointer val, gpointer data)
{
  _ShpFuncData *func_data = data;

  ShpStructureFunc func = func_data->func;
  const gchar *name = key;
  const _ShpValue  *value = val;

  func (name, &value->value, func_data->user_data);
}

/**
 * shp_structure_size:
 * @msg: a #ShpStructure
 *
 * Returns number of parameters in @msg
 *
 * Returns: number of parameters in @msg
 */
guint
shp_structure_size (ShpStructure * msg)
{
  ShpStructurePrivate *priv;

  g_return_if_fail (IS_SHP_STRUCTURE (msg));

  priv = msg->priv;

  return g_hash_table_size (priv->values);
}

/**
 * shp_structure_foreach:
 * @msg: a #ShpStructure
 * @func: a #ShpStructureFunc to be called
 * @user_data: user data to be submitted when calling @func
 *
 * Calls @func for each parameter in @msg
 */
void
shp_structure_foreach (ShpStructure * msg, ShpStructureFunc func,
    gpointer user_data)
{
  ShpStructurePrivate *priv;
  _ShpFuncData *func_data;

  g_return_if_fail (IS_SHP_STRUCTURE (msg));
  g_return_if_fail (func != NULL);

  priv = msg->priv;

  func_data = g_new0 (_ShpFuncData, 1);
  func_data->func = func;
  func_data->user_data = user_data;

  g_hash_table_foreach (priv->values, foreach_cb, func_data);

  g_free (func_data);
}

GType
shp_structure_get_field_type (ShpStructure * msg, const gchar * name)
{
  ShpStructurePrivate *priv;
  _ShpValue *value;

  g_return_val_if_fail (IS_SHP_STRUCTURE (msg), G_TYPE_INVALID);
  g_return_val_if_fail (name != NULL, G_TYPE_INVALID);

  priv = msg->priv;

  value = g_hash_table_lookup (priv->values, name);
  if (!value) {
    g_warning ("incomplete data, missing %s", name);
    return G_TYPE_INVALID;
  }
  return G_VALUE_TYPE (&value);
}
