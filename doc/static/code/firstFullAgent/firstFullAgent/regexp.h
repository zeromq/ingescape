//
//  regexp.h
//  firstFullAgent
//
//  Created by Stephane Vales on 09/05/2018.
//  Copyright © 2018 IngeScape. All rights reserved.
//

#ifndef regexp_h
#define regexp_h

#include <stdio.h>
#include <regex.h>

//size of a buffer to contain any error messages
//encountered when the regular expression is compiled
#define MAX_ERROR_MSG 1024

//maximum number of matches allowed
#define MAX_MATCHES 30
#define BUFFER_SIZE 1024

//structure used to store the matching result
typedef struct matches {
    int nb;
    char results[MAX_MATCHES][BUFFER_SIZE];
} matches_t;

// transform a string pattern into a compiled regex
int compile_regex (regex_t * r, const char * regex_text);

// get the matching result from regex compiled pattern
int match_regex (regex_t * r, const char * to_match, matches_t *collector);

// clean the collector matches
void clean_matches(matches_t *collector);

#endif /* regexp_h */
