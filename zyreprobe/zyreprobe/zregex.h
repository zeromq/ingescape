/*
 *	ZYREPROBE
 *
 *  Copyright (c) 2016 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Stephane Vales <vales@ingenuity.io>
 *
 *
 */

#ifndef zregex_h
#define zregex_h

#include <stdio.h>
#include <regex.h>

//size of a buffer to contain any error messages
//encountered when the regular expression is compiled
#define MAX_ERROR_MSG 0x1000

//maximum number of matches allowed
#define MAX_MATCHES 30
#define BUFFER_SIZE 1024

/**
 *  Structure used to store the matching result
 */
typedef struct matches {
    int nb;
    char results[MAX_MATCHES][BUFFER_SIZE];
} matches;



/**
 *  transform a string patter to compiled regex
 *
 *  @param r          regex pattern compile
 *  @param regex_text string pattern
 *
 *  @return 0 if Ok, Error otherwise
 */
 int compile_regex (regex_t * r, const char * regex_text);

/**
 *  get the matching result from regex compiled pattern
 *
 *  @param r         regex patern compiled
 *  @param to_match  string to check matching
 *  @param collector collector structure to store matching results
 *
 *  @return 0 if Ok, Error otherwise
 */
 int match_regex (regex_t * r, const char * to_match, matches *collector);

/**
 *  clean the collector matches structure
 *
 *  @param collector collector to clear
 */
void clean_matches(matches *collector);


#endif /* zregex_h */
