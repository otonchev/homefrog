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
 * ShpStructure — Generic structure containing fields of names and values.
 */

#ifndef __SHP_STRUCTURE_H__
#define __SHP_STRUCTURE_H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define SHP_STRUCTURE_TYPE (shp_structure_get_type ())
#define SHP_STRUCTURE(object) (G_TYPE_CHECK_INSTANCE_CAST ((object), SHP_STRUCTURE_TYPE,ShpStructure))
#define SHP_STRUCTURE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), SHP_STRUCTURE_TYPE, ShpStructureClass))
#define IS_SHP_STRUCTURE(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), SHP_STRUCTURE_TYPE))
#define IS_SHP_STRUCTURE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SHP_STRUCTURE_TYPE))
#define SHP_STRUCTURE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), SHP_STRUCTURE_TYPE, ShpStructureClass))

typedef struct _ShpStructure ShpStructure;
typedef struct _ShpStructurePrivate ShpStructurePrivate;
typedef struct _ShpStructureClass ShpStructureClass;

typedef void (*ShpStructureFunc) (const gchar * name, const GValue * value,
    gpointer user_data);

struct _ShpStructure {
  GObject parent;

  /*< protected >*/

  /*< private >*/
  ShpStructurePrivate *priv;
};

struct _ShpStructureClass {
  GObjectClass parent_class;

  /*< private >*/
};

ShpStructure* shp_structure_new ();

void shp_structure_add_string (ShpStructure * msg, const gchar * name, const gchar * value);
void shp_structure_add_integer (ShpStructure * msg, const gchar * name, gint value);
void shp_structure_add_double (ShpStructure * msg, const gchar * name, gdouble value);
void shp_structure_add_boolean (ShpStructure * msg, const gchar * name, gboolean value);
void shp_structure_add_long (ShpStructure * msg, const gchar * name, glong value);

const gchar* shp_structure_get_string (ShpStructure * msg, const gchar * name);
gboolean shp_structure_get_integer (ShpStructure * msg, const gchar * name, gint * value);
gboolean shp_structure_get_double (ShpStructure * msg, const gchar * name, gdouble * value);
gboolean shp_structure_get_boolean (ShpStructure * msg, const gchar * name, gboolean * value);
gboolean shp_structure_get_long (ShpStructure * msg, const gchar * name, glong * value);

gboolean shp_structure_has_value (ShpStructure * msg, const gchar * name, GType type);
GType shp_structure_get_field_type (ShpStructure * msg, const gchar * name);
guint shp_structure_size (ShpStructure * msg);
void shp_structure_foreach (ShpStructure * msg, ShpStructureFunc func, gpointer user_data);

GType shp_structure_get_type (void);

G_END_DECLS

#endif /* __SHP_STRUCTURE_H__ */
