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
#include <glib.h>
#include <glib-object.h>

#include "shp-complextype.h"

#ifndef __SHP_COMPLEXTYPE_FACTORY_H__
#define __SHP_COMPLEXTYPE_FACTORY_H__

ShpComplextype * shp_complextype_factory_create (gchar * name);
gboolean shp_complextype_factory_register (gchar * name, GType object_type);
gchar** shp_complextype_factory_get_complextype_list ();
gboolean shp_complextype_factory_setup ();
void shp_complextype_factory_cleanup ();

#endif /* __SHP_COMPLEXTYPE_FACTORY_H__ */
