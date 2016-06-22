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
#ifndef __SHP_JSON_H__
#define __SHP_JSON_H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

typedef struct _ShpJsonNode ShpJsonNode;

ShpJsonNode* shp_json_node_new_number (const gchar * name, gdouble value);
ShpJsonNode* shp_json_node_new_string (const gchar * name, const gchar * value);
ShpJsonNode* shp_json_node_new_boolean (const gchar * name, gboolean value);
ShpJsonNode* shp_json_node_new_array (const gchar * name);
ShpJsonNode* shp_json_node_new_object (const gchar * name);

void shp_json_node_free (ShpJsonNode * node);

gboolean shp_json_node_append_element (ShpJsonNode * array,
    ShpJsonNode * element);

gchar* shp_json_node_to_string (ShpJsonNode * node);

G_END_DECLS

#endif /* __SHP_TELLDUS_H__ */
