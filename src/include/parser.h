//
//  mtic_parseur.h
//
//  Created by Patxi Berard
//  Modified by Vincent Deliencourt
//  Copyright © 2016 IKKY WP4.8. All rights reserved.
//

#ifndef MTIC_PARSER_H
#define MTIC_PARSER_H

#include "mapping.h"

category* load_category (const char* json_str);
category* load_category_from_path (const char* file_path);
const char* export_category (category* cat);
definition* load_definition (const char* json_str);
definition* load_definition_from_path (const char* file_path);
char* export_definition (definition* def);
mapping* load_map (const char* json_str);
mapping* load_map_from_path (const char* load_file);
int mtic_init_mapping (const char* mapping_file_path);
int mtic_init_internal_data (const char* definition_file_path);

#endif // MTIC_PARSER_H
