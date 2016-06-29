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
#include "shp-http.h"
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
    g_critical ("usage: %s path_to_homefrog_conf_file", argv[0]);
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
    /* Turn-on Lamp1 and Lamp2 in the Living room on floor 1
     * Wednesday-Thursday at 19:00.
     */
    ShpBus *bus;
    ShpGroup *group;
    ShpController *controller;
    ShpScene *scene;
    ShpRule *rule;
    ShpMessage *event;
    ShpCondition *condition_time;
    ShpComplextype *complex_type;
    ShpHttp *http;
    ShpPlugin *rest;
    ShpPlugin *timer;
    ShpPlugin *temperature;
    ShpPlugin *temperature2;
    ShpPlugin *temperature3;
    ShpPlugin *temperature4;
    ShpPlugin *temperature5;
    ShpPlugin *telldus;
    ShpPlugin *telldus2;
    ShpPlugin *scenectl;
    ShpPlugin *grovedust;

    /* load REST plugin, API will be available on port 1234 */
    http = shp_http_new (1234);
    rest = shp_plugin_factory_create ("rest", NULL);
    /* this file is optional and describes how different plugins should be
     * visualised */
    g_object_set (G_OBJECT (rest), "config-file", "src/homefrog-rest.config",
        NULL);
    /* provide HTTP instance to the REST plugin */
    g_object_set (G_OBJECT (rest), "http", http, NULL);
    /* expose all /home devices, if other devices are present, they will not
     * be visible unless unabled the same way */
    g_signal_emit_by_name (G_OBJECT (rest), "add-device-path", "/home*");

    /* load timer */
    timer = shp_plugin_factory_create ("timer", "/clock/timer");

    /* load ds1820digitemp plugins for obtaining temperature readings */
    temperature = shp_plugin_factory_create ("ds1820digitemp",
        "/home/floor1/LivingRoom/Temperature");
    g_object_set (G_OBJECT (temperature), "config-dir", "/home/pi/digitemp/",
        NULL);
    g_object_set (G_OBJECT (temperature), "device-id", "2", NULL);

    temperature2 = shp_plugin_factory_create ("ds1820digitemp",
        "/home/floor1/Garage/Temperature");
    g_object_set (G_OBJECT (temperature2), "config-dir", "/home/pi/digitemp/",
        NULL);
    g_object_set (G_OBJECT (temperature2), "device-id", "1", NULL);

    temperature3 = shp_plugin_factory_create ("ds1820digitemp",
        "/home/floor1/StorageRoom/Temperature");
    g_object_set (G_OBJECT (temperature3), "config-dir", "/home/pi/digitemp/",
        NULL);
    g_object_set (G_OBJECT (temperature3), "device-id", "0", NULL);

    temperature4 = shp_plugin_factory_create ("ds1820digitemp",
        "/home/floor1/Bedroom1/Temperature");
    g_object_set (G_OBJECT (temperature4), "config-dir", "/home/pi/digitemp/",
        NULL);
    g_object_set (G_OBJECT (temperature4), "device-id", "3", NULL);

    temperature5 = shp_plugin_factory_create ("ds1820digitemp",
        "/home/floor1/Bedroom2/Temperature");
    g_object_set (G_OBJECT (temperature5), "config-dir", "/home/pi/digitemp/",
        NULL);
    g_object_set (G_OBJECT (temperature5), "device-id", "4", NULL);

    /* load telldus plugin for controlling heater */
    telldus = shp_plugin_factory_create ("telldus",
        "/home/floor1/LivingRoom/Lamp1");
    g_object_set (G_OBJECT (telldus), "device-id", 2, NULL);

    telldus2 = shp_plugin_factory_create ("telldus",
        "/home/floor1/LivingRoom/Lamp2");
    g_object_set (G_OBJECT (telldus2), "device-id", 1, NULL);

    /* load grovedust plugin for measuring dust concentration */
    grovedust = shp_plugin_factory_create ("grovedust",
        "/home/floor1/DustConcentration");
    g_object_set (G_OBJECT (grovedust), "pin", 17, NULL);

    /* create the event bus */
    bus = shp_bus_new ();

    /* load scenectl plugin for controlling all devices in the Living room
     * simultaneously through the REST API */
    scene = shp_scene_new (g_object_ref (bus));
    event = shp_message_new_command ("/home/floor1/LivingRoom/Lamp1");
    shp_message_add_string (event, "command", "off");
    shp_scene_add_event (scene, event);
    event = shp_message_new_command ("/home/floor1/LivingRoom/Lamp2");
    shp_message_add_string (event, "command", "off");
    shp_scene_add_event (scene, event);
    scenectl = shp_plugin_factory_create ("scenectl",
        "/home/floor1/LivingRoom/AllDevicesOff");
    g_object_set (G_OBJECT (scenectl), "scene", scene, NULL);

    /* create the group and add the event bus, the controller and the plugins to
     * it */
    group = shp_group_new (bus);
    controller = shp_controller_new ();
    shp_group_add (group, SHP_COMPONENT (controller));
    shp_group_add (group, SHP_COMPONENT (rest));
    shp_group_add (group, SHP_COMPONENT (timer));
    shp_group_add (group, SHP_COMPONENT (temperature));
    shp_group_add (group, SHP_COMPONENT (temperature2));
    shp_group_add (group, SHP_COMPONENT (temperature3));
    shp_group_add (group, SHP_COMPONENT (temperature4));
    shp_group_add (group, SHP_COMPONENT (temperature5));
    shp_group_add (group, SHP_COMPONENT (telldus));
    shp_group_add (group, SHP_COMPONENT (telldus2));
    shp_group_add (group, SHP_COMPONENT (scenectl));
    shp_group_add (group, SHP_COMPONENT (grovedust));

    /* create scene with two events */
    scene = shp_scene_new (g_object_ref (bus));
    event = shp_message_new_command ("/home/floor1/LivingRoom/Lamp1");
    shp_message_add_string (event, "command", "on");
    shp_scene_add_event (scene, event);
    scene = shp_scene_new (g_object_ref (bus));
    event = shp_message_new_command ("/home/floor1/LivingRoom/Lamp2");
    shp_message_add_string (event, "command", "on");
    shp_scene_add_event (scene, event);

    /* create the condition to be checked before activating the scene */
    condition_time = shp_condition_new ("/clock/timer");
    /* Wednesday 19:00 */
    complex_type = shp_complextype_factory_create ("timer.datetime");
    shp_complextype_add_integer (complex_type, "week_day", 3); /* Wednesday */
    shp_complextype_add_integer (complex_type, "hour", 19);
    shp_complextype_add_integer (complex_type, "minutes", 0);
    shp_condition_add_complextype_option (condition_time, "datetime",
        complex_type, SHP_CONDITION_OPERATOR_EQ);
    /* Thursday 19:00 */
    complex_type = shp_complextype_factory_create ("timer.datetime");
    shp_complextype_add_integer (complex_type, "week_day", 4); /* Thursday */
    shp_complextype_add_integer (complex_type, "hour", 19);
    shp_complextype_add_integer (complex_type, "minutes", 0);
    shp_condition_add_complextype_option (condition_time, "datetime",
        complex_type, SHP_CONDITION_OPERATOR_EQ);

    /* add the condition and the scene to a rule */
    rule = shp_rule_new ();
    shp_rule_add_condition (rule, condition_time);
    shp_rule_set_scene (rule, scene);

    /* let the controller know about our new rule */
    shp_controller_add_rule (controller, rule);

    /* now start the group */
    shp_component_start (SHP_COMPONENT (group));

    /* the group owns and manages all objects added to it, they will be
     * automatically freed when the group itself is freed */
  }
#endif

  /* create and run the main loop */
  loop = g_main_loop_new (NULL, TRUE);
  g_main_loop_run (loop);

  return 0;
}
