/*
 *	IngeScape Editor
 *
 *  Copyright (c) 2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Alexandre Lemort    <lemort@ingenuity.io>
 *
 */

#ifndef SYSTEMSIGNALWATCHER_H
#define SYSTEMSIGNALWATCHER_H

#include <QObject>
#include <QSocketNotifier>
#include <QDebug>


#include "I2Quick.h"



//
// Windows specific includes
//
#ifdef Q_OS_WIN
// TODO
#endif


//
// Unix specific includes
//
#ifdef Q_OS_UNIX

#include <signal.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>

#endif




/**
 * @brief The TerminationSignalWatcher class is used to subscribe to signals send by our system (interrupt, termination)
 */
class TerminationSignalWatcher : public QObject
{
    Q_OBJECT


public:
    /**
     * @brief Constructor
     * @param parent
     */
    explicit TerminationSignalWatcher(QObject *parent = 0);


    /**
     * @brief Destructor
     */
    ~TerminationSignalWatcher();


Q_SIGNALS:
    /**
     * @brief Emit when we receive a signal from our system
     */
    void terminationSignal();



/**
 *-------------------
 *
 * Unix specific API
 *
 *-------------------
 */
#ifdef Q_OS_UNIX
public:
    /**
     * @brief sigaction handler
     * @param signal
     */
    static void unixSignalHandler(int signal);


private Q_SLOTS:
    /**
     * @brief Called when our socket notifier is activated
     * @param sockfd
     */
    void _onSocketNotifierActivated(int sockfd);


private:
    // Socket notifier used to notify our mainloop when a signal is received
    // because sigaction is blocking AND it is not safe for Qt
    QSocketNotifier *_socketNotifier;


    // To store the result of socketpair()
    static int _sockpair[2];

    // Default handlers
    static struct sigaction _defaultHandlerSIGINT;
    static struct sigaction _defaultHandlerSIGTERM;
#endif



/**
 *-------------------
 *
 * Windows specific API
 *
 *-------------------
 */
#ifdef Q_OS_WIN
// TODO
#endif

};

#endif // SYSTEMSIGNALWATCHER_H
