//
//  regexp.c
//  firstFullAgent
//
//  Created by Stephane Vales on 09/05/2018.
//  Copyright © 2018 IngeScape. All rights reserved.
//

#include "regexp.h"
#include <string.h>

int compile_regex (regex_t * r, const char * regex_text){
    int status = regcomp (r, regex_text, REG_EXTENDED|REG_NEWLINE);
    if (status != 0){
        char error_message[MAX_ERROR_MSG];
        regerror (status, r, error_message, MAX_ERROR_MSG);
        printf ("Regex error compiling '%s': %s\n",regex_text, error_message);
        return 1;
    }
    return 0;
}

int match_regex (regex_t * r, const char * to_match, matches_t *collector){
    //init collector
    collector->nb = 0;
    
    //pointer into the string which points to the end of the previous match
    const char * p = to_match;
    //maximum number of matches allowed
    const int n_matches = MAX_MATCHES;
    //the matches found
    regmatch_t m[MAX_MATCHES];
    
    while (1){
        int i = 0;
        int nomatch = regexec (r, p, n_matches, m, 0);
        if (nomatch) {
            //printf ("No more matches.\n");
            return nomatch;
        }
        for (i = 0; i < n_matches; i++) {
            long long start;
            long long finish;
            if (m[i].rm_so == -1) {
                break;
            }
            start = m[i].rm_so + (p - to_match);
            finish = m[i].rm_eo + (p - to_match);
            if (i == 0) {
                //first match: gives global expression
                //this is the only call if not pattern in the regex
                //printf ("$& is ");
            }
            else
            {
                //detail of the  matches, 1 call per match
                strncpy(collector->results[collector->nb++], to_match + start, finish - start);
                //printf ("$%d is ", i);
            }
            //printf ("'%.*s' (bytes %d:%d)\n", (finish - start),to_match + start, start, finish);
        }
        p += m[0].rm_eo;
    }
    return 0;
}

void clean_matches(matches_t *collector)
{
    for (int i = 0; i < MAX_MATCHES; i++){
        for (int j=0; j < BUFFER_SIZE; j++){
            collector->results[i][j] = '\0';
        }
    }
    
}
