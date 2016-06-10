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
 * This is a dummy plugin intended to be used in configurations where
 * actions are not of an interest. For example monitoring sensors in
 * order to be able to answer queries about readings.
 */

#include <glib.h>

#include "../shp-plugin-factory.h"
#include "shp-dummy.h"

#define NAME "dummy"


G_DEFINE_TYPE (ShpDummy, shp_dummy, SHP_PLUGIN_TYPE);

static void
shp_dummy_class_init (ShpDummyClass * klass)
{
}

static void
shp_dummy_init (ShpDummy * self)
{
}

void
shp_plugin_register (void)
{
  shp_plugin_factory_register (NAME, SHP_DUMMY_TYPE);
}
