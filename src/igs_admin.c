/*  =========================================================================
    admin - administration and logging

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of Ingescape, see https://github.com/zeromq/ingescape.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#include <czmq.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "ingescape_classes.h"
#include "ingescape_private.h"

#define INGESCAPE_PROTOCOL 4
#define NUMBER_OF_LOGS_FOR_FFLUSH 0

#ifndef W_OK
#define W_OK 02
#endif

igs_mutex_t lock;
static bool s_lock_initialized = false;

static const char *log_levels[] = {"TRACE",   "DEBUG", "INFO",
                                   "WARNING", "ERROR", "FATAL"};
static const char *log_colors[] = {"\x1b[94m", "\x1b[36m", "\x1b[32m",
                                   "\x1b[33m", "\x1b[31m", "\x1b[35m",
                                   "\x1b[35m"};

#define LOG_TIME_LENGTH 128
char log_content[IGS_MAX_LOG_LENGTH] = "";
char log_time[LOG_TIME_LENGTH] = "";

// TODO: This method is a utility method and is not specialy linked with the administration. It is used in multiple .c files and may be moved to a more relevant place.
void s_admin_make_file_path (const char *from, char *to, size_t size_of_to)
{
    if (from[0] == '~') {
        from++;
#ifdef _WIN32
        char *home = getenv ("USERPROFILE");
#else
        char *home = getenv ("HOME");
#endif
        if (home == NULL)
            igs_error ("could not find path for home directory");
        else {
            strncpy (to, home, size_of_to);
            strncat (to, from, size_of_to);
        }
    }
    else
        strncpy (to, from, size_of_to);
}

////////////////////////////////////////////////////////////////////////
// PRIVATE API
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
// PUBLIC API
////////////////////////////////////////////////////////////////////////

int igs_version (void)
{
    // igs_debug("ingescape version : %d.%d.%d\n", INGESCAPE_VERSION_MAJOR,
    // INGESCAPE_VERSION_MINOR, INGESCAPE_VERSION_PATCH);
    return INGESCAPE_VERSION;
}

int igs_protocol (void)
{
    // igs_debug("ingescape protocol version : %d\n", INGESCAPE_PROTOCOL);
    return INGESCAPE_PROTOCOL;
}

void admin_log (igsagent_t *agent,
                igs_log_level_t level,
                const char *function,
                const char *fmt,
                ...)
{
    assert (agent);
    assert (function);
    assert (fmt);

    if (!s_lock_initialized) {
        IGS_MUTEX_INIT (lock);
        s_lock_initialized = true;
    }
    IGS_MUTEX_LOCK (lock);
    
    // generate log entries for stream and file
    va_list list;
    va_start (list, fmt);
    size_t full_log_length = vsnprintf (NULL, 0, fmt, list);
    va_end (list);
    size_t full_log_length_offset = 0;
    char *full_log_content_rectified = NULL;
    if (core_context->log_in_file || (core_context->log_in_stream && core_context->logger)) {
        full_log_content_rectified = (char*)zmalloc(core_context->log_file_max_line_length * 2 + 1);
        char *full_log_content = (char*)zmalloc(core_context->log_file_max_line_length + 1);
        va_start (list, fmt);
        vsnprintf (full_log_content, core_context->log_file_max_line_length + 1, fmt, list);
        va_end (list);
        size_t j = 0;
        for (size_t i = 0; i < full_log_length; i++) {
            if (full_log_content[i] == '\n') {
                full_log_content_rectified[j] = '\\';
                full_log_content_rectified[j + 1] = 'n';
                j++;
                full_log_length_offset++;
            } else if (full_log_content[i] == '\0')
                break;
            else
                full_log_content_rectified[j] = full_log_content[i];
            j++;
        }
        full_log_content_rectified[j] = '\0';
        free(full_log_content);
    }

    
    if (core_context->log_in_stream && core_context->logger)
        zstr_sendf (core_context->logger, "%s;%s;%s;%s\n",
                    agent->definition->name, log_levels[level], function,
                    full_log_content_rectified);
    
    if (core_context->log_in_file && level >= core_context->log_file_level) {
        full_log_content_rectified[full_log_length + full_log_length_offset] = '\0';
        if (!core_context->log_file
            && strlen (core_context->log_file_path) == 0) {
            // Current path is empty and log file is not already initiated, create
            // file with default path
            char buff[IGS_MAX_PATH_LENGTH] = "";
            snprintf (core_context->log_file_path, IGS_MAX_PATH_LENGTH,
                      IGS_DEFAULT_LOG_DIR);
            strncpy (buff, core_context->log_file_path, IGS_MAX_PATH_LENGTH);
            s_admin_make_file_path (buff, core_context->log_file_path,
                                    IGS_MAX_PATH_LENGTH);
            if (!zsys_file_exists (core_context->log_file_path)) {
                printf ("creating log dir %s\n", core_context->log_file_path);
                if (zsys_dir_create (core_context->log_file_path) != 0)
                    printf ("error while creating log dir %s\n",
                            core_context->log_file_path);
            }
            strncat (core_context->log_file_path, agent->definition->name,
                     IGS_MAX_PATH_LENGTH);
            strncat (core_context->log_file_path, ".log", IGS_MAX_PATH_LENGTH);
            printf ("using log file %s\n", core_context->log_file_path);
            if (core_context != NULL && core_context->node != NULL) {
                s_lock_zyre_peer ();
                igsagent_t *a, *tmp;
                HASH_ITER (hh, core_context->agents, a, tmp)
                {
                    zmsg_t *msg = zmsg_new ();
                    zmsg_addstr (msg, LOG_FILE_PATH_MSG);
                    zmsg_addstr (msg, core_context->log_file_path);
                    zmsg_addstr (msg, a->uuid);
                    zyre_shout (core_context->node, IGS_PRIVATE_CHANNEL, &msg);
                }
                s_unlock_zyre_peer ();
            }
        }
        if (!core_context->log_file
            || !zsys_file_exists (core_context->log_file_path)) {
            core_context->log_file = fopen (core_context->log_file_path, "a");
            if (!core_context->log_file)
                printf ("error while trying to create/open log file: %s\n",
                        core_context->log_file_path);
        }
        if (core_context->log_file) {
#if defined(__WINDOWS__)
            SYSTEMTIME lt;
            GetLocalTime (&lt);
            snprintf (log_time, LOG_TIME_LENGTH,
                      "%02d/%02d/%d;%02d:%02d:%02d.%06ld", lt.wDay, lt.wMonth,
                      lt.wYear, lt.wHour, lt.wMinute, lt.wSecond,
                      lt.wMilliseconds);
#else
            struct timeval tick;
            gettimeofday (&tick, NULL);
            struct tm *tm = localtime (&tick.tv_sec);
            snprintf (log_time, LOG_TIME_LENGTH,
                      "%02d/%02d/%d;%02d:%02d:%02d.%06d", tm->tm_mday,
                      tm->tm_mon + 1, tm->tm_year + 1900, tm->tm_hour,
                      tm->tm_min, tm->tm_sec, (int) tick.tv_usec);
#endif
            if (fprintf (core_context->log_file, "%s;%s;%s;%s;%s\n",
                         agent->definition->name, log_time, log_levels[level],
                         function, full_log_content_rectified)
                > 0) {
                if (++core_context->log_nb_of_entries
                    > NUMBER_OF_LOGS_FOR_FFLUSH) {
                    core_context->log_nb_of_entries = 0;
                    fflush (core_context->log_file);
                }
            }
            else
                printf ("error while writing logs in %s\n",
                        core_context->log_file_path);
        }
    }
    
    if ((core_context->log_in_console && level >= core_context->log_level)
        || level >= IGS_LOG_WARN) {
        va_start (list, fmt);
        vsnprintf (log_content, IGS_MAX_LOG_LENGTH, fmt, list);
        va_end (list);
        if (level >= IGS_LOG_WARN) {
            if (core_context->use_color_in_console)
                fprintf (stderr, "%s;%s%s\x1b[0m;%s;%s\n",
                         agent->definition->name, log_colors[level],
                         log_levels[level], function, log_content);
            else
                fprintf (stderr, "%s;%s;%s;%s\n", agent->definition->name,
                         log_levels[level], function, log_content);
        }
        else {
            if (core_context->use_color_in_console)
                fprintf (stdout, "%s;%s%s\x1b[0m;%s;%s\n",
                         agent->definition->name, log_colors[level],
                         log_levels[level], function, log_content);
            else
                fprintf (stdout, "%s;%s;%s;%s\n", agent->definition->name,
                         log_levels[level], function, log_content);
        }
    }
    
    if (full_log_content_rectified)
        free (full_log_content_rectified);
    assert (s_lock_initialized);
    IGS_MUTEX_UNLOCK (lock);
}

void igs_log_set_console_level (igs_log_level_t level)
{
    core_init_context ();
    core_context->log_level = level;
}

igs_log_level_t igs_log_console_level ()
{
    core_init_context ();
    return core_context->log_level;
}

void igs_log_set_file (bool allow, const char *path)
{
    core_init_context ();
    if (allow != core_context->log_in_file) {
        core_context->log_in_file = allow;
        if (core_context->network_actor != NULL && core_context->node != NULL) {
            s_lock_zyre_peer ();
            igsagent_t *agent, *tmp;
            HASH_ITER (hh, core_context->agents, agent, tmp)
            {
                zmsg_t *msg = zmsg_new ();
                zmsg_addstr (msg, LOG_IN_FILE_MSG);
                if (allow)
                    zmsg_addstr (msg, "1");
                else
                    zmsg_addstr (msg, "0");
                zmsg_addstr (msg, agent->uuid);
                zyre_shout (core_context->node, IGS_PRIVATE_CHANNEL, &msg);
            }
            s_unlock_zyre_peer ();
        }
    }
    if (path && strlen (path) > 0) {
        char tmp_path[4096] = "";
        s_admin_make_file_path (path, tmp_path, 4095);
        if (!zsys_file_exists (tmp_path)) {
            zfile_t *newF = zfile_new (NULL, tmp_path);
            if (newF)
                zfile_output (newF);
            zfile_destroy (&newF);
        }
        if (access (tmp_path, W_OK) == -1) {
            igs_error ("'%s' is not writable and will not be used", tmp_path);
            return;
        }
        if (streq (core_context->log_file_path, tmp_path)) {
            igs_info ("'%s' is already the log path",
                      core_context->log_file_path);
            return;
        }
        strncpy (core_context->log_file_path, tmp_path, 4096);

        if (core_context->log_file) {
            fflush (core_context->log_file);
            fclose (core_context->log_file);
            core_context->log_file = NULL;
        }
        core_context->log_file = fopen (core_context->log_file_path, "a");
        if (core_context->log_file == NULL)
            igs_error ("could NOT create log file at path %s",
                       core_context->log_file_path);
        else
            igs_info ("switching to new log file: %s",
                      core_context->log_file_path);
        if (core_context->log_file && core_context && core_context->node) {
            s_lock_zyre_peer ();
            igsagent_t *agent, *tmp;
            HASH_ITER (hh, core_context->agents, agent, tmp)
            {
                zmsg_t *msg = zmsg_new ();
                zmsg_addstr (msg, LOG_FILE_PATH_MSG);
                zmsg_addstr (msg, core_context->log_file_path);
                zmsg_addstr (msg, agent->uuid);
                zyre_shout (core_context->node, IGS_PRIVATE_CHANNEL, &msg);
            }
            s_unlock_zyre_peer ();
        }
    }
    else {
        // switch to default log file path : init is done in admin_log()
        if (core_context->log_file) {
            fclose (core_context->log_file);
            core_context->log_file = NULL;
        }
        core_context->log_file_path[0] = '\0';
    }
}

bool igs_log_file ()
{
    core_init_context ();
    return core_context->log_in_file;
}

void igs_log_set_console (bool allow)
{
    core_init_context ();
    core_context->log_in_console = allow;
}

bool igs_log_console ()
{
    core_init_context ();
    return core_context->log_in_console;
}

void igs_log_set_console_color (bool allow)
{
    core_init_context ();
    core_context->use_color_in_console = allow;
}

bool igs_log_console_color ()
{
    core_init_context ();
    return core_context->use_color_in_console;
}

void igs_log_set_stream (bool stream)
{
    core_init_context ();
    if (stream != core_context->log_in_stream) {
        core_context->log_in_stream = stream;
        if (core_context->network_actor && core_context->node) {
            s_lock_zyre_peer ();
            igsagent_t *agent, *tmp;
            HASH_ITER (hh, core_context->agents, agent, tmp)
            {
                zmsg_t *msg = zmsg_new ();
                zmsg_addstr (msg, LOG_IN_STREAM_MSG);
                zmsg_addstr (msg, (stream) ? "1" : "0");
                zmsg_addstr (msg, agent->uuid);
                zyre_shout (core_context->node, IGS_PRIVATE_CHANNEL, &msg);
            }
            s_unlock_zyre_peer ();
        }
    }
}

bool igs_log_stream ()
{
    core_init_context ();
    return core_context->log_in_stream;
}

void igs_log_set_file_path (const char *path)
{
    igs_log_set_file (core_context->log_in_file, path);
}

char *igs_log_file_path ()
{
    core_init_context ();
    return (strlen(core_context->log_file_path)>0) ? strdup (core_context->log_file_path) : NULL;
}

void igs_log_set_file_level (igs_log_level_t level)
{
    core_init_context ();
    core_context->log_file_level = level;
}

void igs_log_set_file_max_line_length (size_t size)
{
    core_init_context ();
    core_context->log_file_max_line_length = size;
}
