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

typedef struct monitor {
    unsigned int period;
    igs_monitorEvent_t status;
    zactor_t *monitorActor;
    zloop_t *loop;
    unsigned int port;
    char *networkDevice;
} monitor_t;
monitor_t *monitor = NULL;

typedef struct monitorCallback {
    igs_monitorCallback callback_ptr;
    void *myData;
    struct monitorCallback *prev;
    struct monitorCallback *next;
} monitorCallback_t;
monitorCallback_t *monitorCallbacks = NULL;

bool monitor_shallStartStopAgent = false;

//Timer callback to check network 
int monitor_triggerNetworkCheck(zloop_t *loop, int timer_id, void *arg){
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
    monitorCallback_t *cb = NULL;
    ziflist_t *iflist = ziflist_new ();
    assert (iflist);
    const char *name = ziflist_first (iflist);
    while (name) {
        if (agentElements != NULL && strcmp(name, agentElements->networkDevice) == 0){
            foundNetworkDevice = true;
            if (strcmp(agentElements->ipAddress, ziflist_address(iflist)) != 0){
                //IP address has changed
                igs_warn("IP address has changed from %s to %s", agentElements->ipAddress, ziflist_address(iflist));
                DL_FOREACH(monitorCallbacks, cb){
                    cb->callback_ptr(IGS_NETWORK_ADDRESS_CHANGED, agentElements->networkDevice, agentElements->ipAddress, cb->myData);
                }
                if (monitor_shallStartStopAgent){
                    unsigned int port = agentElements->zyrePort;
                    char *networkDevice = strdup(agentElements->networkDevice);
                    igs_warn("restarting agent after IP address change on %s", networkDevice);
                    igs_stop();
                    igs_startWithDevice(networkDevice, port);
                    free(networkDevice);
                }
            }
            break;
        } else if (agentElements == NULL){
            //agent is stopped : check if we need to restart it
            if (monitor->status == IGS_NETWORK_DEVICE_NOT_AVAILABLE
                && strcmp(name, monitor->networkDevice) == 0){
                //device was missing and has come back : restart agent
                foundNetworkDevice = true;
                igs_warn("restarting agent after network device %s is back", monitor->networkDevice);
                monitor->status = IGS_NETWORK_OK;
                igs_startWithDevice(monitor->networkDevice, monitor->port);
                free(monitor->networkDevice);
                monitor->networkDevice = NULL;
            }
            break;
        }
        name = ziflist_next (iflist);
    }
    ziflist_destroy (&iflist);
    
    //NB: if nework device has disappeared, we do not risk detecting an IP address change before
    //detecting device disappearance.
    if (agentElements != NULL
        && !foundNetworkDevice
        && monitor->status != IGS_NETWORK_DEVICE_NOT_AVAILABLE){
        //agent is started but network device was not found
        //network device has disappeared
        igs_warn("network device %s has disappeared", agentElements->networkDevice);
        DL_FOREACH(monitorCallbacks, cb){
            cb->callback_ptr(IGS_NETWORK_DEVICE_NOT_AVAILABLE, agentElements->networkDevice, agentElements->ipAddress, cb->myData);
        }
        if (monitor_shallStartStopAgent){
            igs_warn("stopping agent after network device %s has disappeared", agentElements->networkDevice);
            monitor->port = agentElements->zyrePort;
            if (monitor->networkDevice != NULL)
                free(monitor->networkDevice);
            monitor->networkDevice = strdup(agentElements->networkDevice);
            igs_stop();
            monitor->status = IGS_NETWORK_DEVICE_NOT_AVAILABLE;
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
    zsock_signal (pipe, 0);
    
    monitor->loop = zloop_new ();
    assert (monitor->loop);
    zloop_set_verbose (monitor->loop, false);
    
    void *zpipe = zsock_resolve(pipe);
    if (zpipe == NULL){
        igs_error("Could not get the pipe descriptor to the main thread for polling");
    }
    zmq_pollitem_t zpipePollItem;
    zpipePollItem.socket = zpipe;
    zpipePollItem.fd = 0;
    zpipePollItem.events = ZMQ_POLLIN;
    zpipePollItem.revents = 0;
    
    zloop_poller (monitor->loop, &zpipePollItem, monitor_manageParent, NULL);
    zloop_poller_set_tolerant(monitor->loop, &zpipePollItem);
    zloop_timer(monitor->loop, monitor->period, 0, monitor_triggerNetworkCheck, NULL);
    
    zloop_start (monitor->loop);
    
    zloop_destroy(&monitor->loop);
}

void igs_enableMonitoring(unsigned int period){
    if (monitor != NULL){
        igs_warn("monitor is already started");
        return;
    }
    monitor = (monitor_t *)calloc(1, sizeof(monitor_t));
    monitor->period = period;
    monitor->status = IGS_NETWORK_OK;
    monitor->monitorActor = zactor_new (monitor_initLoop, NULL);
}

void igs_disableMonitoring(void){
    if (monitor == NULL){
        igs_warn("monitor is not started");
        return;
    }
    zstr_sendx (monitor->monitorActor, "$TERM", NULL);
    if (monitor->monitorActor != NULL){
        zactor_destroy (&monitor->monitorActor);
    }
    free(monitor);
    monitor = NULL;
}

void igs_monitor(igs_monitorCallback cb, void *myData){
    if (cb != NULL){
        monitorCallback_t *newCb = calloc(1, sizeof(monitorCallback_t));
        newCb->callback_ptr = cb;
        newCb->myData = myData;
        DL_APPEND(monitorCallbacks, newCb);
    }else{
        igs_warn("callback is null");
    }
}

void igs_monitoringShallStartStopAgent(bool flag){
    monitor_shallStartStopAgent = flag;
}
