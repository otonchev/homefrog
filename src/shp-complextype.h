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
 * ShpComplextype — complextype containing a set of other types.
 */

#ifndef __SHP_COMPLEXTYPE_H__
#define __SHP_COMPLEXTYPE_H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define SHP_COMPLEXTYPE_TYPE (shp_complextype_get_type ())
#define SHP_COMPLEXTYPE(object) (G_TYPE_CHECK_INSTANCE_CAST ((object), SHP_COMPLEXTYPE_TYPE,ShpComplextype))
#define SHP_COMPLEXTYPE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), SHP_COMPLEXTYPE_TYPE, ShpComplextypeClass))
#define IS_SHP_COMPLEXTYPE(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), SHP_COMPLEXTYPE_TYPE))
#define IS_SHP_COMPLEXTYPE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SHP_COMPLEXTYPE_TYPE))
#define SHP_COMPLEXTYPE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), SHP_COMPLEXTYPE_TYPE, ShpComplextypeClass))

typedef struct _ShpComplextype ShpComplextype;
typedef struct _ShpComplextypePrivate ShpComplextypePrivate;
typedef struct _ShpComplextypeClass ShpComplextypeClass;

typedef void (*ShpComplextypeFunc) (const gchar * name, const GValue * value,
    gpointer user_data);

struct _ShpComplextype {
  GObject parent;

  /*< protected >*/

  /*< private >*/
  ShpComplextypePrivate *priv;
};

struct _ShpComplextypeClass {
  GObjectClass parent_class;

  /*< private >*/
};

ShpComplextype* shp_complextype_new ();

void shp_complextype_add_string (ShpComplextype * self, const gchar * name, const gchar * value);
void shp_complextype_add_integer (ShpComplextype * self, const gchar * name, gint value);
void shp_complextype_add_double (ShpComplextype * self, const gchar * name, gdouble value);
void shp_complextype_add_boolean (ShpComplextype * self, const gchar * name, gboolean value);
void shp_complextype_add_long (ShpComplextype * self, const gchar * name, glong value);

const gchar* shp_complextype_get_string (ShpComplextype * self, const gchar * name);
gboolean shp_complextype_get_integer (ShpComplextype * self, const gchar * name, gint * value);
gboolean shp_complextype_get_double (ShpComplextype * self, const gchar * name, gdouble * value);
gboolean shp_complextype_get_boolean (ShpComplextype * self, const gchar * name, gboolean * value);
gboolean shp_complextype_get_long (ShpComplextype * self, const gchar * name, glong * value);

gboolean shp_complextype_has_value (ShpComplextype * self, const gchar * name, GType type);
GType shp_complextype_get_field_type (ShpComplextype * self, const gchar * name);
guint shp_complextype_size (ShpComplextype * self);
void shp_complextype_foreach (ShpComplextype * self, ShpComplextypeFunc func, gpointer user_data);

GType shp_complextype_get_type (void);

G_END_DECLS

#endif /* __SHP_COMPLEXTYPE_H__ */
