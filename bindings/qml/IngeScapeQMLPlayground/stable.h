/*
 *  IngeScape - QML playground
 *
 *  Copyright (c) 2018 Ingenuity i/o. All rights reserved.
 *
 *  See license terms for the rights and conditions
 *  defined by copyright holders.
 *
 *
 *  Contributors:
 *      Alexandre Lemort <lemort@ingenuity.io>
 *
 */


/* Add C includes here */



/* Add C++ includes here */
#if defined __cplusplus

#ifdef _WIN32
  #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
  #endif

  #define NOMINMAX
  #include <windows.h>
  #include <winsock2.h>
#endif



#include <iostream>

#include <QMetaProperty>
#include <QMetaMethod>
#include <QObject>

#include <QDebug>

#include <QApplication>

#include <QFileSystemWatcher>
#include <QtConcurrent>
#include <QFutureWatcher>
#include <QTemporaryFile>
#include <QTextStream>

#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>
#include <QQmlProperty>
#include <QQuickItem>

#endif
