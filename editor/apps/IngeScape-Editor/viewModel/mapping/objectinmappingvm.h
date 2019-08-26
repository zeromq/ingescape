/*
 *	IngeScape Editor
 *
 *  Copyright © 2017-2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

#ifndef OBJECTINMAPPINGVM_H
#define OBJECTINMAPPINGVM_H

#include <QObject>
#include <I2PropertyHelpers.h>


/**
  * Types of objects in the mapping
  */
I2_ENUM(ObjectInMappingTypes, UNKNOWN, AGENT, ACTION)


/**
 * @brief The ObjectInMappingVM class is the base class for derived class of view model of agent/action in mapping
 */
class ObjectInMappingVM : public QObject
{
    Q_OBJECT

    // Type of our object in the global mapping
    I2_QML_PROPERTY_READONLY(ObjectInMappingTypes::Value, type)

    // Name of our object in the global mapping
    I2_QML_PROPERTY_READONLY(QString, name)

    // Unique id of our object in the global mapping
    I2_QML_PROPERTY_READONLY(QString, uid)

    // The position corresponds to the corner Top-Left of the box
    I2_QML_PROPERTY(QPointF, position)

    // The object's width (some objects can be resized)
    I2_QML_PROPERTY_FUZZY_COMPARE(qreal, width)

    // Flag indicating if our object is reduced
    I2_QML_PROPERTY(bool, isReduced)

    // List of view models of link inputs
    //I2_QOBJECT_LISTMODEL(LinkInputVM, linkInputsList)

    // List of view models of link outputs
    //I2_QOBJECT_LISTMODEL(LinkOutputVM, linkOutputsList)


public:

    /**
     * @brief Constructor
     * @param type
     * @param name
     * @param uid
     * @param position
     * @param parent
     */
    explicit ObjectInMappingVM(ObjectInMappingTypes::Value type,
                               //QString name,
                               //QString uid,
                               QPointF position,
                               qreal width,
                               QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~ObjectInMappingVM();

};

QML_DECLARE_TYPE(ObjectInMappingVM)

#endif // OBJECTINMAPPINGVM_H
