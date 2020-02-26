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
            if ((agent->loopElements != NULL)
                && (strcmp(name, agent->loopElements->networkDevice) == 0)){
                // Agent is started
                foundNetworkDevice = true;

                // Check if IP address has changed : this might cause trouble in agent communication
                if (strcmp(agent->loopElements->ipAddress, ziflist_address(iflist)) != 0){
                    igsAgent_warn(agent, "IP address has changed from %s to %s",
                             agent->loopElements->ipAddress, ziflist_address(iflist));

                    // Call our callbacks
                    DL_FOREACH(agent->monitorCallbacks, cb){
                        cb->callback_ptr(agent, IGS_NETWORK_ADDRESS_CHANGED,
                                         agent->loopElements->networkDevice, ziflist_address(iflist), cb->myData);
                    }

                    // check if we need to restart after IP address change
                    if (agent->monitor_shallStartStopAgent){
                        unsigned int port = agent->loopElements->zyrePort;
                        char *networkDevice = strdup(agent->loopElements->networkDevice);
                        igsAgent_warn(agent, "restarting agent after IP address has changed on %s", networkDevice);
                        igsAgent_stop(agent);
                        igsAgent_startWithDevice(agent, networkDevice, port);
                        free(networkDevice);
                    }
                }
                break;
            }
            // Agent is stopped : check if this is our expected network device
            else if ((agent->loopElements == NULL)
                     && (agent->monitor->networkDevice != NULL)
                     && (strcmp(name, agent->monitor->networkDevice) == 0)){
                // Agent is not started BUT network device is available
                foundNetworkDevice = true;
                //Nothing special to do here
                break;
            }
        } else if (agent->monitor->status == IGS_NETWORK_DEVICE_NOT_AVAILABLE) {
            // network device was missing : check if situation has changed
            if ((agent->loopElements == NULL)
                && (agent->monitor->networkDevice != NULL)
                && (strcmp(name, agent->monitor->networkDevice) == 0)){
                foundNetworkDevice = true;
                igsAgent_warn(agent, "network device %s has come back", agent->monitor->networkDevice);

                // Update status
                agent->monitor->status = IGS_NETWORK_OK;

                // Call our callbacks
                DL_FOREACH(agent->monitorCallbacks, cb){
                    cb->callback_ptr(agent, IGS_NETWORK_OK, agent->monitor->networkDevice, NULL, cb->myData);
                }

                // check if we need to restart it
                if (agent->monitor_shallStartStopAgent){
                    igsAgent_warn(agent, "restarting agent after network device %s is back", agent->monitor->networkDevice);
                    igsAgent_stop(agent);
                    igsAgent_startWithDevice(agent, agent->monitor->networkDevice, agent->monitor->port);
                }

                // Clean-up
                if (agent->monitor->networkDevice != NULL)
                    free(agent->monitor->networkDevice);
                agent->monitor->networkDevice = NULL;
                break;
            }
            else if ((agent->loopElements != NULL)
                      && (strcmp(name, agent->loopElements->networkDevice) == 0)){
                // agent is now started : it was restarted manually
                foundNetworkDevice = true;

                // Update our status
                agent->monitor->status = IGS_NETWORK_OK;

                // Call our callbacks
                DL_FOREACH(agent->monitorCallbacks, cb){
                    cb->callback_ptr(agent, IGS_NETWORK_OK_AFTER_MANUAL_RESTART, agent->loopElements->networkDevice, agent->loopElements->ipAddress, cb->myData);
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
        if (agent->loopElements != NULL) {
            //agent is started but network device was not found
            igsAgent_warn(agent, "network device %s has disappeared", agent->loopElements->networkDevice);

            // Update status
            agent->monitor->status = IGS_NETWORK_DEVICE_NOT_AVAILABLE;

            // Save network values before calling our callbacks and igs_stop
            agent->monitor->port = agent->loopElements->zyrePort;
            if (agent->monitor->networkDevice != NULL)
                free(agent->monitor->networkDevice);
            agent->monitor->networkDevice = strdup(agent->loopElements->networkDevice);

            // Call our callbacks
            DL_FOREACH(agent->monitorCallbacks, cb){
                cb->callback_ptr(agent, IGS_NETWORK_DEVICE_NOT_AVAILABLE, agent->loopElements->networkDevice, agent->loopElements->ipAddress, cb->myData);
            }

            // Check if we need to stop it
            if (agent->monitor_shallStartStopAgent){
                // NB: we use monitor->networkDevice instead of agentElements->networkDevice because callbacks may call igs_stop()
                igsAgent_warn(agent, "stopping agent after network device %s has disappeared", agent->monitor->networkDevice);
                igsAgent_stop(agent);
            }

        }else if (agent->monitor->networkDevice != NULL){
            //agent is not started AND we have an expected network device
            igsAgent_warn(agent, "network device %s not found", agent->monitor->networkDevice);

            // Update status
            agent->monitor->status = IGS_NETWORK_DEVICE_NOT_AVAILABLE;

            // Call our callbacks
            DL_FOREACH(agent->monitorCallbacks, cb){
                cb->callback_ptr(agent, IGS_NETWORK_DEVICE_NOT_AVAILABLE, agent->monitor->networkDevice, NULL, cb->myData);
            }

            //NB: we don't need to call igs_stop because our agent is not running
        }
    }
    
    return 0;
}

//manage messages from the parent thread
int monitor_manageParent (zloop_t *loop, zmq_pollitem_t *item, void *arg){
    IGS_UNUSED(loop)
    igsAgent_t *agent = (igsAgent_t *)arg;
    
    if (item->revents & ZMQ_POLLIN)
    {
        zmsg_t *msg = zmsg_recv ((zsock_t *)item->socket);
        if (!msg){
            igsAgent_error(agent, "Could not read message from main thread : Agent will interrupt immediately.");
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
        igsAgent_error(agent, "Could not get the pipe descriptor to the main thread for polling");
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
        igsAgent_warn(agent, "monitor is already started");
        return;
    }
    agent->monitor = (monitor_t *)calloc(1, sizeof(monitor_t));
    agent->monitor->period = period;
    agent->monitor->status = IGS_NETWORK_OK;
    agent->monitor->monitorActor = zactor_new (monitor_initLoop, agent);
    assert(agent->monitor->monitorActor);
    igs_nbOfAgentsInProcess++;
}

void igsAgent_monitoringEnableWithExpectedDevice(igsAgent_t *agent, unsigned int period,
                                                 const char* networkDevice, unsigned int port){
    if (agent->monitor != NULL){
        igsAgent_warn(agent, "monitor is already started");
        return;
    }

    agent->monitor = (monitor_t *)calloc(1, sizeof(monitor_t));
    agent->monitor->period = period;
    agent->monitor->status = IGS_NETWORK_OK;

    if ((networkDevice == NULL) || (strlen(networkDevice) == 0)){
        igsAgent_warn(agent, "networkDevice should not be NULL or empty");
    }else{
        agent->monitor->networkDevice = strdup(networkDevice);
    }
    agent->monitor->port = port;

    agent->monitor->monitorActor = zactor_new (monitor_initLoop, agent);
    assert(agent->monitor->monitorActor);
    igs_nbOfAgentsInProcess++;
}

void igsAgent_monitoringDisable(igsAgent_t *agent){
    if (agent->monitor == NULL){
        igsAgent_warn(agent, "monitor is not started");
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
    igs_nbOfAgentsInProcess--;
    //igsAgent_debug(agent, "still %d agents running in process", igs_nbOfAgentsInProcess);
#if (defined WIN32 || defined _WIN32)
    // On Windows, we need to use a sledgehammer to avoid assertion errors
    // NB: If we don't call zsys_shutdown, the application will crash on exit
    // (WSASTARTUP assertion failure)
    if (igs_nbOfAgentsInProcess == 0) {
        igsAgent_debug(agent, "calling zsys_shutdown after last agent in process has stopped");
        zsys_shutdown();
    }
#endif
}

bool igsAgent_isMonitoringEnabled(igsAgent_t *agent){
    return (agent->monitor != NULL);
}

void igsAgent_monitor(igsAgent_t *agent, igsAgent_monitorCallback cb, void *myData){
    if (cb != NULL){
        monitorCallback_t *newCb = calloc(1, sizeof(monitorCallback_t));
        newCb->callback_ptr = cb;
        newCb->myData = myData;
        DL_APPEND(agent->monitorCallbacks, newCb);
    }else{
        igsAgent_warn(agent, "callback is null");
    }
}

void igsAgent_monitoringShallStartStopAgent(igsAgent_t *agent, bool flag){
    agent->monitor_shallStartStopAgent = flag;
}
