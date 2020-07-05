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
int igs_monitor_triggerNetworkCheck(zloop_t *loop, int timer_id, void *arg){
    core_initCoreAgent();
    IGS_UNUSED(loop)
    IGS_UNUSED(timer_id)
    IGS_UNUSED(arg)

#if (defined WIN32 || defined _WIN32)
    WORD version_requested = MAKEWORD (2, 2);
    WSADATA wsa_data;
    int rc = WSAStartup (version_requested, &wsa_data);
    assert (rc == 0);
    assert (LOBYTE (wsa_data.wVersion) == 2 &&
            HIBYTE (wsa_data.wVersion) == 2);
#endif
    
    bool foundNetworkDevice = false;
    igs_monitor_callback_t *cb = NULL;
    ziflist_t *iflist = ziflist_new ();
    assert (iflist);
    const char *name = ziflist_first (iflist);
    //go through the available devices to check network consistency depending on previous state
    while (name) {
        if (coreContext->monitor->status == IGS_NETWORK_OK){
            //Agent was previously OK : check if it still is
            if (coreContext->node != NULL && strcmp(name, coreContext->networkDevice) == 0){
                // Agent is started
                foundNetworkDevice = true;

                // Check if IP address has changed : this might cause trouble in agent communication
                if (strcmp(coreContext->ipAddress, ziflist_address(iflist)) != 0){
                    igs_warn("IP address has changed from %s to %s",
                             coreContext->ipAddress, ziflist_address(iflist));

                    // Call our callbacks
                    DL_FOREACH(coreContext->monitorCallbacks, cb){
                        cb->callback_ptr(IGS_NETWORK_ADDRESS_CHANGED, coreContext->networkDevice,
                                         ziflist_address(iflist), cb->myData);
                    }

                    // check if we need to restart after IP address change
                    if (coreContext->monitor_shallStartStopAgent){
                        unsigned int port = coreContext->network_zyrePort;
                        char *networkDevice = strdup(coreContext->networkDevice);
                        igs_warn("restarting agent after IP address has changed on %s", networkDevice);
                        igs_stop();
                        igs_startWithDevice(networkDevice, port);
                        free(networkDevice);
                    }
                }
                break;
            }
            // Agent is stopped : check if this is our expected network device
            else if ((coreContext->node == NULL)
                     && (coreContext->monitor->networkDevice != NULL)
                     && (strcmp(name, coreContext->monitor->networkDevice) == 0)){
                // Agent is not started BUT network device is available
                foundNetworkDevice = true;
                //Nothing special to do here
                break;
            }
        } else if (coreContext->monitor->status == IGS_NETWORK_DEVICE_NOT_AVAILABLE) {
            // network device was missing : check if situation has changed
            if ((coreContext->node == NULL)
                && (coreContext->monitor->networkDevice != NULL)
                && (strcmp(name, coreContext->monitor->networkDevice) == 0)){
                foundNetworkDevice = true;
                igs_warn("network device %s has come back", coreContext->monitor->networkDevice);

                // Update status
                coreContext->monitor->status = IGS_NETWORK_OK;

                // Call our callbacks
                DL_FOREACH(coreContext->monitorCallbacks, cb){
                    cb->callback_ptr(IGS_NETWORK_OK, coreContext->monitor->networkDevice, NULL, cb->myData);
                }

                // check if we need to restart it
                if (coreContext->monitor_shallStartStopAgent){
                    igs_warn("restarting agent after network device %s is back", coreContext->monitor->networkDevice);
                    igs_stop();
                    igs_startWithDevice(coreContext->monitor->networkDevice, coreContext->monitor->port);
                }

                // Clean-up
                if (coreContext->monitor->networkDevice != NULL){
                    free(coreContext->monitor->networkDevice);
                    coreContext->monitor->networkDevice = NULL;
                }
                break;
            }
            else if ((coreContext->node != NULL)
                      && (strcmp(name, coreContext->networkDevice) == 0)){
                // agent is now started : it was restarted manually
                foundNetworkDevice = true;

                // Update our status
                coreContext->monitor->status = IGS_NETWORK_OK;

                // Call our callbacks
                DL_FOREACH(coreContext->monitorCallbacks, cb){
                    cb->callback_ptr(IGS_NETWORK_OK_AFTER_MANUAL_RESTART, coreContext->networkDevice, coreContext->ipAddress, cb->myData);
                }

                //NB: we don't need to (re)start our agent because it is already started

                // Clean-up if needed
                if (coreContext->monitor->networkDevice != NULL)
                    free(coreContext->monitor->networkDevice);
                coreContext->monitor->networkDevice = NULL;
                break;
            }
        }
        name = ziflist_next (iflist);
    }
    ziflist_destroy (&iflist);
    
    //NB: if expected network device has disappeared, we cannot detect an IP address change before
    //detecting device disappearance. This has consequences in the logic used here.
    
    if (!foundNetworkDevice
        && (coreContext->monitor->status != IGS_NETWORK_DEVICE_NOT_AVAILABLE)){
        // we did not find our expected network device and this is new

        // Check if our agent is started
        if (coreContext->node != NULL) {
            //agent is started but network device was not found
            igs_warn("network device %s has disappeared", coreContext->networkDevice);

            // Update status
            coreContext->monitor->status = IGS_NETWORK_DEVICE_NOT_AVAILABLE;

            // Save network values before calling our callbacks and igs_stop
            coreContext->monitor->port = coreContext->network_zyrePort;
            if (coreContext->monitor->networkDevice != NULL)
                free(coreContext->monitor->networkDevice);
            coreContext->monitor->networkDevice = strdup(coreContext->networkDevice);

            // Call our callbacks
            DL_FOREACH(coreContext->monitorCallbacks, cb){
                cb->callback_ptr(IGS_NETWORK_DEVICE_NOT_AVAILABLE, coreContext->networkDevice, coreContext->ipAddress, cb->myData);
            }

            // Check if we need to stop it
            if (coreContext->monitor_shallStartStopAgent){
                // NB: we use monitor->networkDevice instead of coreContext->networkDevice because callbacks may call igs_stop()
                igs_warn("stopping agent after network device %s has disappeared", coreContext->monitor->networkDevice);
                igs_stop();
            }

        }else if (coreContext->monitor->networkDevice != NULL){
            //agent is not started AND we have an expected network device
            igs_warn("network device %s not found", coreContext->monitor->networkDevice);

            // Update status
            coreContext->monitor->status = IGS_NETWORK_DEVICE_NOT_AVAILABLE;

            // Call our callbacks
            DL_FOREACH(coreContext->monitorCallbacks, cb){
                cb->callback_ptr(IGS_NETWORK_DEVICE_NOT_AVAILABLE, coreContext->monitor->networkDevice, NULL, cb->myData);
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
    IGS_UNUSED(args)
    
    coreContext->monitor->loop = zloop_new ();
    assert (coreContext->monitor->loop);
    zloop_set_verbose (coreContext->monitor->loop, false);
    
    void *zpipe = zsock_resolve(pipe);
    if (zpipe == NULL){
        igs_error("Could not get the pipe descriptor to the main thread for polling");
    }
    zmq_pollitem_t zpipePollItem;
    zpipePollItem.socket = zpipe;
    zpipePollItem.fd = 0;
    zpipePollItem.events = ZMQ_POLLIN;
    zpipePollItem.revents = 0;
    
    zloop_poller (coreContext->monitor->loop, &zpipePollItem, monitor_manageParent, NULL);
    zloop_poller_set_tolerant(coreContext->monitor->loop, &zpipePollItem);
    zloop_timer(coreContext->monitor->loop, coreContext->monitor->period, 0, igs_monitor_triggerNetworkCheck, NULL);
    zsock_signal (pipe, 0);
    
    zloop_start (coreContext->monitor->loop);

    zloop_destroy(&coreContext->monitor->loop);
}

void igs_monitoringEnable(unsigned int period){
    if (coreContext->monitor != NULL){
        igs_warn("monitor is already started");
        return;
    }
    coreContext->monitor = (igs_monitor_t *)calloc(1, sizeof(igs_monitor_t));
    coreContext->monitor->period = period;
    coreContext->monitor->status = IGS_NETWORK_OK;
    coreContext->monitor->monitorActor = zactor_new (monitor_initLoop, NULL);
    assert(coreContext->monitor->monitorActor);
}

void igs_monitoringEnableWithExpectedDevice(unsigned int period,
                                            const char* networkDevice, unsigned int port){
    if (coreContext->monitor != NULL){
        igs_warn("monitor is already started");
        return;
    }

    coreContext->monitor = (igs_monitor_t *)calloc(1, sizeof(igs_monitor_t));
    coreContext->monitor->period = period;
    coreContext->monitor->status = IGS_NETWORK_OK;

    if ((networkDevice == NULL) || (strlen(networkDevice) == 0)){
        igs_warn("networkDevice should not be NULL or empty");
    }else{
        coreContext->monitor->networkDevice = strdup(networkDevice);
    }
    coreContext->monitor->port = port;

    coreContext->monitor->monitorActor = zactor_new (monitor_initLoop, NULL);
    assert(coreContext->monitor->monitorActor);
}

void igs_monitoringDisable(){
    if (coreContext->monitor == NULL){
        igs_warn("monitor is not started");
        return;
    }
    zstr_sendx (coreContext->monitor->monitorActor, "$TERM", NULL);
    if (coreContext->monitor->monitorActor != NULL){
        zactor_destroy (&coreContext->monitor->monitorActor);
    }
    if (coreContext->monitor->networkDevice != NULL)
        free(coreContext->monitor->networkDevice);
    coreContext->monitor->networkDevice = NULL;
    free(coreContext->monitor);
    coreContext->monitor = NULL;
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

bool igs_isMonitoringEnabled(){
    return (coreContext->monitor != NULL);
}

void igs_monitor(igs_monitorCallback cb, void *myData){
    if (cb != NULL){
        igs_monitor_callback_t *newCb = calloc(1, sizeof(igs_monitor_callback_t));
        newCb->callback_ptr = cb;
        newCb->myData = myData;
        DL_APPEND(coreContext->monitorCallbacks, newCb);
    }else{
        igs_warn("callback is null");
    }
}

void igs_monitoringShallStartStopAgent(bool flag){
    coreContext->monitor_shallStartStopAgent = flag;
}
