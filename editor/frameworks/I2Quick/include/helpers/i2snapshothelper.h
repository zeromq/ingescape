/*
 *	I2Quick
 *
 *  Copyright (c) 2016-2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *		Alexandre Lemort <lemort@ingenuity.io>
 *
 */

#ifndef _I2_SNAPSHOTHELPER_H_
#define _I2_SNAPSHOTHELPER_H_

#include <QObject>

#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>

#include <QQuickItem>
#include <QQuickItemGrabResult>

#include "i2quick_global.h"
#include "I2PropertyHelpers.h"


/**
 * @brief The I2SnapshotHelper class is used to save snapshots of our application
 */
class I2QUICK_EXPORT I2SnapshotHelper : public QObject
{
    Q_OBJECT

    // Directory used to save snapshots
    I2_QML_PROPERTY_CUSTOM_SETTER(QString, directory)

public:
    /**
     * @brief Default constructor
     * @param parent
     */
    explicit I2SnapshotHelper(QObject *parent = nullptr);


    /**
     * @brief Method used to provide a singleton to QML
     * @param engine
     * @param scriptEngine
     * @return
     */
     static QObject* qmlSingleton(QQmlEngine* engine, QJSEngine* scriptEngine);


Q_SIGNALS:
     /**
     * @brief SaveItem has succeeded
     */
     void saveItemSucceeded();


     /**
     * @brief SaveItem has failed
     */
     void saveItemFailed();


public Q_SLOTS:
     /**
      * @brief Take a snapshot of a given item and save it
      *
      * @param item
      * @param snapshotSize
      * @param fileNamePrefix
      * @param dateTimeStampSuffix
      */
     void saveItem(QQuickItem* item, QSize snapshotSize = QSize(),  QString fileNamePrefix = "snapshot", QString dateTimeStampSuffix = "-yyyy-MM-dd--hh-mm-ss-zzz");


     /**
      * @brief Take a snapshot of a window associated to a given item and save it
      *
      * @param item
      * @param snapshotSize
      * @param fileNamePrefix
      * @param dateTimeStampSuffix
      */
     void saveWindowOfItem(QQuickItem* item, QSize snapshotSize = QSize(),  QString fileNamePrefix = "snapshot", QString dateTimeStampSuffix = "-yyyy-MM-dd--hh-mm-ss-zzz");


     /**
      * @brief Save a given item grab result (snapshot) using a given prefix and given suffix for date-time stamp
      *
      * @param grabResult fileNamePrefix
      * @param fileNamePrefix
      * @param dateTimeStampSuffix
      *
      * @return true if our snapshot has been saved, false otherwise
      */
     bool saveItemGrabResult(QQuickItemGrabResult* grabResult, QString fileNamePrefix = "snapshot", QString dateTimeStampSuffix = "-yyyy-MM-dd--hh-mm-ss-zzz");


};


QML_DECLARE_TYPE(I2SnapshotHelper)

#endif // _I2_SNAPSHOTHELPER_H_
