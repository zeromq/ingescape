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
    core_initContext();
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
            //peer was previously OK : check if it still is
            if (coreContext->node && coreContext->networkDevice
                && strcmp(name, coreContext->networkDevice) == 0){
                // peer is started
                foundNetworkDevice = true;

                // check if IP address has changed : this might cause trouble in peer communication
                if (strcmp(coreContext->ipAddress, ziflist_address(iflist)) != 0){
                    igs_warn("IP address has changed from %s to %s",
                             coreContext->ipAddress, ziflist_address(iflist));

                    // call our callbacks
                    DL_FOREACH(coreContext->monitorCallbacks, cb){
                        cb->callback_ptr(IGS_NETWORK_ADDRESS_CHANGED, coreContext->networkDevice,
                                         ziflist_address(iflist), cb->myData);
                    }

                    // check if we need to restart after IP address change
                    if (coreContext->monitor_shallStartStopAgent){
                        unsigned int port = coreContext->network_zyrePort;
                        char *networkDevice = strdup(coreContext->networkDevice);
                        igs_warn("restarting peer after IP address has changed on %s", networkDevice);
                        igs_stop();
                        igs_startWithDevice(networkDevice, port);
                        free(networkDevice);
                    }
                }
                break;
            }
            // peer is stopped : check if this is our expected network device
            else if ((coreContext->node == NULL)
                     && coreContext->monitor->networkDevice
                     && (strcmp(name, coreContext->monitor->networkDevice) == 0)){
                // peer is not started BUT network device is available
                foundNetworkDevice = true;
                // nothing special to do here
                break;
            }
        } else if (coreContext->monitor->status == IGS_NETWORK_DEVICE_NOT_AVAILABLE) {
            // network device was missing : check if situation has changed
            if ((coreContext->node == NULL)
                && coreContext->monitor->networkDevice
                && (strcmp(name, coreContext->monitor->networkDevice) == 0)){
                foundNetworkDevice = true;
                igs_warn("network device %s has come back", coreContext->monitor->networkDevice);

                // update status
                coreContext->monitor->status = IGS_NETWORK_OK;

                // call our callbacks
                DL_FOREACH(coreContext->monitorCallbacks, cb){
                    cb->callback_ptr(IGS_NETWORK_OK, coreContext->monitor->networkDevice, NULL, cb->myData);
                }

                // check if we need to restart peer
                if (coreContext->monitor_shallStartStopAgent){
                    igs_warn("restarting agent after network device %s is back", coreContext->monitor->networkDevice);
                    igs_stop();
                    igs_startWithDevice(coreContext->monitor->networkDevice, coreContext->monitor->port);
                }

                // clean-up
                if (coreContext->monitor->networkDevice)
                    free(coreContext->monitor->networkDevice);
                coreContext->monitor->networkDevice = NULL;
                break;
            }
            else if (coreContext->node
                     && (strcmp(name, coreContext->networkDevice) == 0)){
                // peer is now started : it was restarted manually
                foundNetworkDevice = true;

                // update our status
                coreContext->monitor->status = IGS_NETWORK_OK;

                // Call our callbacks
                DL_FOREACH(coreContext->monitorCallbacks, cb){
                    cb->callback_ptr(IGS_NETWORK_OK_AFTER_MANUAL_RESTART, coreContext->networkDevice, coreContext->ipAddress, cb->myData);
                }

                //NB: we don't need to (re)start our peer because it is already started

                // clean-up
                if (coreContext->monitor->networkDevice)
                    free(coreContext->monitor->networkDevice);
                coreContext->monitor->networkDevice = NULL;
                break;
            }
        }
        name = ziflist_next (iflist);
    }
    ziflist_destroy (&iflist);
    
    //NB: if expected network device has disappeared, we cannot detect an IP address change before
    //detecting device reappearance. This has consequences in the logic used here.
    
    if (!foundNetworkDevice
        && (coreContext->monitor->status != IGS_NETWORK_DEVICE_NOT_AVAILABLE)){
        // we did not find our expected network device and this is new

        // check if our peer is started
        if (coreContext->node){
            //peer is started but network device was not found
            igs_warn("network device %s has disappeared", coreContext->networkDevice);

            // update status
            coreContext->monitor->status = IGS_NETWORK_DEVICE_NOT_AVAILABLE;

            // save network values before calling our callbacks and igs_stop
            coreContext->monitor->port = coreContext->network_zyrePort;
            if (coreContext->monitor->networkDevice)
                free(coreContext->monitor->networkDevice);
            if (coreContext->networkDevice)
                coreContext->monitor->networkDevice = strdup(coreContext->networkDevice);

            // call our callbacks
            DL_FOREACH(coreContext->monitorCallbacks, cb){
                cb->callback_ptr(IGS_NETWORK_DEVICE_NOT_AVAILABLE, coreContext->networkDevice, coreContext->ipAddress, cb->myData);
            }

            // check if we need to stop peer
            if (coreContext->monitor_shallStartStopAgent){
                // NB: we use monitor->networkDevice instead of coreContext->networkDevice because callbacks may call igs_stop()
                igs_warn("stopping agent after network device %s has disappeared", coreContext->monitor->networkDevice);
                igs_stop();
            }

        }else if (coreContext->monitor->networkDevice){
            //peer is not started AND we have an expected network device
            igs_warn("network device %s not found", coreContext->monitor->networkDevice);

            // update status
            coreContext->monitor->status = IGS_NETWORK_DEVICE_NOT_AVAILABLE;

            // call our callbacks
            DL_FOREACH(coreContext->monitorCallbacks, cb){
                cb->callback_ptr(IGS_NETWORK_DEVICE_NOT_AVAILABLE, coreContext->monitor->networkDevice, NULL, cb->myData);
            }
            //NB: we don't need to call igs_stop because our agent is not running
        }
    }
    return 0;
}

//manage messages from the parent thread
int monitor_manageParent (zloop_t *loop, zsock_t *socket, void *arg){
    IGS_UNUSED(loop)
    IGS_UNUSED(arg)
    zmsg_t *msg = zmsg_recv (socket);
    if (!msg){
        igs_error("Could not read message from main thread : Ingescape will interrupt immediately.");
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
    return 0;
}

static void monitor_initLoop (zsock_t *pipe, void *args){
    IGS_UNUSED(args)
    coreContext->monitor->loop = zloop_new ();
    //zloop_set_verbose (coreContext->monitor->loop, false);
    zloop_reader(coreContext->monitor->loop, pipe, monitor_manageParent, NULL);
    zloop_reader_set_tolerant(coreContext->monitor->loop, pipe);
    zloop_timer(coreContext->monitor->loop, coreContext->monitor->period, 0, igs_monitor_triggerNetworkCheck, NULL);
    zsock_signal (pipe, 0);
    zloop_start (coreContext->monitor->loop);
    zloop_destroy(&coreContext->monitor->loop);
}

void igs_monitoringEnable(unsigned int period){
    core_initContext();
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
    assert(networkDevice);
    assert(strlen(networkDevice) > 0);
    core_initContext();
    if (coreContext->monitor != NULL){
        igs_warn("monitor is already started");
        return;
    }
    coreContext->monitor = (igs_monitor_t *)calloc(1, sizeof(igs_monitor_t));
    coreContext->monitor->period = period;
    coreContext->monitor->status = IGS_NETWORK_OK;
    coreContext->monitor->networkDevice = strdup(networkDevice);
    coreContext->monitor->port = port;
    coreContext->monitor->monitorActor = zactor_new (monitor_initLoop, NULL);
    assert(coreContext->monitor->monitorActor);
}

void igs_monitoringDisable(){
    core_initContext();
    if (coreContext->monitor == NULL){
        igs_warn("monitor is not started");
        return;
    }
    zstr_sendx (coreContext->monitor->monitorActor, "$TERM", NULL);
    if (coreContext->monitor->monitorActor){
        zactor_destroy (&coreContext->monitor->monitorActor);
    }
    if (coreContext->monitor->networkDevice)
        free(coreContext->monitor->networkDevice);
    coreContext->monitor->networkDevice = NULL;
    free(coreContext->monitor);
    coreContext->monitor = NULL;
    //igsAgent_debug(agent, "still %d agents running in process", igs_nbOfAgentsInProcess);
#if (defined WIN32 || defined _WIN32)
    // NB: If we don't call zsys_shutdown on windows, the application will crash on exit
    // (WSASTARTUP assertion failure)
    if (coreContext->networkActor == NULL) {
        igs_debug("calling zsys_shutdown");
        zsys_shutdown();
    }
#endif
}

bool igs_isMonitoringEnabled(){
    core_initContext();
    return (coreContext->monitor != NULL);
}

void igs_monitor(igs_monitorCallback cb, void *myData){
    core_initContext();
    assert(cb);
    igs_monitor_callback_t *newCb = calloc(1, sizeof(igs_monitor_callback_t));
    newCb->callback_ptr = cb;
    newCb->myData = myData;
    DL_APPEND(coreContext->monitorCallbacks, newCb);
}

void igs_monitoringShallStartStopAgent(bool flag){
    core_initContext();
    coreContext->monitor_shallStartStopAgent = flag;
}
