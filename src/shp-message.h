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
 * ShpMessage is the basic unit of passing data from plugins.
 */

#ifndef __SHP_MESSAGE_H__
#define __SHP_MESSAGE_H__

#include <glib.h>
#include <glib-object.h>

#include "shp-complextype.h"

G_BEGIN_DECLS

#define SHP_MESSAGE_TYPE (shp_message_get_type ())
#define SHP_MESSAGE(object) (G_TYPE_CHECK_INSTANCE_CAST ((object), SHP_MESSAGE_TYPE,ShpMessage))
#define SHP_MESSAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), SHP_MESSAGE_TYPE, ShpMessageClass))
#define IS_SHP_MESSAGE(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), SHP_MESSAGE_TYPE))
#define IS_SHP_MESSAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SHP_MESSAGE_TYPE))
#define SHP_MESSAGE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), SHP_MESSAGE_TYPE, ShpMessageClass))

typedef struct _ShpMessage ShpMessage;
typedef struct _ShpMessagePrivate ShpMessagePrivate;
typedef struct _ShpMessageClass ShpMessageClass;

typedef void (*ShpMessageFunc) (const gchar * name, const GValue * value,
    gpointer user_data);

struct _ShpMessage {
  GObject parent;

  /*< protected >*/

  /*< private >*/
  ShpMessagePrivate *priv;
};

struct _ShpMessageClass {
  GObjectClass parent_class;

  /*< private >*/
};

ShpMessage* shp_message_new (const gchar * source_path);
ShpMessage* shp_message_new_command (const gchar * destination_path);
ShpMessage* shp_message_new_command_from_string (const gchar * destination_path, const gchar * options);

ShpMessage* shp_message_copy (const ShpMessage * message);

void shp_message_add_string (ShpMessage * msg, const gchar * name, const gchar * value);
void shp_message_add_integer (ShpMessage * msg, const gchar * name, gint value);
void shp_message_add_double (ShpMessage * msg, const gchar * name, gdouble value);
void shp_message_add_boolean (ShpMessage * msg, const gchar * name, gboolean value);
void shp_message_add_long (ShpMessage * msg, const gchar * name, glong value);
void shp_message_add_complextype (ShpMessage * msg, const gchar * name, ShpComplextype * value);

gboolean shp_message_has_value (ShpMessage * msg, const gchar * name, GType type);
GType shp_message_get_field_type (ShpMessage * msg, const gchar * name);

const gchar* shp_message_get_string (ShpMessage * msg, const gchar * name);
gboolean shp_message_get_integer (ShpMessage * msg, const gchar * name, gint * value);
gboolean shp_message_get_double (ShpMessage * msg, const gchar * name, gdouble * value);
gboolean shp_message_get_boolean (ShpMessage * msg, const gchar * name, gboolean * value);
gboolean shp_message_get_long (ShpMessage * msg, const gchar * name, glong * value);
const ShpComplextype * shp_message_get_complextype (ShpMessage * msg, const gchar * name);

guint shp_message_size (ShpMessage * msg);

void shp_message_foreach (ShpMessage * msg, ShpMessageFunc func, gpointer user_data);

const gchar* shp_message_get_name (ShpMessage * msg);
const gchar* shp_message_get_source_path (const ShpMessage * msg);
const gchar* shp_message_get_destination_path (const ShpMessage * msg);

gchar* shp_message_to_string (ShpMessage * msg);

GType shp_message_get_type (void);

G_END_DECLS

#endif /* __SHP_MESSAGE_H__ */
