/*
 *	I2Quick
 *
 *  Copyright (c) 2015-2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *		Alexandre Lemort <lemort@ingenuity.io>
 *
 */


#ifndef _I2_SIGNALUTILS_H_
#define _I2_SIGNALUTILS_H_

#include <QObject>

// To export symbols
#include "i2quick_global.h"


/**
 * @brief The I2SignalUtils class defines a set of utility functions to subscribe/unsubscribe
 *  to signals
 */
class I2QUICK_EXPORT I2SignalUtils : public QObject
{
    Q_OBJECT

public:
    explicit I2SignalUtils(QObject *parent = 0);

    /**
     * @brief Subscribe to all properties of a given source object
     *
     * @param source Source object that will emit property changed signals
     * @param observer Observer that want to receive property changed signals
     * @param observerSlot Slot of our observer that will be triggered when a property changed signal is emitted
     * @param excludeReadOnlyProperties Flag indicating if read-only properties must be excluded
     * @param type Type of connection
     */
    static void subscribeToAllProperties(QObject *source, QObject *observer, const char *observerSlot, bool excludeReadOnlyProperties = false, Qt::ConnectionType type = Qt::AutoConnection);


    /**
     * @brief Subscribe to all properties of a given source object, except the ones of unwantedProperties list
     *
     * @param source Source object that will emit property changed signals
     * @param observer Observer that want to receive property changed signals
     * @param observerSlot Slot of our observer that will be triggered when a property changed signal is emitted
     * @param unwantedProperties List of properties that we must not listen to property changed signals
     * @param excludeReadOnlyProperties Flag indicating if read-only properties must be excluded
     * @param type Type of connection
     */
    static void subscribeToAllPropertiesExcept(QObject *source, QObject *observer, const char *observerSlot, QList<QString> unwantedProperties, bool excludeReadOnlyProperties = false, Qt::ConnectionType type = Qt::AutoConnection);


    /**
     * @brief Subscribe to all writable properties of a given source object
     *        i.e. read-only properties are excluded
     *
     * @param source Source object that will emit property changed signals
     * @param observer Observer that want to receive property changed signals
     * @param observerSlot Slot of our observer that will be triggered when a property changed signal is emitted
     * @param type Type of connection
     */
    static void subscribeToAllWritableProperties(QObject *source, QObject *observer, const char *observerSlot, Qt::ConnectionType type = Qt::AutoConnection);


    /**
     * @brief Subscribe to all writable properties of a given source object, except the ones of unwantedProperties list
     *        i.e. read-only properties are excluded
     *
     * @param source Source object that will emit property changed signals
     * @param observer Observer that want to receive property changed signals
     * @param observerSlot Slot of our observer that will be triggered when a property changed signal is emitted
     * @param unwantedProperties List of properties that we must not listen to property changed signals
     * @param type Type of connection
     */
    static void subscribeToAllWritablePropertiesExcept(QObject *source, QObject *observer, const char *observerSlot, QList<QString> unwantedProperties, Qt::ConnectionType type = Qt::AutoConnection);
};

#endif // _I2_SIGNALUTILS_H_
