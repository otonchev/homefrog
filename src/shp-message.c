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
 * ShpMessage is the basic unit of passing data from components installed on
 * the system. There are two main types of messages: Events and Configurations.
 * Events are messages emitted by plugins to notify other componenets that an
 * event has occured but also messages sent to plugins to trigger actions.
 * Configurations on the othger hand are messages emitted by componenets in
 * general to notify that certain configuration (a set of conditions and a
 * trigger) exists.
 *
 * Each message may have two paths set: source path and destination path.
 * source path indicates which component has emitted the message, destination
 * path which component the message is intended for.
 * Plugins will normally set the source path only leaving destination path
 * empty while components conrolling other components will do the other way
 * around.
 */

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <string.h>

#include "shp-message.h"

enum
{
  PROP_0,
  PROP_NAME,
  PROP_SOURCE_PATH,
  PROP_DESTINATION_PATH,
  PROP_TYPE,
  PROP_LAST
};

#define DEFAULT_NAME "no_name"
#define DEFAULT_SOURCE_PATH NULL
#define DEFAULT_DESTINATION_PATH NULL
#define MESSAGE_TYPE_DEFAULT SHP_MESSAGE_EVENT

#define SHP_MESSAGE_TYPE_TYPE (shp_message_type_get_type())

G_DEFINE_TYPE (ShpMessage, shp_message, G_TYPE_OBJECT);

struct _ShpMessagePrivate {
  GHashTable *values;
  gchar *name;
  gchar *source_path;
  gchar *destination_path;
  ShpMessageType type;
};

static void shp_message_finalize (GObject * object);
static void shp_message_get_property (GObject * object, guint propid,
    GValue * value, GParamSpec * pspec);
static void shp_message_set_property (GObject * object, guint propid,
    const GValue * value, GParamSpec * pspec);

typedef struct {
  GValue value;
} _ShpValue;

GType
shp_message_type_get_type (void)
{
  static GType shp_message_type_type = 0;

  if (g_once_init_enter (&shp_message_type_type)) {
    GType type;
    static const GEnumValue message_type[] = {
      {SHP_MESSAGE_EVENT,         "Event",         "Event"        },
      {SHP_MESSAGE_CONFIGURATION, "Configuration", "Configuration"},
      {0,                         NULL,            NULL           }
    };

    type = g_enum_register_static ("ShpMessageType", message_type);
    g_once_init_leave (&shp_message_type_type, type);
  }

  return shp_message_type_type;
}

static void
shp_message_class_init (ShpMessageClass * klass)
{
  GObjectClass *gobject_class;

  gobject_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (ShpMessagePrivate));

  gobject_class->finalize = shp_message_finalize;
  gobject_class->set_property = shp_message_set_property;
  gobject_class->get_property = shp_message_get_property;

  g_object_class_install_property (gobject_class, PROP_NAME,
      g_param_spec_string ("name", "message name", "The name of the message",
          DEFAULT_NAME, G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class, PROP_SOURCE_PATH,
      g_param_spec_string ("source-path", "Source path",
          "Path to the source of this message", DEFAULT_SOURCE_PATH,
          G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class, PROP_DESTINATION_PATH,
      g_param_spec_string ("destination-path", "Destination path",
          "Path to the destination of this message", DEFAULT_DESTINATION_PATH,
          G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class, PROP_TYPE,
      g_param_spec_enum ("type", "Message type",
          "The Message type", SHP_MESSAGE_TYPE_TYPE,
          MESSAGE_TYPE_DEFAULT, G_PARAM_READWRITE));
}

static void
shp_message_get_property (GObject * object, guint propid, GValue * value,
    GParamSpec * pspec)
{
  ShpMessagePrivate *priv;
  ShpMessage *message = SHP_MESSAGE (object);

  priv = message->priv;

  switch (propid) {
    case PROP_NAME:
      g_value_set_string (value, priv->name);
      break;
    case PROP_SOURCE_PATH:
      g_value_set_string (value, priv->source_path);
      break;
    case PROP_DESTINATION_PATH:
      g_value_set_string (value, priv->destination_path);
      break;
    case PROP_TYPE:
      g_value_set_enum (value, priv->type);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, propid, pspec);
  }
}

static void
shp_message_set_property (GObject * object, guint propid, const GValue * value,
    GParamSpec * pspec)
{
  ShpMessagePrivate *priv;
  ShpMessage *message = SHP_MESSAGE (object);

  priv = message->priv;

  switch (propid) {
    case PROP_NAME:
      g_free (priv->name);
      priv->name = g_strdup (g_value_get_string (value));
      break;
    case PROP_SOURCE_PATH:
      g_free (priv->source_path);
      priv->source_path = g_strdup (g_value_get_string (value));
      break;
    case PROP_DESTINATION_PATH:
      g_free (priv->destination_path);
      priv->destination_path = g_strdup (g_value_get_string (value));
      break;
    case PROP_TYPE:
      priv->type = g_value_get_enum (value);
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
  ShpMessagePrivate *priv;

  self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self,
                                            SHP_MESSAGE_TYPE,
                                            ShpMessagePrivate);

  priv = self->priv;

  priv->values = g_hash_table_new_full (g_str_hash, g_str_equal, g_free,
      free_value_cb);
  priv->name = g_strdup (DEFAULT_NAME);
  priv->source_path = g_strdup (DEFAULT_SOURCE_PATH);
  priv->destination_path = g_strdup (DEFAULT_DESTINATION_PATH);
  priv->type = MESSAGE_TYPE_DEFAULT;
}

static void
shp_message_finalize (GObject * object)
{
  ShpMessagePrivate *priv;
  ShpMessage *self = SHP_MESSAGE (object);

  priv = self->priv;

  g_hash_table_unref (priv->values);
  priv->values = NULL;
  g_free (priv->name);
  priv->name = NULL;
  g_free (priv->source_path);
  priv->source_path = NULL;
  g_free (priv->destination_path);
  priv->destination_path = NULL;
}

/**
 * shp_message_new:
 * @source_path: path for the messages (normally the path of the component
 *               creating the message)
 *
 * Creates a new instance of #ShpMessage. This function is normally used by
 * plugins to generate result. Free with g_object_unref() when no-longer needed.
 *
 * Returns: a new instance of #ShpMessage
 */
ShpMessage*
shp_message_new (const gchar * source_path)
{
  return g_object_new (SHP_MESSAGE_TYPE, "source-path", source_path, NULL);
}

/**
 * shp_message_new:
 * @destination_path: path for the messages (normally the path of the component
 *                    to receive and process the message)
 *
 * Creates a new instance of #ShpMessage inetnded to be used for controlling a
 * plugin. Free with g_object_unref()  when no-longer needed.
 *
 * Returns: a new instance of #ShpMessage
 */
ShpMessage*
shp_message_new_command (const gchar * destination_path)
{
  return g_object_new (SHP_MESSAGE_TYPE, "destination-path", destination_path,
      NULL);
}

/* command=(string)on&test=(string)blabla */

/**
 * shp_message_new:
 *
 * Creates a new instance of #ShpMessage. Free with g_object_unref()
 * when no-longer needed.
 *
 * Returns: a new instance of #ShpMessage
 */
ShpMessage*
shp_message_new_command_from_string (const gchar * destination_path,
    const gchar * options)
{
  ShpMessage *result;
  gchar **options_list;
  guint i;

  g_return_val_if_fail (destination_path != NULL, NULL);
  g_return_val_if_fail (options != NULL, NULL);

  g_debug ("message: creating message from string");

  result = shp_message_new_command (destination_path);

  options_list = g_strsplit (options, "&", 0);
  for (i = 0; options_list[i] != NULL; i++) {
    gchar *p;
    gchar *q;

    g_debug ("message: current option: %s", options_list[i]);

    if ((p = strchr (options_list[i], '=')) == NULL) {
      g_strfreev (options_list);
      goto error;
    }

    *p = '\0';
    p++;

    g_debug ("message: value and value type: %s", p);

    if (*p != '(') {
      g_strfreev (options_list);
      goto error;
    }

    if ((q = strchr (p, ')')) == NULL) {
      g_debug ("message: invalid format for type and value");
      g_strfreev (options_list);
      goto error;
    }

    p++;

    *q = '\0';
    q++;

    g_debug ("message: name: %s, value: %s, type: %s", options_list[i], p, q);

    if (!g_strcmp0 (p, "string")) {
      shp_message_add_string (result, options_list[i], q);
    } else if (!g_strcmp0 (p, "int")) {
      gint int_val;
      int_val = atoi (q);
      shp_message_add_integer (result, options_list[i], int_val);
    } else if (!g_strcmp0 (p, "double")) {
      gint double_val;
      double_val = atof (q);
      shp_message_add_double (result, options_list[i], double_val);
    } else {
      goto error;
    }
  }

  g_strfreev (options_list);

  return result;

error:
  g_object_unref (result);
  return NULL;
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
  ShpMessagePrivate *priv;

  g_return_if_fail (IS_SHP_MESSAGE (msg));
  g_return_if_fail (name != NULL);

  priv = msg->priv;

  _ShpValue *value = g_new0 (_ShpValue, 1);
  g_value_init (&value->value, G_TYPE_STRING);
  g_value_set_string (&value->value, val);
  g_hash_table_insert (priv->values, g_strdup (name), value);
}

/**
 * shp_message_add_string:
 * @msg: a #ShpMessage
 * @name: data's field name
 * @val: value
 *
 * Add a new key-value pair to a #ShpMessage, value is of type #ShpComplexType.
 */
void
shp_message_add_complextype (ShpMessage * msg, const gchar * name,
    ShpComplextype * val)
{
  ShpMessagePrivate *priv;

  g_return_if_fail (IS_SHP_MESSAGE (msg));
  g_return_if_fail (name != NULL);
  g_return_if_fail (IS_SHP_COMPLEXTYPE (val));

  priv = msg->priv;

  _ShpValue *value = g_new0 (_ShpValue, 1);
  g_value_init (&value->value, SHP_COMPLEXTYPE_TYPE);
  g_value_set_object (&value->value, val);
  g_hash_table_insert (priv->values, g_strdup (name), value);
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
  ShpMessagePrivate *priv;

  g_return_if_fail (IS_SHP_MESSAGE (msg));
  g_return_if_fail (name != NULL);

  priv = msg->priv;

  _ShpValue *value = g_new0 (_ShpValue, 1);
  g_value_init (&value->value, G_TYPE_INT);
  g_value_set_int (&value->value, val);
  g_hash_table_insert (priv->values, g_strdup (name), value);
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
  ShpMessagePrivate *priv;

  g_return_if_fail (IS_SHP_MESSAGE (msg));
  g_return_if_fail (name != NULL);

  priv = msg->priv;

  _ShpValue *value = g_new0 (_ShpValue, 1);
  g_value_init (&value->value, G_TYPE_DOUBLE);
  g_value_set_double (&value->value, val);
  g_hash_table_insert (priv->values, g_strdup (name), value);
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
  ShpMessagePrivate *priv;

  g_return_if_fail (IS_SHP_MESSAGE (msg));
  g_return_if_fail (name != NULL);

  priv = msg->priv;

  _ShpValue *value = g_new0 (_ShpValue, 1);
  g_value_init (&value->value, G_TYPE_BOOLEAN);
  g_value_set_boolean (&value->value, val);
  g_hash_table_insert (priv->values, g_strdup (name), value);
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
  ShpMessagePrivate *priv;

  g_return_if_fail (IS_SHP_MESSAGE (msg));
  g_return_if_fail (name != NULL);

  priv = msg->priv;

  _ShpValue *value = g_new0 (_ShpValue, 1);
  g_value_init (&value->value, G_TYPE_LONG);
  g_value_set_long (&value->value, val);
  g_hash_table_insert (priv->values, g_strdup (name), value);
}

/**
 * shp_message_has value:
 * @msg: a #ShpMessage
 * @name: data's field name
 * @type: msg type
 *
 * Check whether there is a field with name @name and of type @type in the
 * message. This function is normally called before calling any of the
 * shp_message_get_*() set of functions.
 *
 * Returns: TRUE if such a field exists and FALSE otherwise
 */
gboolean
shp_message_has_value (ShpMessage * msg, const gchar * name, GType type)
{
  ShpMessagePrivate *priv;
  _ShpValue *value;

  g_return_val_if_fail (IS_SHP_MESSAGE (msg), FALSE);
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
  ShpMessagePrivate *priv;
  _ShpValue *value;
  const gchar *ret;

  g_return_val_if_fail (IS_SHP_MESSAGE (msg), NULL);
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

const ShpComplextype*
shp_message_get_complextype (ShpMessage * msg, const gchar * name)
{
  ShpMessagePrivate *priv;
  _ShpValue *value;
  const ShpComplextype *ret;

  g_return_val_if_fail (IS_SHP_MESSAGE (msg), NULL);
  g_return_val_if_fail (name != NULL, NULL);

  priv = msg->priv;

  value = g_hash_table_lookup (priv->values, name);
  if (!value) {
    g_warning ("incomplete data, missing %s", name);
    return NULL;
  }
  if (G_VALUE_TYPE (&value->value) != SHP_COMPLEXTYPE_TYPE) {
    g_warning ("invalid type of data, expected: %s, got: %s",
        g_type_name (SHP_COMPLEXTYPE_TYPE), G_VALUE_TYPE_NAME (&value->value));
    return NULL;
  }
  ret = g_value_get_object (&value->value);
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
  ShpMessagePrivate *priv;
  _ShpValue *value;

  g_return_val_if_fail (IS_SHP_MESSAGE (msg), FALSE);
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
  ShpMessagePrivate *priv;
  _ShpValue *value;

  g_return_val_if_fail (IS_SHP_MESSAGE (msg), FALSE);
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
  ShpMessagePrivate *priv;
  _ShpValue *value;

  g_return_val_if_fail (IS_SHP_MESSAGE (msg), FALSE);
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
  ShpMessagePrivate *priv;
  _ShpValue *value;

  g_return_val_if_fail (IS_SHP_MESSAGE (msg), FALSE);
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
  ShpMessagePrivate *priv;

  g_return_if_fail (IS_SHP_MESSAGE (msg));

  priv = msg->priv;

  return g_hash_table_size (priv->values);
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
  ShpMessagePrivate *priv;
  _ShpFuncData *func_data;

  g_return_if_fail (IS_SHP_MESSAGE (msg));
  g_return_if_fail (func != NULL);

  priv = msg->priv;

  func_data = g_new0 (_ShpFuncData, 1);
  func_data->func = func;
  func_data->user_data = user_data;

  g_hash_table_foreach (priv->values, foreach_cb, func_data);

  g_free (func_data);
}

/**
 * shp_message_get_field_type:
 * @name: field name
 *
 * Gets the type of the field with @name
 *
 * Returns: #GType
 */
GType
shp_message_get_field_type (ShpMessage * msg, const gchar * name)
{
  ShpMessagePrivate *priv;
  _ShpValue *value;

  g_return_val_if_fail (IS_SHP_MESSAGE (msg), G_TYPE_INVALID);
  g_return_val_if_fail (name != NULL, G_TYPE_INVALID);

  priv = msg->priv;

  value = g_hash_table_lookup (priv->values, name);
  if (!value) {
    g_warning ("incomplete data, missing %s", name);
    return G_TYPE_INVALID;
  }
  return G_VALUE_TYPE (&value);
}

/**
 * shp_message_get_name:
 * @msg: a #ShpMessage
 *
 * Gets the name of the message
 *
 * Returns: the name for @msg (transfer-none)
 */
const char*
shp_message_get_name (ShpMessage * msg)
{
  ShpMessagePrivate *priv;
  g_return_val_if_fail (IS_SHP_MESSAGE (msg), NULL);

  priv = msg->priv;

  return priv->name;
}

/**
 * shp_message_get_source_path:
 * @msg: a #ShpMessage
 *
 * Gets source path
 *
 * Returns: source path (transfer-none)
 */
const char*
shp_message_get_source_path (const ShpMessage * msg)
{
  ShpMessagePrivate *priv;
  g_return_val_if_fail (IS_SHP_MESSAGE (msg), NULL);

  priv = msg->priv;

  return priv->source_path;
}

/**
 * shp_message_get_destination_path:
 * @msg: a #ShpMessage
 *
 * Gets destination path
 *
 * Returns: destination path (transfer-none)
 */
const char*
shp_message_get_destination_path (const ShpMessage * msg)
{
  ShpMessagePrivate *priv;
  g_return_val_if_fail (IS_SHP_MESSAGE (msg), NULL);

  priv = msg->priv;

  return priv->destination_path;
}

static void
print_pair (gpointer key, gpointer val, gpointer user_data)
{
  const gchar *name = key;
  const _ShpValue  *value = val;
  GString *str = user_data;

  switch (G_VALUE_TYPE (&value->value)) {
    case G_TYPE_STRING:
      g_string_append_printf (str, " '%s':'(string)%s'", name,
          g_value_get_string (&value->value));
      break;
    case G_TYPE_INT:
      g_string_append_printf (str, " '%s':'(int)%d'", name,
          g_value_get_int (&value->value));
      break;
    case G_TYPE_DOUBLE:
      g_string_append_printf (str, " '%s':'(double)%f'", name,
          g_value_get_double (&value->value));
      break;
    case G_TYPE_LONG:
      g_string_append_printf (str, " '%s':'(double)%ld'", name,
          g_value_get_long (&value->value));
      break;
    default:
      break;
  }
}

/**
 * shp_message_to_string:
 * @msg: a #ShpMessage
 *
 * Creates a string representing @msg, can be pretty printed
 *
 * Returns: newly allocated string, free with g_free()
 */
gchar*
shp_message_to_string (ShpMessage * msg)
{
  GString *str;
  ShpMessagePrivate *priv;
  g_return_val_if_fail (IS_SHP_MESSAGE (msg), NULL);

  priv = msg->priv;

  str = g_string_new (priv->source_path);

  g_hash_table_foreach (priv->values, print_pair, str);
  return g_string_free (str, FALSE);
}

/**
 * shp_message_copy:
 * @msg: a #ShpMessage
 *
 * Creates a copy of @message
 *
 * Returns: new #ShpMessage, free with g_object_unref when no-longer needed
 */
ShpMessage*
shp_message_copy (const ShpMessage * message)
{
  ShpMessage *result;
  ShpMessagePrivate *opriv;
  ShpMessagePrivate *priv;
  GHashTableIter iter;
  gpointer key, value;

  g_return_val_if_fail (IS_SHP_MESSAGE (message), NULL);

  opriv = message->priv;

  result = g_object_new (SHP_MESSAGE_TYPE, "source-path",
      shp_message_get_source_path (message), "destination-path",
      shp_message_get_destination_path (message), NULL);

  priv = result->priv;

  g_hash_table_iter_init (&iter, opriv->values);
  while (g_hash_table_iter_next (&iter, &key, &value)) {
    const _ShpValue *src_value = value;
    _ShpValue *dest_value = g_new0 (_ShpValue, 1);

    g_value_init (&dest_value->value, G_VALUE_TYPE (&src_value->value));
    g_value_copy (&src_value->value, &dest_value->value);

    g_hash_table_insert (priv->values, g_strdup ((gchar *)key), dest_value);
  }

  return result;
}

/**
 * shp_message_get_message_type:
 * @msg: a #ShpMessage
 *
 * Gets the type of message
 *
 * Returns: #ShpMessageType
 */
ShpMessageType
shp_message_get_message_type (ShpMessage * msg)
{
  ShpMessageType type;

  g_return_val_if_fail (IS_SHP_MESSAGE (msg), SHP_MESSAGE_UNKNOWN);

  g_object_get (G_OBJECT (msg), "type", &type, NULL);
  return type;
}

/**
 * shp_message_set_message_type:
 * @msg: a #ShpMessage
 * @type: #ShpMessageType
 *
 * Sets the type
 */
void
shp_message_set_message_type (ShpMessage * msg, ShpMessageType type)
{
  g_return_if_fail (IS_SHP_MESSAGE (msg));

  g_object_set (G_OBJECT (msg), "type", type, NULL);
}
