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
#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

#include "shp-bus.h"
#include "shp-controller.h"
#include "shp-complextype-factory.h"
#include "shp-component.h"
#include "shp-group.h"
#include "shp-message.h"
#include "shp-plugin-factory.h"
#include "shp-rule.h"
#include "shp-scene.h"

int
main (int argc, char *argv[])
{
  GMainLoop *loop;
  GKeyFile *file;
  gchar *plugin_dir;
  gchar *file_name;

  if (argc != 2) {
    g_critical ("usage: %s path_to_shp_conf_file", argv[0]);
    exit (1);
  }

  file_name = argv[1];

  g_debug ("using config: %s", file_name);

  if (!shp_complextype_factory_setup ()) {
    g_critical ("unable to set up complextype factory");
    exit (1);
  }

  /* read plugin dir and load all available plugins */
  file = g_key_file_new ();
  if (!g_key_file_load_from_file (file, file_name, G_KEY_FILE_NONE, NULL)) {
    g_warning ("unable to load config file %s", file_name);
    g_key_file_free (file);
    exit (1);
  }

  plugin_dir = g_key_file_get_string (file, "program", "PluginDir", NULL);
  g_debug ("plugin dir from configuration file: %s", plugin_dir);

  if (!shp_plugin_factory_setup (plugin_dir)) {
    g_critical ("unable to set up plugin factory");
    exit (1);
  }

  g_key_file_free (file);

#if 0
  {
    /* Turn Heater in the Living room on floor 1 on if the temperature drops
     * below 17C and time is after 19:45.
     */
    ShpBus *bus;
    ShpGroup *group;
    ShpController *controller;
    ShpScene *scene;
    ShpRule *rule;
    ShpMessage *event;
    ShpCondition *condition_temp;
    ShpCondition *condition_time;
    ShpComplextype *complex_type;
    ShpPlugin *timer;
    ShpPlugin *temperature;
    ShpPlugin *telldus;

    /* load timer */
    timer = shp_plugin_factory_create ("timer", "/clock/timer");

    /* load ds1820digitemp plugin for obtaining temperature readings */
    temperature = shp_plugin_factory_create ("ds1820digitemp",
        "/home/floor1/LivingRoom/Temperature");
    g_object_set (G_OBJECT (temperature), "config-dir", "/home/pi/digitemp/",
        NULL);
    g_object_set (G_OBJECT (temperature), "device-id", "2", NULL);

    /* load telldus plugin for controlling heater */
    telldus = shp_plugin_factory_create ("telldus",
        "/home/floor1/LivingRoom/Heater");
    g_object_set (G_OBJECT (telldus), "device-id", 2, NULL);

    /* create the group and add the event bus, the controller and the plugins to
     * it */
    bus = shp_bus_new ();
    group = shp_group_new (bus);
    controller = shp_controller_new ();
    shp_group_add (group, SHP_COMPONENT (controller));
    shp_group_add (group, SHP_COMPONENT (timer));
    shp_group_add (group, SHP_COMPONENT (temperature));
    shp_group_add (group, SHP_COMPONENT (telldus));

    /* create scene with one event */
    scene = shp_scene_new (g_object_ref (bus));
    event = shp_message_new_command ("/home/floor1/LivingRoom/Heater");
    shp_message_add_string (event, "command", "on");
    shp_scene_add_event (scene, event);

    /* create the 2 conditions to be checked before activating the scene */
    condition_temp = shp_condition_new ("/home/floor1/LivingRoom/Temperature");
    shp_condition_add_double_option (condition_temp, "temperature", 15,
        SHP_CONDITION_OPERATOR_GT);

    condition_time = shp_condition_new ("/clock/timer");
    complex_type = shp_complextype_factory_create ("timer.time");
    shp_complextype_add_integer (complex_type, "hour", 19);
    shp_complextype_add_integer (complex_type, "minutes", 45);
    shp_condition_add_complextype_option (condition_time, "datetime",
        complex_type, SHP_CONDITION_OPERATOR_GT);

    /* add 2 conditions and scene to rule */
    rule = shp_rule_new ();
    shp_rule_add_condition (rule, condition_temp);
    shp_rule_add_condition (rule, condition_time);
    shp_rule_set_scene (rule, scene);

    /* let the controller know about our new rule */
    shp_controller_add_rule (controller, rule);

    /* now start the group */
    shp_component_start (SHP_COMPONENT (group));
  }
#endif

  /* create and run the main loop */
  loop = g_main_loop_new (NULL, TRUE);
  g_main_loop_run (loop);

  return 0;
}
