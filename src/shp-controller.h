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

/*
 * A ShpController monitors the event bus for events and controls all
 * components.
 */

#ifndef __SHP_CONTROLLER_H__
#define __SHP_CONTROLLER_H__

#include <glib.h>
#include <glib-object.h>

#include "shp-component.h"
#include "shp-rule.h"

G_BEGIN_DECLS

#define SHP_CONTROLLER_TYPE (shp_controller_get_type ())
#define SHP_CONTROLLER(object) (G_TYPE_CHECK_INSTANCE_CAST ((object), SHP_CONTROLLER_TYPE,ShpController))
#define SHP_CONTROLLER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), SHP_CONTROLLER_TYPE, ShpControllerClass))
#define IS_SHP_CONTROLLER(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), SHP_CONTROLLER_TYPE))
#define IS_SHP_CONTROLLER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SHP_CONTROLLER_TYPE))
#define SHP_CONTROLLER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), SHP_CONTROLLER_TYPE, ShpControllerClass))

typedef struct _ShpController ShpController;
typedef struct _ShpControllerPrivate ShpControllerPrivate;
typedef struct _ShpControllerClass ShpControllerClass;

/**
 * ShpController:
 *
 */
struct _ShpController {
  ShpComponent parent;

  /*< protected >*/

  /*< private >*/
  ShpControllerPrivate *priv;
};

/**
 * ShpControllerClass:
 *
 */
struct _ShpControllerClass {
  ShpComponentClass parent_class;

  /*< private >*/

  /* signal callbacks */

  /*< protected >*/

  /* virtual methods for subclasses */
};

ShpController* shp_controller_new ();
void shp_controller_add_rule (ShpController * controller, ShpRule * rule);

GType shp_controller_get_type (void);

G_END_DECLS

#endif /* __SHP_CONTROLLER_H__ */
