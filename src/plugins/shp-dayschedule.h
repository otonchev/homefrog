/* The SmartHomeProject.org
 * Copyright (C) 2015  Ognyan Tonchev <otonchev at gmail.com >
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
#ifndef __SHP_DAYSCHEDULE_H__
#define __SHP_DAYSCHEDULE_H__

#include <glib.h>
#include <glib-object.h>

#include "../shp-plugin.h"

G_BEGIN_DECLS

#define SHP_DAYSCHEDULE_TYPE (shp_dayschedule_get_type ())
#define SHP_DAYSCHEDULE(object) (G_TYPE_CHECK_INSTANCE_CAST ((object), SHP_DAYSCHEDULE_TYPE,ShpDayschedule))
#define SHP_DAYSCHEDULE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), SHP_DAYSCHEDULE_TYPE, ShpDayscheduleClass))
#define IS_SHP_DAYSCHEDULE(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), SHP_DAYSCHEDULE_TYPE))
#define IS_SHP_DAYSCHEDULE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SHP_DAYSCHEDULE_TYPE))
#define SHP_DAYSCHEDULE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), SHP_DAYSCHEDULE_TYPE, ShpDayscheduleClass))

typedef struct _ShpDayschedule ShpDayschedule;
typedef struct _ShpDayscheduleClass ShpDayscheduleClass;

struct _ShpDayschedule {
  ShpPlugin parent;

  /*< protected >*/

  /*< private >*/
  GSource *dispatch_source;

  gint minutes;
  gint hour;
  gboolean saturday;
  gboolean sunday;
  gboolean monday;
  gboolean tuesday;
  gboolean wednesday;
  gboolean thursday;
  gboolean friday;

  gboolean active;
};

struct _ShpDayscheduleClass {
  ShpPluginClass parent_class;

  /*< private >*/
};

void shp_plugin_register (void);

GType shp_dayschedule_get_type (void);

G_END_DECLS

#endif /* __SHP_DAYSCHEDULE_H__ */
