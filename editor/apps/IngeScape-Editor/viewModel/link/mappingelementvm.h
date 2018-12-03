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

    // Identifier with all names: [outputAgent##output-->inputAgent##input]
    I2_CPP_NOSIGNAL_PROPERTY(QString, name)

    // First model of mapping element
    //I2_QML_PROPERTY_READONLY(ElementMappingM*, firstModel)

    // Models of mapping elements
    I2_QOBJECT_LISTMODEL(ElementMappingM, models)


public:
    explicit MappingElementVM(QString name,
                              ElementMappingM* modelM,
                              QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~MappingElementVM();


Q_SIGNALS:


public Q_SLOTS:

};

QML_DECLARE_TYPE(MappingElementVM)

#endif // MAPPINGELEMENTVM_H
