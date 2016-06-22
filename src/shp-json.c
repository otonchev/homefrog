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

#include "shp-json.h"

typedef enum ShpJsonNodeType
{
  SHP_JSON_NODE_NULL,
  SHP_JSON_NODE_NUMBER,
  SHP_JSON_NODE_STRING,
  SHP_JSON_NODE_BOOLEAN,
  SHP_JSON_NODE_OBJECT,
  SHP_JSON_NODE_ARRAY,
} ShpJsonNodeType;

struct _ShpJsonNode {
  gchar * name;
  ShpJsonNodeType type;

  union {
    gdouble number; /* number */
    gboolean boolean; /* boolean */
    gchar *string; /* string */
    GPtrArray *children; /* array, object */
  };
};

static void
free_node (ShpJsonNode * node)
{
  if (node == NULL)
    return;

  switch (node->type) {
    case SHP_JSON_NODE_STRING:
      g_free (node->string);
      node->string = NULL;
      break;
    case SHP_JSON_NODE_OBJECT:
    case SHP_JSON_NODE_ARRAY:
      g_ptr_array_unref (node->children);
      break;
    default:
      break;
  }

  g_free (node->name);
  node->name = NULL;
  g_free (node);
}

static ShpJsonNode*
create_node (const gchar * name, ShpJsonNodeType type)
{
  ShpJsonNode *result;

  result = g_new0 (ShpJsonNode, 1);
  result->type = type;
  result->name = g_strdup (name);

  return result;
}

ShpJsonNode*
shp_json_node_new_number (const gchar * name, gdouble value)
{
  ShpJsonNode *result = create_node (name, SHP_JSON_NODE_NUMBER);
  result->number = value;

  return result;
}

ShpJsonNode*
shp_json_node_new_string (const gchar * name, const gchar * value)
{
  ShpJsonNode *result = create_node (name, SHP_JSON_NODE_STRING);
  result->string = g_strdup (value);

  return result;
}

ShpJsonNode*
shp_json_node_new_boolean (const gchar * name, gboolean value)
{
  ShpJsonNode *result = create_node (name, SHP_JSON_NODE_BOOLEAN);
  result->boolean = value;

  return result;
}

ShpJsonNode*
shp_json_node_new_array (const gchar * name)
{
  ShpJsonNode *result = create_node (name, SHP_JSON_NODE_ARRAY);

  result->children =
      g_ptr_array_new_with_free_func ((GDestroyNotify) free_node);

  return result;
}

ShpJsonNode*
shp_json_node_new_object (const gchar * name)
{
  ShpJsonNode *result = create_node (name, SHP_JSON_NODE_OBJECT);

  result->children =
      g_ptr_array_new_with_free_func ((GDestroyNotify) free_node);

  return result;
}

gboolean
shp_json_node_append_element (ShpJsonNode * node, ShpJsonNode * element)
{
  g_return_val_if_fail (node != NULL, FALSE);
  g_return_val_if_fail (node->type == SHP_JSON_NODE_OBJECT ||
      node->type == SHP_JSON_NODE_ARRAY, FALSE);

  g_ptr_array_add (node->children, element);
  return TRUE;
}

void
shp_json_node_free (ShpJsonNode * node)
{
  free_node (node);
}

static void
append_spaces (GString * string, guint num)
{
  guint i;

  for (i = 0; i < num; i++)
    g_string_append (string, " ");
}

static void
to_string (ShpJsonNode * node, GString * accum, guint level)
{
  gint i;

  switch (node->type) {
    case SHP_JSON_NODE_NUMBER:
      append_spaces (accum, level * 4);
      if (node->number - (gint)node->number == 0)
        g_string_append_printf (accum, "\"%s\":%d", node->name,
            (gint)node->number);
      else
        g_string_append_printf (accum, "\"%s\":%f", node->name, node->number);
      break;

    case SHP_JSON_NODE_BOOLEAN:
      append_spaces (accum, level * 4);
      g_string_append_printf (accum, "\"%s\":%s", node->name,
          (node->boolean) ? "true" : "false");
      break;

    case SHP_JSON_NODE_STRING:
      append_spaces (accum, level * 4);
      g_string_append_printf (accum, "\"%s\":\"%s\"", node->name, node->string);
      break;

    case SHP_JSON_NODE_OBJECT:
      append_spaces (accum, level * 4);
      if (node->name)
        g_string_append_printf (accum, "\"%s\":{\n", node->name);
      else
        g_string_append (accum, "{\n");

      for (i = 0; i < node->children->len; i++) {
        ShpJsonNode *child = g_ptr_array_index (node->children, i);
        if (i > 0)
          g_string_append (accum, ",\n");
        to_string (child, accum, level + 1);
      }

      g_string_append (accum, "\n");
      append_spaces (accum, level * 4);
      g_string_append (accum, "}");
      break;

    case SHP_JSON_NODE_ARRAY:
      append_spaces (accum, level * 4);
      g_string_append_printf (accum, "\"%s\":[\n", node->name);

      for (i = 0; i < node->children->len; i++) {
        ShpJsonNode *child = g_ptr_array_index (node->children, i);
        if (i > 0)
          g_string_append (accum, ",\n");
        to_string (child, accum, level + 1);
      }

      g_string_append (accum, "\n");
      append_spaces (accum, level * 4);
      g_string_append (accum, "]");
      break;

    default:
      break;
  }
}

gchar*
shp_json_node_to_string (ShpJsonNode * node)
{
  GString *string;
  gchar *result;

  string = g_string_new (NULL);

  to_string (node, string, 1);

  result = g_string_free (string, FALSE);

  return result;
}
