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
 * ShpMessage is the basic unit of passing data from plugins.
 */

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

#include "shp-message.h"

enum
{
  PROP_0,
  PROP_NAME,
  PROP_LAST
};

#define DEFAULT_NAME "no_name"

G_DEFINE_TYPE (ShpMessage, shp_message, G_TYPE_OBJECT);

static void shp_message_finalize (GObject * object);
static void shp_message_get_property (GObject * object, guint propid,
    GValue * value, GParamSpec * pspec);
static void shp_message_set_property (GObject * object, guint propid,
    const GValue * value, GParamSpec * pspec);

typedef struct {
  GValue value;
} _ShpValue;

static void
shp_message_class_init (ShpMessageClass * klass)
{
  GObjectClass *gobject_class;

  gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->finalize = shp_message_finalize;
  gobject_class->set_property = shp_message_set_property;
  gobject_class->get_property = shp_message_get_property;

  g_object_class_install_property (gobject_class, PROP_NAME,
      g_param_spec_string ("name", "message name", "The name of the message",
          DEFAULT_NAME, G_PARAM_READWRITE));
}

static void
shp_message_get_property (GObject * object, guint propid, GValue * value,
    GParamSpec * pspec)
{
  ShpMessage *message = SHP_MESSAGE (object);

  switch (propid) {
    case PROP_NAME:
      g_value_set_string (value, message->name);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, propid, pspec);
  }
}

static void
shp_message_set_property (GObject * object, guint propid, const GValue * value,
    GParamSpec * pspec)
{
  ShpMessage *message = SHP_MESSAGE (object);

  switch (propid) {
    case PROP_NAME:
      g_free (message->name);
      message->name = g_strdup (g_value_get_string (value));
      break;
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
shp_message_init (ShpMessage * self)
{
  self->values = g_hash_table_new_full (g_str_hash, g_str_equal, g_free,
      free_value_cb);
  self->name = g_strdup (DEFAULT_NAME);
}

static void
shp_message_finalize (GObject * object)
{
  ShpMessage *self = SHP_MESSAGE (object);
  g_hash_table_unref (self->values);
  g_free (self->name);
}

/**
 * shp_message_new:
 *
 * Creates a new instance of #ShpMessage. Free with g_object_unref()
 * when no-longer needed.
 *
 * Returns: a new instance of #ShpMessage
 */
ShpMessage*
shp_message_new (const gchar * name)
{
  return g_object_new (SHP_MESSAGE_TYPE, "name", name, NULL);
}

/**
 * shp_message_add_string:
 * @msg: a #ShpMessage
 * @name: data's field name
 * @val: value
 *
 * Add a new key-value pair to a #ShpMessage, value is of type string
 */
void
shp_message_add_string (ShpMessage * msg, const gchar * name,
    const gchar * val)
{
  g_return_if_fail (IS_SHP_MESSAGE (msg));
  g_return_if_fail (name != NULL);

  _ShpValue *value = g_new0 (_ShpValue, 1);
  g_value_init (&value->value, G_TYPE_STRING);
  g_value_set_string (&value->value, val);
  g_hash_table_insert (msg->values, g_strdup (name), value);
}

/**
 * shp_message_add_integer:
 * @msg: a #ShpMessage
 * @name: data's field name
 * @val: value
 *
 * Add a new key-value pair to a #ShpMessage, value is of type integer
 */
void
shp_message_add_integer (ShpMessage * msg, const gchar * name, gint val)
{
  g_return_if_fail (IS_SHP_MESSAGE (msg));
  g_return_if_fail (name != NULL);

  _ShpValue *value = g_new0 (_ShpValue, 1);
  g_value_init (&value->value, G_TYPE_INT);
  g_value_set_int (&value->value, val);
  g_hash_table_insert (msg->values, g_strdup (name), value);
}

/**
 * shp_message_add_double:
 * @msg: a #ShpMessage
 * @name: data's field name
 * @val: value
 *
 * Add a new key-value pair to a #ShpMessage, value is of type double
 */
void
shp_message_add_double (ShpMessage * msg, const gchar * name, gdouble val)
{
  g_return_if_fail (IS_SHP_MESSAGE (msg));
  g_return_if_fail (name != NULL);

  _ShpValue *value = g_new0 (_ShpValue, 1);
  g_value_init (&value->value, G_TYPE_DOUBLE);
  g_value_set_double (&value->value, val);
  g_hash_table_insert (msg->values, g_strdup (name), value);
}

/**
 * shp_message_add_boolean:
 * @msg: a #ShpMessage
 * @name: data's field name
 * @val: value
 *
 * Add a new key-value pair to a #ShpMessage, value is of type boolean
 */
void
shp_message_add_boolean (ShpMessage * msg, const gchar * name, gboolean val)
{
  g_return_if_fail (IS_SHP_MESSAGE (msg));
  g_return_if_fail (name != NULL);

  _ShpValue *value = g_new0 (_ShpValue, 1);
  g_value_init (&value->value, G_TYPE_BOOLEAN);
  g_value_set_boolean (&value->value, val);
  g_hash_table_insert (msg->values, g_strdup (name), value);
}

/**
 * shp_message_add_long:
 * @msg: a #ShpMessage
 * @name: data's field name
 * @val: value
 *
 * Add a new key-value pair to a #ShpMessage, value is of type long
 */
void
shp_message_add_long (ShpMessage * msg, const gchar * name, glong val)
{
  g_return_if_fail (IS_SHP_MESSAGE (msg));
  g_return_if_fail (name != NULL);

  _ShpValue *value = g_new0 (_ShpValue, 1);
  g_value_init (&value->value, G_TYPE_LONG);
  g_value_set_long (&value->value, val);
  g_hash_table_insert (msg->values, g_strdup (name), value);
}

/**
 * shp_message_has value:
 * @msg: a #ShpMessage
 * @name: data's field name
 * @type: msg type
 *
 * Check whether there is a field with name @name and of type @type in the
 * data. This function is normally called before calling any of the
 * shp_message_get_*() set of functions.
 *
 * Returns: TRUE if such a field exists and FALSE otherwise
 */
gboolean
shp_message_has_value (ShpMessage * msg, const gchar * name, GType type)
{
  _ShpValue *value;

  g_return_val_if_fail (IS_SHP_MESSAGE (msg), FALSE);
  g_return_val_if_fail (name != NULL, FALSE);
  g_return_val_if_fail (G_TYPE_IS_FUNDAMENTAL (type), FALSE);

  value = g_hash_table_lookup (msg->values, name);
  if (value == NULL)
    return FALSE;

  if (G_VALUE_TYPE (value) == type)
    return TRUE;

  return FALSE;
}

/**
 * shp_message_get_string:
 * @msg: a #ShpMessage
 * @name: data's field name
 *
 * Returns the value of type string corresponding to @name
 *
 * Returns: a value of type string
 */
const gchar*
shp_message_get_string (ShpMessage * msg, const gchar * name)
{
  _ShpValue *value;
  const gchar *ret;

  g_return_val_if_fail (IS_SHP_MESSAGE (msg), NULL);
  g_return_val_if_fail (name != NULL, NULL);

  value = g_hash_table_lookup (msg->values, name);
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
 * shp_message_get_integer:
 * @msg: a #ShpMessage
 * @name: data's field name
 *
 * Returns the value of type integer corresponding to @name
 *
 * Returns: a value of type integer
 */
gboolean
shp_message_get_integer (ShpMessage * msg, const gchar * name, gint * result)
{
  _ShpValue *value;

  g_return_val_if_fail (IS_SHP_MESSAGE (msg), FALSE);
  g_return_val_if_fail (name != NULL, FALSE);
  g_return_val_if_fail (result != NULL, FALSE);

  value = g_hash_table_lookup (msg->values, name);
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
 * shp_message_get_double:
 * @msg: a #ShpMessage
 * @name: data's field name
 *
 * Returns the value of type double corresponding to @name
 *
 * Returns: a value of type double
 */
gboolean
shp_message_get_double (ShpMessage * msg, const gchar * name, gdouble * result)
{
  _ShpValue *value;

  g_return_val_if_fail (IS_SHP_MESSAGE (msg), FALSE);
  g_return_val_if_fail (name != NULL, FALSE);
  g_return_val_if_fail (result != NULL, FALSE);

  value = g_hash_table_lookup (msg->values, name);
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
 * shp_message_get_boolean:
 * @msg: a #ShpMessage
 * @name: data's field name
 *
 * Returns the value of type boolean corresponding to @name
 *
 * Returns: a value of type boolean
 */
gboolean
shp_message_get_boolean (ShpMessage * msg, const gchar * name,
    gboolean * result)
{
  _ShpValue *value;

  g_return_val_if_fail (IS_SHP_MESSAGE (msg), FALSE);
  g_return_val_if_fail (name != NULL, FALSE);
  g_return_val_if_fail (result != NULL, FALSE);

  value = g_hash_table_lookup (msg->values, name);
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
 * shp_message_get_long:
 * @msg: a #ShpMessage
 * @name: data's field name
 *
 * Returns the value of type long corresponding to @name
 *
 * Returns: a value of type long
 */
gboolean
shp_message_get_long (ShpMessage * msg, const gchar * name, glong * result)
{
  _ShpValue *value;

  g_return_val_if_fail (IS_SHP_MESSAGE (msg), FALSE);
  g_return_val_if_fail (name != NULL, FALSE);
  g_return_val_if_fail (result != NULL, FALSE);

  value = g_hash_table_lookup (msg->values, name);
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
  ShpMessageFunc func;
  gpointer user_data;
} _ShpFuncData;

static void
foreach_cb (gpointer key, gpointer val, gpointer data)
{
  _ShpFuncData *func_data = data;

  ShpMessageFunc func = func_data->func;
  const gchar *name = key;
  const _ShpValue  *value = val;

  func (name, &value->value, func_data->user_data);
}

/**
 * shp_message_size:
 * @msg: a #ShpMessage
 *
 * Returns number of parameters in @msg
 *
 * Returns: number of parameters in @msg
 */
guint
shp_message_size (ShpMessage * msg)
{
  g_return_if_fail (IS_SHP_MESSAGE (msg));

  return g_hash_table_size (msg->values);
}

/**
 * shp_message_foreach:
 * @msg: a #ShpMessage
 * @func: a #ShpMessageFunc to be called
 * @user_data: user data to be submitted when calling @func
 *
 * Calls @func for each parameter in @msg
 */
void
shp_message_foreach (ShpMessage * msg, ShpMessageFunc func,
    gpointer user_data)
{
  _ShpFuncData *func_data;

  g_return_if_fail (IS_SHP_MESSAGE (msg));
  g_return_if_fail (func != NULL);

  func_data = g_new0 (_ShpFuncData, 1);
  func_data->func = func;
  func_data->user_data = user_data;

  g_hash_table_foreach (msg->values, foreach_cb, func_data);

  g_free (func_data);
}

GType
shp_message_get_field_type (ShpMessage * msg, const gchar * name)
{
  _ShpValue *value;

  g_return_val_if_fail (IS_SHP_MESSAGE (msg), G_TYPE_INVALID);
  g_return_val_if_fail (name != NULL, G_TYPE_INVALID);

  value = g_hash_table_lookup (msg->values, name);
  if (!value) {
    g_warning ("incomplete data, missing %s", name);
    return G_TYPE_INVALID;
  }
  return G_VALUE_TYPE (&value);
}
