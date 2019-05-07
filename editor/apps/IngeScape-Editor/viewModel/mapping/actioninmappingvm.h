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

#ifndef ACTIONINMAPPINGVM_H
#define ACTIONINMAPPINGVM_H

#include <QObject>
#include <I2PropertyHelpers.h>
#include <viewModel/mapping/objectinmappingvm.h>
#include <model/scenario/actionm.h>

/**
 * @brief The ActionInMappingVM class defines a view model of action in the global mapping
 */
class ActionInMappingVM : public ObjectInMappingVM
{
    Q_OBJECT

    // Model of action
    I2_QML_PROPERTY_READONLY(ActionM*, action)


public:

    /**
     * @brief Constructor
     * @param action
     * @param position
     * @param parent
     */
    explicit ActionInMappingVM(ActionM* action,
                               QPointF position,
                               QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~ActionInMappingVM();

};

QML_DECLARE_TYPE(ActionInMappingVM)

#endif // ACTIONINMAPPINGVM_H
