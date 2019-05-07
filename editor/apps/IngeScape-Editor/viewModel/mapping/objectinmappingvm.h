/*
 *	IngeScape Editor
 *
 *  Copyright © 2017-2018 Ingenuity i/o. All rights reserved.
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
 * @brief The ObjectInMappingVM class is the base class for derived class of view model of agent/action in mapping
 */
class ObjectInMappingVM : public QObject
{
    Q_OBJECT

    // Name of our object
    I2_QML_PROPERTY_READONLY(QString, name)

    // The position corresponds to the corner Top-Left of the box
    I2_QML_PROPERTY(QPointF, position)

    // List of view models of link inputs
    //I2_QOBJECT_LISTMODEL(LinkInputVM, linkInputsList)

    // List of view models of link outputs
    //I2_QOBJECT_LISTMODEL(LinkOutputVM, linkOutputsList)


public:

    /**
     * @brief Constructor
     * @param name
     * @param position
     * @param parent
     */
    explicit ObjectInMappingVM(QString name,
                               QPointF position,
                               QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~ObjectInMappingVM();

};

QML_DECLARE_TYPE(ObjectInMappingVM)

#endif // OBJECTINMAPPINGVM_H
