//
//  monitor.c
//  ingescape
//
//  Created by Stephane Vales on 08/05/2019.
//  Copyright © 2019 Ingenuity i/o. All rights reserved.
//

#include <stdio.h>
#include "ingescape_advanced.h"
#include "ingescape_private.h"

//Timer callback to check network 
int monitor_triggerNetworkCheck(zloop_t *loop, int timer_id, void *arg){
    IGS_UNUSED(loop)
    IGS_UNUSED(timer_id)
    igsAgent_t *agent = (igsAgent_t *)arg;

#if (defined WIN32 || defined _WIN32)
    WORD version_requested = MAKEWORD (2, 2);
    WSADATA wsa_data;
    int rc = WSAStartup (version_requested, &wsa_data);
    assert (rc == 0);
    assert (LOBYTE (wsa_data.wVersion) == 2 &&
            HIBYTE (wsa_data.wVersion) == 2);
#endif
    
    bool foundNetworkDevice = false;
    monitorCallback_t *cb = NULL;
    ziflist_t *iflist = ziflist_new ();
    assert (iflist);
    const char *name = ziflist_first (iflist);
    //go through the available devices to check network consistency depending on previous state
    while (name) {
        if (agent->monitor->status == IGS_NETWORK_OK){
            //Agent was previously OK : check if it still is
            if ((agent->agentElements != NULL)
                && (strcmp(name, agent->agentElements->networkDevice) == 0)){
                // Agent is started
                foundNetworkDevice = true;

                // Check if IP address has changed : this might cause trouble in agent communication
                if (strcmp(agent->agentElements->ipAddress, ziflist_address(iflist)) != 0){
                    igs_warn("IP address has changed from %s to %s",
                             agent->agentElements->ipAddress, ziflist_address(iflist));

                    // Call our callbacks
                    DL_FOREACH(agent->monitorCallbacks, cb){
                        cb->callback_ptr(IGS_NETWORK_ADDRESS_CHANGED,
                                         agent->agentElements->networkDevice, ziflist_address(iflist), cb->myData);
                    }

                    // check if we need to restart after IP address change
                    if (agent->monitor_shallStartStopAgent){
                        unsigned int port = agent->agentElements->zyrePort;
                        char *networkDevice = strdup(agent->agentElements->networkDevice);
                        igs_warn("restarting agent after IP address has changed on %s", networkDevice);
                        igs_stop();
                        igs_startWithDevice(networkDevice, port);
                        free(networkDevice);
                    }
                }
                break;
            }
            // Agent is stopped : check if this is our expected network device
            else if ((agent->agentElements == NULL)
                     && (agent->monitor->networkDevice != NULL)
                     && (strcmp(name, agent->monitor->networkDevice) == 0)){
                // Agent is not started BUT network device is available
                foundNetworkDevice = true;
                //Nothing special to do here
                break;
            }
        } else if (agent->monitor->status == IGS_NETWORK_DEVICE_NOT_AVAILABLE) {
            // network device was missing : check if situation has changed
            if ((agent->agentElements == NULL)
                && (agent->monitor->networkDevice != NULL)
                && (strcmp(name, agent->monitor->networkDevice) == 0)){
                foundNetworkDevice = true;
                igs_warn("network device %s has come back", agent->monitor->networkDevice);

                // Update status
                agent->monitor->status = IGS_NETWORK_OK;

                // Call our callbacks
                DL_FOREACH(agent->monitorCallbacks, cb){
                    cb->callback_ptr(IGS_NETWORK_OK, agent->monitor->networkDevice, NULL, cb->myData);
                }

                // check if we need to restart it
                if (agent->monitor_shallStartStopAgent){
                    igs_warn("restarting agent after network device %s is back", agent->monitor->networkDevice);
                    igs_stop();
                    igs_startWithDevice(agent->monitor->networkDevice, agent->monitor->port);
                }

                // Clean-up
                if (agent->monitor->networkDevice != NULL)
                    free(agent->monitor->networkDevice);
                agent->monitor->networkDevice = NULL;
                break;
            }
            else if ((agent->agentElements != NULL)
                      && (strcmp(name, agent->agentElements->networkDevice) == 0)){
                // agent is now started : it was restarted manually
                foundNetworkDevice = true;

                // Update our status
                agent->monitor->status = IGS_NETWORK_OK;

                // Call our callbacks
                DL_FOREACH(agent->monitorCallbacks, cb){
                    cb->callback_ptr(IGS_NETWORK_OK_AFTER_MANUAL_RESTART, agent->agentElements->networkDevice, agent->agentElements->ipAddress, cb->myData);
                }

                //NB: we don't need to (re)start our agent because it is already started

                // Clean-up if needed
                if (agent->monitor->networkDevice != NULL)
                    free(agent->monitor->networkDevice);
                agent->monitor->networkDevice = NULL;
                break;
            }
        }
        name = ziflist_next (iflist);
    }
    ziflist_destroy (&iflist);
    
    //NB: if expected network device has disappeared, we cannot detect an IP address change before
    //detecting device disappearance. This has consequences in the logic used here.
    
    if (!foundNetworkDevice
        && (agent->monitor->status != IGS_NETWORK_DEVICE_NOT_AVAILABLE)){
        // we did not find our expected network device and this is new

        // Check if our agent is started
        if (agent->agentElements != NULL) {
            //agent is started but network device was not found
            igs_warn("network device %s has disappeared", agent->agentElements->networkDevice);

            // Update status
            agent->monitor->status = IGS_NETWORK_DEVICE_NOT_AVAILABLE;

            // Save network values before calling our callbacks and igs_stop
            agent->monitor->port = agent->agentElements->zyrePort;
            if (agent->monitor->networkDevice != NULL)
                free(agent->monitor->networkDevice);
            agent->monitor->networkDevice = strdup(agent->agentElements->networkDevice);

            // Call our callbacks
            DL_FOREACH(agent->monitorCallbacks, cb){
                cb->callback_ptr(IGS_NETWORK_DEVICE_NOT_AVAILABLE, agent->agentElements->networkDevice, agent->agentElements->ipAddress, cb->myData);
            }

            // Check if we need to stop it
            if (agent->monitor_shallStartStopAgent){
                // NB: we use monitor->networkDevice instead of agentElements->networkDevice because callbacks may call igs_stop()
                igs_warn("stopping agent after network device %s has disappeared", agent->monitor->networkDevice);
                igs_stop();
            }

        }else if (agent->monitor->networkDevice != NULL){
            //agent is not started AND we have an expected network device
            igs_warn("network device %s not found", agent->monitor->networkDevice);

            // Update status
            agent->monitor->status = IGS_NETWORK_DEVICE_NOT_AVAILABLE;

            // Call our callbacks
            DL_FOREACH(agent->monitorCallbacks, cb){
                cb->callback_ptr(IGS_NETWORK_DEVICE_NOT_AVAILABLE, agent->monitor->networkDevice, NULL, cb->myData);
            }

            //NB: we don't need to call igs_stop because our agent is not running
        }
    }
    
    return 0;
}

//manage messages from the parent thread
int monitor_manageParent (zloop_t *loop, zmq_pollitem_t *item, void *arg){
    IGS_UNUSED(loop)
    IGS_UNUSED(arg)
    
    if (item->revents & ZMQ_POLLIN)
    {
        zmsg_t *msg = zmsg_recv ((zsock_t *)item->socket);
        if (!msg){
            igs_error("Could not read message from main thread : Agent will interrupt immediately.");
            return -1;
        }
        char *command = zmsg_popstr (msg);
        if (streq (command, "$TERM")){
            free (command);
            zmsg_destroy (&msg);
            return -1;
        } else {
            //nothing to do so far
        }
        
        free (command);
        zmsg_destroy (&msg);
    }
    return 0;
}

static void monitor_initLoop (zsock_t *pipe, void *args){
    igsAgent_t *agent = (igsAgent_t *)args;
    zsock_signal (pipe, 0);
    
    agent->monitor->loop = zloop_new ();
    assert (agent->monitor->loop);
    zloop_set_verbose (agent->monitor->loop, false);
    
    void *zpipe = zsock_resolve(pipe);
    if (zpipe == NULL){
        igs_error("Could not get the pipe descriptor to the main thread for polling");
    }
    zmq_pollitem_t zpipePollItem;
    zpipePollItem.socket = zpipe;
    zpipePollItem.fd = 0;
    zpipePollItem.events = ZMQ_POLLIN;
    zpipePollItem.revents = 0;
    
    zloop_poller (agent->monitor->loop, &zpipePollItem, monitor_manageParent, agent);
    zloop_poller_set_tolerant(agent->monitor->loop, &zpipePollItem);
    zloop_timer(agent->monitor->loop, agent->monitor->period, 0, monitor_triggerNetworkCheck, agent);
    
    zloop_start (agent->monitor->loop);

    zloop_destroy(&agent->monitor->loop);
}

void igsAgent_monitoringEnable(igsAgent_t *agent, unsigned int period){
    if (agent->monitor != NULL){
        igs_warn("monitor is already started");
        return;
    }
    agent->monitor = (monitor_t *)calloc(1, sizeof(monitor_t));
    agent->monitor->period = period;
    agent->monitor->status = IGS_NETWORK_OK;
    agent->monitor->monitorActor = zactor_new (monitor_initLoop, agent);
}

void igsAgent_monitoringEnableWithExpectedDevice(igsAgent_t *agent, unsigned int period,
                                                 const char* networkDevice, unsigned int port){
    if (agent->monitor != NULL){
        igs_warn("monitor is already started");
        return;
    }

    agent->monitor = (monitor_t *)calloc(1, sizeof(monitor_t));
    agent->monitor->period = period;
    agent->monitor->status = IGS_NETWORK_OK;

    if ((networkDevice == NULL) || (strlen(networkDevice) == 0)){
        igs_warn("networkDevice should not be NULL or empty");
    }else{
        agent->monitor->networkDevice = strdup(networkDevice);
    }
    agent->monitor->port = port;

    agent->monitor->monitorActor = zactor_new (monitor_initLoop, agent);
}

void igsAgent_monitoringDisable(igsAgent_t *agent){
    if (agent->monitor == NULL){
        igs_warn("monitor is not started");
        return;
    }
    zstr_sendx (agent->monitor->monitorActor, "$TERM", NULL);
    if (agent->monitor->monitorActor != NULL){
        zactor_destroy (&agent->monitor->monitorActor);
    }
    if (agent->monitor->networkDevice != NULL)
        free(agent->monitor->networkDevice);
    agent->monitor->networkDevice = NULL;
    free(agent->monitor);
    agent->monitor = NULL;

#if (defined WIN32 || defined _WIN32)
    // On Windows, we need to use a sledgehammer to avoid assertion errors
    // NB: If we don't call zsys_shutdown, the application will crash on exit
    // (WSASTARTUP assertion failure)
    if (agent->agentElements == NULL) {
        zsys_shutdown();
    }
#endif
}

bool igsAgent_isMonitoringEnabled(igsAgent_t *agent){
    return (agent->monitor != NULL);
}

void igsAgent_monitor(igsAgent_t *agent, igs_monitorCallback cb, void *myData){
    if (cb != NULL){
        monitorCallback_t *newCb = calloc(1, sizeof(monitorCallback_t));
        newCb->callback_ptr = cb;
        newCb->myData = myData;
        DL_APPEND(agent->monitorCallbacks, newCb);
    }else{
        igs_warn("callback is null");
    }
}

void igsAgent_monitoringShallStartStopAgent(igsAgent_t *agent, bool flag){
    agent->monitor_shallStartStopAgent = flag;
}
