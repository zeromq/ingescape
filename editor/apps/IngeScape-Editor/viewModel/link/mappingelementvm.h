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

#ifndef MAPPINGELEMENTVM_H
#define MAPPINGELEMENTVM_H

#include <QObject>
#include <I2PropertyHelpers.h>
#include <model/mapping/elementmappingm.h>

/**
 * @brief The MappingElementVM class defines a view model of a mapping element
 */
class MappingElementVM : public QObject
{
    Q_OBJECT

    // Name with all names formatted: "outputAgent##output-->inputAgent##input"
    //I2_CPP_NOSIGNAL_PROPERTY(QString, name)
    I2_QML_PROPERTY_READONLY(QString, name)

    // Models of mapping elements
    I2_QOBJECT_LISTMODEL(ElementMappingM, models)

    // First model of mapping element
    I2_QML_PROPERTY_READONLY(ElementMappingM*, firstModel)


public:

    /**
     * @brief Constructor
     * @param name
     * @param modelM
     * @param parent
     */
    explicit MappingElementVM(QString name,
                              ElementMappingM* modelM,
                              QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~MappingElementVM();


Q_SIGNALS:


public Q_SLOTS:


protected Q_SLOTS:
    /**
     * @brief Slot called when the list of models changed
     */
    void _onModelsChanged();

};

QML_DECLARE_TYPE(MappingElementVM)

#endif // MAPPINGELEMENTVM_H
