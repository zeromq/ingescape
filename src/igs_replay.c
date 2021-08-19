/*  =========================================================================
    replay - replay from logs

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of Ingescape, see https://github.com/zeromq/ingescape.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#if defined(__WINDOWS__)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <winsock2.h>
#endif

#include "ingescape_classes.h"
#include "ingescape_private.h"
#include <czmq.h>
#include <stdio.h>
#include <stdlib.h>

// general variables
zactor_t *s_replay_actor = NULL;
zloop_t *s_replay_loop = NULL;
zfile_t *s_replay_file = NULL;
bool s_replay_can_start = false;
size_t s_replay_speed = 0;
uint s_replay_mode = 0;
char s_replay_start_time[9] = "";
bool s_replay_is_paused = false;
bool s_replay_shall_stop = false;
bool s_replay_is_beyond_start_time = true;
time_t s_replay_requested_start_time = 0;
const char *s_replay_current_line = NULL;
char s_replay_agent[IGS_MAX_AGENT_NAME_LENGTH] = "";
time_t s_replay_start = 0;
time_t s_replay_end = 0;
long s_replay_nb_lines = 0;

// current action to execute
char s_current_agent[IGS_MAX_AGENT_NAME_LENGTH] = "";
int s_current_microsec = 0;
char s_current_action_types[16] = "";
igs_replay_mode_t s_current_action_type = 0;
char s_current_iop_name[IGS_MAX_IOP_NAME_LENGTH] = "";
char s_current_data_types[64] = "";
igs_iop_value_type_t s_current_data_type = IGS_UNKNOWN_T;
char s_current_iop_data[8192] = "";
struct tm s_current_time = {0};
time_t s_current_unix_time = 0;
time_t s_current_unix_msec_time = 0;
time_t s_previous_unix_msec_time = 0;

long long s_execute_current_and_find_next_action (void)
{
    // execute current action
    if (s_current_action_type
        && (streq (s_replay_agent, "")
            || streq (s_current_agent, s_replay_agent))) {
        if (s_current_data_type == IGS_DATA_T && s_current_iop_data[0] == '|') {
            // ignore this data entry because it is a size and not actual binary data
        } else {
            igsagent_t *agent, *tmp;
            HASH_ITER (hh, core_context->agents, agent, tmp)
            {
                if (streq (agent->definition->name, s_current_agent)) {
                    switch (s_current_action_type) {
                        case IGS_REPLAY_INPUT:
                            if (s_replay_mode & IGS_REPLAY_INPUT) {
                                igs_info ("replaying %s.%s.%s = (%s) %s",
                                          agent->definition->name, "input",
                                          s_current_iop_name,
                                          s_current_data_types,
                                          s_current_iop_data);
                                igsagent_input_set_string (agent,
                                                            s_current_iop_name,
                                                            s_current_iop_data);
                            }
                            break;
                        case IGS_REPLAY_OUTPUT:
                            if (s_replay_mode & IGS_REPLAY_OUTPUT) {
                                igs_info ("replaying %s.%s.%s = (%s) %s",
                                          agent->definition->name, "output",
                                          s_current_iop_name,
                                          s_current_data_types,
                                          s_current_iop_data);
                                igsagent_output_set_string (
                                  agent, s_current_iop_name,
                                  s_current_iop_data);
                            }
                            break;
                        case IGS_REPLAY_PARAMETER:
                            if (s_replay_mode & IGS_REPLAY_PARAMETER) {
                                igs_info ("replaying %s.%s.%s = (%s) %s",
                                          agent->definition->name, "parameter",
                                          s_current_iop_name,
                                          s_current_data_types,
                                          s_current_iop_data);
                                igsagent_parameter_set_string (
                                  agent, s_current_iop_name,
                                  s_current_iop_data);
                            }
                            break;
                        case IGS_REPLAY_CALL_SERVICE:
                            if (s_replay_mode & IGS_REPLAY_CALL_SERVICE) {
                                // TODO:
                            }
                            break;
                        case IGS_REPLAY_EXECUTE_SERVICE:
                            if (s_replay_mode & IGS_REPLAY_EXECUTE_SERVICE) {
                                // TODO:
                            }
                            break;

                        default: {
                            break;
                        }
                    }
                }
            }
        }
    }
    s_current_action_type = 0;

    // find next action
    s_replay_current_line = zfile_readln (s_replay_file);
    s_replay_nb_lines++;
    while (s_replay_current_line) {
        bool found_data = false;
        // Try to find an IOP or service log entry
        int res = sscanf (
          (char *) s_replay_current_line,
          "%1023[^;];%d/%d/%d;%d:%d:%d.%d;DEBUG;model_writeIOP;set "
          "%15s %1023s to %63s %8191[^\0]",
          s_current_agent, &s_current_time.tm_mday, &s_current_time.tm_mon,
          &s_current_time.tm_year, &s_current_time.tm_hour,
          &s_current_time.tm_min, &s_current_time.tm_sec, &s_current_microsec,
          s_current_action_types, s_current_iop_name, s_current_data_types,
          s_current_iop_data);
        if (res == 12) {
            // printf("line: %s\n", s_replay_current_line);
            if (streq (s_current_action_types, "input")) {
                s_current_action_type = IGS_REPLAY_INPUT;
            } else if (streq (s_current_action_types, "output")) {
                s_current_action_type = IGS_REPLAY_OUTPUT;
            } else if (streq (s_current_action_types, "parameter")) {
                s_current_action_type = IGS_REPLAY_PARAMETER;
            } else {
                s_current_action_type = 0;
            }

            if (streq (s_current_data_types, "impulsion")) {
                s_current_data_type = IGS_IMPULSION_T;
            } else if (streq (s_current_data_types, "bool")) {
                s_current_data_type = IGS_BOOL_T;
            } else if (streq (s_current_data_types, "int")) {
                s_current_data_type = IGS_INTEGER_T;
            } else if (streq (s_current_data_types, "double")) {
                s_current_data_type = IGS_DOUBLE_T;
            } else if (streq (s_current_data_types, "string")) {
                s_current_data_type = IGS_STRING_T;
            } else if (streq (s_current_data_types, "data")) {
                s_current_data_type = IGS_DATA_T;
            } else {
                s_current_data_type = IGS_UNKNOWN_T;
            }

            found_data = true;
        } else {
            // TODO: handle service logs
        }

        if (found_data) {
            // compute current time
            if (s_replay_speed > 0
                || (s_replay_requested_start_time == 0
                    && strlen (s_replay_start_time) > 0)) {
                s_current_time.tm_year -= 1900;
                s_current_time.tm_mon -= 1;
                s_current_time.tm_isdst = 0;
                s_current_unix_time = timegm (&s_current_time);
                s_current_unix_msec_time = s_current_unix_time * 1000
                                           + (int) (s_current_microsec / 1000);
            }

            // compute start time if needed
            if (s_replay_requested_start_time == 0
                && strlen (s_replay_start_time) > 0) {
                // we need to init day, month and year for our
                // s_replay_requested_start_time based on the replay file.
                struct tm requested_start_time = {0};
                requested_start_time.tm_year = s_current_time.tm_year;
                requested_start_time.tm_mon = s_current_time.tm_mon;
                requested_start_time.tm_mday = s_current_time.tm_mday;
                requested_start_time.tm_isdst = 0;
                int nb = sscanf (s_replay_start_time, "%d:%d:%d",
                                 &requested_start_time.tm_hour,
                                 &requested_start_time.tm_min,
                                 &requested_start_time.tm_sec);
                if (nb == 3) { // we have a valid start time : we activate flag
                    s_replay_is_beyond_start_time = false;
                    s_replay_requested_start_time =
                      timegm (&requested_start_time);
                } else {
                    igs_error ("invalid start time : '%s'",
                               s_replay_start_time);
                }
            }

            // Compute return value depending on situation
            if (s_replay_is_beyond_start_time && s_replay_speed > 0
                && s_previous_unix_msec_time != 0) {
                // replay is active with non-null speed, start_time has not been reached
                long long delta = (long long) ((s_current_unix_msec_time
                                                - s_previous_unix_msec_time)
                                               / s_replay_speed);
                s_previous_unix_msec_time = s_current_unix_msec_time;
                return delta;
            } else if (!s_replay_is_beyond_start_time
                       && s_current_unix_time
                            >= s_replay_requested_start_time) {
                // we have reached requested start time
                igs_info ("reached start time : %s", s_replay_start_time);
                s_replay_is_beyond_start_time = true;
                // full-throttle replay (certainly until next entry because we just
                // reached requested start time)
                s_previous_unix_msec_time = s_current_unix_msec_time;
                return 0;
            } else {
                // full-throttle replay
                s_previous_unix_msec_time = s_current_unix_msec_time;
                return 0;
            }
        } else {
            // no usefull data found : continue reading file
            s_replay_current_line = zfile_readln (s_replay_file);
            s_replay_nb_lines++;
        }
    }
    // reached end of file
    return -1;
}

// main function for parsing and using the log files
int s_replay_run_through_log_file (zloop_t *loop, int timer_id, void *arg)
{
    IGS_UNUSED (timer_id);
    IGS_UNUSED (arg);

    if (!s_replay_start)
        s_replay_start = zclock_mono ();
    if (s_replay_shall_stop)
        return -1;
    if (s_replay_is_paused) {
        zloop_timer (loop, 250, 1, s_replay_run_through_log_file, NULL);
    }

    long long time_to_wait = s_execute_current_and_find_next_action ();
    while (time_to_wait == 0 && !s_replay_shall_stop && !s_replay_is_paused) {
        // if time_to_wait is zero, we continue as fast as we can
        time_to_wait = s_execute_current_and_find_next_action ();
    }
    if (!s_replay_shall_stop && !s_replay_is_paused) {
        if (time_to_wait == -1)
            return -1; // stop
        if (time_to_wait > 250) {
            // use timer
            zloop_timer (loop, time_to_wait, 1, s_replay_run_through_log_file,
                         NULL);
        } else { // value is between 0 and 250
            // use sleep
            zclock_sleep ((int) time_to_wait);
            s_replay_run_through_log_file (loop, timer_id, arg);
        }
    }
    return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// SERVICES

void igs_replay_initcb (const char *sender_agent_name,
                        const char *sender_agentuuid,
                        const char *service_name,
                        igs_service_arg_t *first_argument,
                        size_t nb_args,
                        const char *token,
                        void *my_data)
{
    IGS_UNUSED (sender_agent_name);
    IGS_UNUSED (sender_agentuuid);
    IGS_UNUSED (service_name);
    IGS_UNUSED (nb_args);
    IGS_UNUSED (token);
    IGS_UNUSED (my_data);
    igs_replay_terminate ();
    char *log_file_path = first_argument->c;
    size_t speed = first_argument->next->i;
    char *start_time = first_argument->next->next->c;
    bool wait_for_start = first_argument->next->next->next->b;
    uint replay_mode = first_argument->next->next->next->next->i;
    char *agent = first_argument->next->next->next->next->next->c;
    igs_replay_init (log_file_path, speed, start_time, wait_for_start,
                     replay_mode, agent);
}

void igs_replay_startcb (const char *sender_agent_name,
                         const char *sender_agentuuid,
                         const char *service_name,
                         igs_service_arg_t *first_argument,
                         size_t nb_args,
                         const char *token,
                         void *my_data)
{
    IGS_UNUSED (sender_agent_name);
    IGS_UNUSED (sender_agentuuid);
    IGS_UNUSED (service_name);
    IGS_UNUSED (first_argument);
    IGS_UNUSED (nb_args);
    IGS_UNUSED (token);
    IGS_UNUSED (my_data);
    igs_replay_start ();
}

void igs_replay_pausecb (const char *sender_agent_name,
                         const char *sender_agentuuid,
                         const char *service_name,
                         igs_service_arg_t *first_argument,
                         size_t nb_args,
                         const char *token,
                         void *my_data)
{
    IGS_UNUSED (sender_agent_name);
    IGS_UNUSED (sender_agentuuid);
    IGS_UNUSED (service_name);
    IGS_UNUSED (nb_args);
    IGS_UNUSED (token);
    IGS_UNUSED (my_data);
    bool pause = first_argument->b;
    igs_replay_pause (pause);
}

void igs_replay_terminatecb (const char *sender_agent_name,
                             const char *sender_agentuuid,
                             const char *service_name,
                             igs_service_arg_t *first_argument,
                             size_t nb_args,
                             const char *token,
                             void *my_data)
{
    IGS_UNUSED (sender_agent_name);
    IGS_UNUSED (sender_agentuuid);
    IGS_UNUSED (service_name);
    IGS_UNUSED (first_argument);
    IGS_UNUSED (nb_args);
    IGS_UNUSED (token);
    IGS_UNUSED (my_data);
    igs_replay_terminate ();
}

//////////////////////////////////////////////////////////////////////////////////
// REPLAY THREAD CONTROL AND LOOP

// messages from other threads to replay thread
int pipe_read_from_other_threads (zloop_t *loop, zsock_t *socket, void *arg)
{
    IGS_UNUSED (loop);
    IGS_UNUSED (arg);
    char *msg = zstr_recv (socket);
    if (streq (msg, "START_REPLAY") && !s_replay_can_start
        && !s_replay_shall_stop) {
        s_replay_can_start = true;
        zloop_timer (s_replay_loop, 0, 1, s_replay_run_through_log_file, NULL);
    } else if (streq (msg, "STOP_REPLAY")) {
        free (msg);
        return -1;
    }
    free (msg);
    return 0;
}

// loop for replay trhead
void replay_run_loop (zsock_t *pipe, void *args)
{
    IGS_UNUSED (args);
    s_replay_loop = zloop_new ();
    if (s_replay_can_start) {
        zloop_timer (s_replay_loop, 1500, 1, s_replay_run_through_log_file,
                     NULL); // 1500 ms gives time for network init
    }
    zloop_reader (s_replay_loop, pipe, pipe_read_from_other_threads, NULL);
    zloop_reader_set_tolerant (s_replay_loop, pipe);
    zsock_signal (pipe, 0);

    zloop_start (s_replay_loop);

    s_replay_end = zclock_mono ();
    if (s_replay_speed == 0) {
        igs_info ("full throttle replay achieved in %ld milliseconds (%ld "
                  "lines parsed)",
                  s_replay_end - s_replay_start, s_replay_nb_lines);
        printf ("full throttle replay achieved in %ld milliseconds (%ld lines "
                "parsed)\n",
                s_replay_end - s_replay_start, s_replay_nb_lines);
    }
    s_replay_start = 0;
    zloop_destroy (&s_replay_loop);
}

//////////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS

void igs_replay_init (const char *log_file_path,
                      size_t speed,
                      const char *start_time,
                      bool wait_for_start,
                      uint replay_mode,
                      const char *agent)
{
    assert (log_file_path || wait_for_start);
    assert (speed >= 0);
    assert (!start_time || strlen (start_time) < 9);
    igs_replay_terminate ();
    char log_file[IGS_MAX_PATH_LENGTH] = "";
    if (log_file_path) {
        s_admin_make_file_path (log_file_path, log_file, IGS_MAX_PATH_LENGTH);
        if (!zsys_file_exists (log_file)) {
            igs_error ("file %s does not exist", log_file_path);
        }
        if (s_replay_file) {
            igs_error ("replay already active with file %s",
                       zfile_filename (s_replay_file, NULL));
            return;
        }
        s_replay_file = zfile_new (NULL, log_file);
        if (!s_replay_file || zfile_input (s_replay_file)) {
            igs_error ("could not read %s",
                       zfile_filename (s_replay_file, NULL));
            zfile_destroy (&s_replay_file);
            return;
        }
    }
    s_replay_speed = speed;

    if (start_time)
        strncpy (s_replay_start_time, start_time, 8);
    else
        strcpy (s_replay_start_time, "");

    if (agent)
        strncpy (s_replay_agent, agent, IGS_MAX_AGENT_NAME_LENGTH);
    else
        strcpy (s_replay_agent, "");

    if (!replay_mode)
        replay_mode = IGS_REPLAY_INPUT + IGS_REPLAY_OUTPUT
                      + IGS_REPLAY_PARAMETER + IGS_REPLAY_EXECUTE_SERVICE
                      + IGS_REPLAY_CALL_SERVICE;
    else
        replay_mode = s_replay_mode;

    s_replay_can_start = !wait_for_start;
    s_replay_start = s_replay_end = 0;
    s_replay_is_paused = false;
    s_replay_nb_lines = 0;
    s_replay_shall_stop = false;
    s_replay_is_beyond_start_time = true;

    if (wait_for_start && !igs_service_exists ("igs_replay_init")) {
        igs_service_init ("igs_replay_init", igs_replay_initcb, NULL);
        igs_service_arg_add ("igs_replay_init", "log_file_path", IGS_STRING_T);
        igs_service_arg_add ("igs_replay_init", "speed", IGS_DOUBLE_T);
        igs_service_arg_add ("igs_replay_init", "start_time", IGS_STRING_T);
        igs_service_arg_add ("igs_replay_init", "wait_for_start", IGS_BOOL_T);
        igs_service_arg_add ("igs_replay_init", "replay_mode", IGS_INTEGER_T);
        igs_service_arg_add ("igs_replay_init", "agent", IGS_STRING_T);

        igs_service_init ("igs_replay_start", igs_replay_startcb, NULL);

        igs_service_init ("igs_replay_pause", igs_replay_pausecb, NULL);
        igs_service_arg_add ("igs_replay_pause", "pause", IGS_BOOL_T);

        igs_service_init ("igs_replay_terminate", igs_replay_terminatecb, NULL);
    }
    s_replay_actor = zactor_new (replay_run_loop, NULL);
}

void igs_replay_start (void)
{
    if (s_replay_actor)
        zstr_send (zactor_sock (s_replay_actor), "START_REPLAY");
    else
        igs_error ("init replay before starting it");
}

void igs_replay_pause (bool pause)
{
    s_replay_is_paused = pause;
}

void igs_replay_terminate (void)
{
    if (s_replay_actor) {
        s_replay_shall_stop = true;
        zstr_send (zactor_sock (s_replay_actor), "STOP_REPLAY");
        zactor_destroy (&s_replay_actor);
    }
    if (s_replay_file)
        zfile_destroy (&s_replay_file);

    if (igs_service_exists ("igs_replay_init")) {
        igs_service_remove ("igs_replay_init");
        igs_service_remove ("igs_replay_start");
        igs_service_remove ("igs_replay_pause");
        igs_service_remove ("igs_replay_terminate");
    }
}
