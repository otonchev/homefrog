Homefrog v0.1
-------------

Plugin-based Home Automation framework written in C/GObject

Current state: C APIs are more or less in place, no documentation yet, tested
on Raspberry Pi

Code is designed for efficiency and testability, although no unit tests are
available yet ;-)

All plugins are built and installed separately, they can be found under:
"src/plugins" for the sources and all .so files are built in
"src/plugins/.libs/". The application will automatically load them at startup.
Plugins are free to load plugin-specific complex data types as well when basic
types like integer, string, double, etc are not enough.


Setting up devices / Configuring plugins:
-----------------------------------------

Plugins are documented in docs/plugins.txt and docs/*.txt


DEMO (minimalistic web interface):
----------------------------------

The web page uses the REST API and dynamically generates content. For more info
look in the web/ folder.

http://ogy.noip.me:3456/demo.html

Screenshot:
[![Homefrog](https://raw.githubusercontent.com/otonchev/homefrog/master/images/homefrog.png)]()
https://raw.githubusercontent.com/otonchev/homefrog/master/images/homefrog.png


DEMO (REST API, see below for description):
-------------------------------------------

http://ogy.noip.me:5678/home
http://ogy.noip.me:5678/web


Features under development:
---------------------------

    * sorting out format for configure files (so that app can be configured
          without writing C-code :-))
    * Web pages (not really started yet, working on necessary underlying
          infrastructure, although creating simple Web UI can be done with no
          effort even today)


How to build and run:
---------------------

    only first time:

    aclocal
    autoconf
    automake --add-missing

    and then:

    ./configure
    make
    G_MESSAGES_DEBUG=all ./src/homefrog ./src/homefrog.config


API architecture:
-----------------

    +---------------------------------------------------------------+
    | Group                                                         |
    |                                                               |
    | +-----------------+  +---------+       +-----------------+    |
    | |                 |  |  Timer  |       |         +------+|    |
    | |   Controller    |  +---------+       |  REST   | Http ||    |
    | |                 |       |            |         +------+|    |
    | +-----------------+       |            +-----------------+    |
    |          |                |                     |             |
    |          v                v                     v             |
    | +-----------------------------------------------------------+ |
    | |                         EventBUS                          | |
    | +-----------------------------------------------------------+ |
    |          ^                    ^                     ^         |
    |          |                    |                     |         |
    | +----------------+   +-----------------+   +----------------+ |
    | | ds1820digitemp |   |    telldus      |   |      MySQL     | |
    | |    Plugin      |   |     Plugin      |   |     Plugin     | |
    | +----------------+   +-----------------+   +----------------+ |
    |                                                               |
    +---------------------------------------------------------------+


Example 1:
----------

Using the guts of the API to control a heater based on the temperature in the
room and the time:

    /* Turn Heater in the Living room on floor 1 on if the temperature drops
     * below 17C and time is after 19:45.
     */
    ShpBus *bus;
    ShpGroup *group;
    ShpController *controller;
    ShpScene *scene;
    ShpHttp *http;
    ShpRule *rule;
    ShpMessage *event;
    ShpCondition *condition_temp;
    ShpCondition *condition_time;
    ShpComplextype *complex_type;
    ShpPlugin *rest;
    ShpPlugin *timer;
    ShpPlugin *temperature;
    ShpPlugin *telldus;

    /* load REST plugin, API will be available on port 1234 */
    http = shp_http_new (1234);
    rest = shp_plugin_factory_create ("rest", NULL);
    /* this file is optional and describes how different plugins should be
     * visualised */
    g_object_set (G_OBJECT (rest), "config-file", "src/homefrog-rest.config",
        NULL);
    /* provide HTTP instance to the REST plugin */
    g_object_set (G_OBJECT (rest), "http", http, NULL);
    /* expose all /home* devices, if other devices are present, they will not
     * be visible unless unabled the same way */
    g_signal_emit_by_name (G_OBJECT (rest), "add-device-path", "/home*");

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
    shp_group_add (group, SHP_COMPONENT (rest));
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

    /* add the 2 conditions and the scene to a rule */
    rule = shp_rule_new ();
    shp_rule_add_condition (rule, condition_temp);
    shp_rule_add_condition (rule, condition_time);
    shp_rule_set_scene (rule, scene);

    /* let the controller know about our new rule */
    shp_controller_add_rule (controller, rule);

    /* now start the group */
    shp_component_start (SHP_COMPONENT (group));

    /* the group owns and manages all objects added to it, they will be
     * automatically freed when the group itself is freed */


Example 2:
----------

Turn off all heaters in the Living room remotely. Extend Example 1 with the
lines below:


    /* load telldus plugins for controlling heaters */
    telldus = shp_plugin_factory_create ("telldus",
        "/home/floor1/LivingRoom/Heater1");
    g_object_set (G_OBJECT (telldus), "device-id", 2, NULL);
    shp_group_add (group, SHP_COMPONENT (telldus));

    telldus2 = shp_plugin_factory_create ("telldus",
        "/home/floor1/LivingRoom/Heater2");
    g_object_set (G_OBJECT (telldus2), "device-id", 1, NULL);
    shp_group_add (group, SHP_COMPONENT (telldus2));

    /* load scenectl plugin for turning off all heaters in the Living room
     * simultaneously through the REST API */
    scenectl = shp_plugin_factory_create ("scenectl",
        "/home/floor1/LivingRoom/TurnHeatersOff");
    scene = shp_scene_new (g_object_ref (bus));
    event = shp_message_new_command ("/home/floor1/LivingRoom/Heater1");
    shp_message_add_string (event, "command", "off");
    shp_scene_add_event (scene, event);
    event = shp_message_new_command ("/home/floor1/LivingRoom/Heater2");
    shp_message_add_string (event, "command", "off");
    shp_scene_add_event (scene, event);
    g_object_set (G_OBJECT (scenectl), "scene", scene, NULL);
    shp_group_add (group, SHP_COMPONENT (scenectl));

To turn off both heaters simultaneously call
"/home/floor1/LivingRoom/TurnHeatersOff" as shown below.


Example 3:
----------

Turn-on Lamp1 and Lamp2 in the Living room on floor 1 on Wednesdays and
Thursdays at 19:00.

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


Accessing the REST API:
-----------------------

The API is provided by the "rest" plugin and returns Json responses.
It is automatically enabled when the plugin is loaded.


http://<ip>/web
---------------

To get a description on how all the different plugins are to be visualised on
the Web (all that is generated based on the content of the rest configure file:
the src/homefrog-rest.config):

GET http://192.168.0.240:6666/web

>output:

    {
        "ds1820digitemp":{
            "device-type":"sensor-simple",
            "display-options":[
                {
                    "option":"temperature",
                    "type":"double"
                },
                {
                    "option":"units",
                    "type":"string"
                }
            ]
        },
        "telldus":{
            "device-type":"switch-simple",
            "display-options":[
                {
                    "option":"status",
                    "type":"string"
                }
            ]
        },
        "scenectl":{
            "device-type":"scene",
            "display-options":[

            ]
        },
        "grovedust":{
            "device-type":"sensor-simple",
            "display-options":[
                {
                    "option":"aqi",
                    "type":"integer"
                }
            ]
        }
    }

http://<ip>/home*
-----------------

To display all components running on the system:

GET http://192.168.0.240:6666/home

>output:

    {
        "/home/floor1/Bedroom2/Temperature":{
            "units":"C",
            "device-id":"4",
            "units_humanized":"Celsius",
            "rest.timestamp":"2016-06-23 15-28-39",
            "temperature":25.799999,
            "name":"ds1820digitemp"
        },
        "/home/floor1/LivingRoom/Temperature":{
            "units":"C",
            "device-id":"2",
            "units_humanized":"Celsius",
            "rest.timestamp":"2016-06-23 15-28-39",
            "temperature":23,
            "name":"ds1820digitemp"
        },
        "/home/floor1/Garage/Temperature":{
            "units":"C",
            "device-id":"1",
            "units_humanized":"Celsius",
            "rest.timestamp":"2016-06-23 15-28-39",
            "temperature":19.400000,
            "name":"ds1820digitemp"
        },
        "/home/floor1/Bedroom1/Temperature":{
            "units":"C",
            "device-id":"3",
            "units_humanized":"Celsius",
            "rest.timestamp":"2016-06-23 15-28-39",
            "temperature":22.900000,
            "name":"ds1820digitemp"
        },
        "/home/floor1/LivingRoom/AllDevicesOff":{
            "/home/floor1/LivingRoom/Heater1":"no_name",
            "/home/floor1/LivingRoom/Heater2":"no_name",
            "name":"scenectl",
            "rest.timestamp":"2016-06-23 15-28-39"
        },
        "/home/floor1/LivingRoom/Heater1":{
            "device-id":2,
            "status":"on",
            "rest.timestamp":"2016-06-23 15-28-39",
            "name":"telldus"
        },
        "/home/floor1/StorageRoom/Temperature":{
            "units":"C",
            "device-id":"0",
            "units_humanized":"Celsius",
            "rest.timestamp":"2016-06-23 15-28-39",
            "temperature":22.400000,
            "name":"ds1820digitemp"
        },
        "/home/floor1/LivingRoom/Heater2":{
            "device-id":1,
            "status":"off",
            "rest.timestamp":"2016-06-23 15-28-39",
            "name":"telldus"
        }
    }

To display all sensors in the Living room on the 1st floor:

GET http://192.168.0.240:6666/home/floor1/LivingRoom/

>output:

    {
        "/home/floor1/LivingRoom/Temperature":{
            "units":"C",
            "rest.timestamp":"2016-06-22 09-24-56",
            "units_humanized":"Celsius",
            "temperature":20.600000,
            "device-id":"2",
            "name":"ds1820digitemp"
        },
        "/home/floor1/LivingRoom/Heater":{
            "status":"on",
            "device-id":2,
            "rest.timestamp":"2016-06-22 09-24-56",
            "name":"telldus"
        }
    }

To display status for the Heater in the Living room:

GET http://192.168.0.240:6666/home/floor1/LivingRoom/Heater

>output:

    {
        "/home/floor1/LivingRoom/Heater":{
            "status":"on",
            "device-id":2,
            "rest.timestamp":"2016-06-22 09-24-56",
            "name":"telldus"
        }
    }

To display history of all status updates for a particular sensor:

GET http://192.168.0.240:6666/home/floor1/LivingRoom/Temperature?history

>output:

    {
        "/home/floor1/LivingRoom/Temperature":[
            {
                "units":"C",
                "units_humanized":"Celsius",
                "rest.timestamp":"2016-06-22 09-32-51",
                "temperature":20.800000,
                "device-id":"2",
                "name":"ds1820digitemp"
            },
            {
                "units":"C",
                "units_humanized":"Celsius",
                "rest.timestamp":"2016-06-22 09-31-51",
                "temperature":20.800000,
                "device-id":"2",
                "name":"ds1820digitemp"
            },
            {
                "units":"C",
                "units_humanized":"Celsius",
                "rest.timestamp":"2016-06-22 09-30-51",
                "temperature":20.800000,
                "device-id":"2",
                "name":"ds1820digitemp"
            },
            {
                "units":"C",
                "units_humanized":"Celsius",
                "rest.timestamp":"2016-06-22 09-29-51",
                "temperature":20.600000,
                "device-id":"2",
                "name":"ds1820digitemp"
            },
            {
                "units":"C",
                "units_humanized":"Celsius",
                "rest.timestamp":"2016-06-22 09-28-51",
                "temperature":20.600000,
                "device-id":"2",
                "name":"ds1820digitemp"
            },
            {
                "units":"C",
                "units_humanized":"Celsius",
                "rest.timestamp":"2016-06-22 09-27-51",
                "temperature":20.600000,
                "device-id":"2",
                "name":"ds1820digitemp"
            },
            {
                "units":"C",
                "units_humanized":"Celsius",
                "rest.timestamp":"2016-06-22 09-26-51",
                "temperature":20.600000,
                "device-id":"2",
                "name":"ds1820digitemp"
            },
            {
                "units":"C",
                "units_humanized":"Celsius",
                "rest.timestamp":"2016-06-22 09-25-51",
                "temperature":20.600000,
                "device-id":"2",
                "name":"ds1820digitemp"
            },
            {
                "units":"C",
                "units_humanized":"Celsius",
                "rest.timestamp":"2016-06-22 09-24-56",
                "temperature":20.600000,
                "device-id":"2",
                "name":"ds1820digitemp"
            }
        ]
    }

To activate the Heater in the Living room on floor 1:

POST http://192.168.0.240:6666/home/floor1/LivingRoom/Heater

To activate scene "TurnHeatersOff":

POST http://192.168.0.240:6666/home/floor1/LivingRoom/TurnHeatersOff
