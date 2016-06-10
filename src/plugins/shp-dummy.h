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
#ifndef __SHP_DUMMY_H__
#define __SHP_DUMMY_H__

#include <glib.h>
#include <glib-object.h>

#include "../shp-plugin.h"

G_BEGIN_DECLS

#define SHP_DUMMY_TYPE (shp_dummy_get_type ())
#define SHP_DUMMY(object) (G_TYPE_CHECK_INSTANCE_CAST ((object), SHP_DUMMY_TYPE,ShpDummy))
#define SHP_DUMMY_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), SHP_DUMMY_TYPE, ShpDummyClass))
#define IS_SHP_DUMMY(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), SHP_DUMMY_TYPE))
#define IS_SHP_DUMMY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SHP_DUMMY_TYPE))
#define SHP_DUMMY_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), SHP_DUMMY_TYPE, ShpDummyClass))

typedef struct _ShpDummy ShpDummy;
typedef struct _ShpDummyClass ShpDummyClass;

struct _ShpDummy {
  ShpPlugin parent;

  /*< protected >*/

  /*< private >*/
};

struct _ShpDummyClass {
  ShpPluginClass parent_class;

  /*< private >*/
};

void shp_plugin_register (void);

GType shp_dummy_get_type (void);

G_END_DECLS

#endif /* __SHP_DUMMY_H__ */
