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
#include <QGuiApplication>

#include <QAbstractListModel>
#include <QSortFilterProxyModel>

#include <QStyleHints>

#include <QPainter>
#include <QPainterPath>
#include <QPainterPathStroker>

#include <QVector>
#include <QBasicTimer>

#include <QSvgRenderer>

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
#include <QQuickPaintedItem>
#include <QQuickImageProvider>
#include <QQmlExtensionPlugin>

#include <QQuickWindow>
#include <QScreen>

#include <QSGNode>
#include <QSGGeometry>
#include <QSGGeometryNode>
#include <QSGFlatColorMaterial>
#include <QSGOpacityNode>


#endif
