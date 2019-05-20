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

#ifndef ACTIONINMAPPINGVM_H
#define ACTIONINMAPPINGVM_H

#include <QObject>
#include <I2PropertyHelpers.h>
#include <viewModel/mapping/objectinmappingvm.h>
#include <model/scenario/actionm.h>
#include <viewModel/link/linkinputvm.h>
#include <viewModel/link/linkoutputvm.h>

/**
 * @brief The ActionInMappingVM class defines a view model of action in the global mapping
 * Allows to manage actions and links graphically
 */
class ActionInMappingVM : public ObjectInMappingVM
{
    Q_OBJECT

    // Be carreful: do not confond the uid of our view model of action in the mapping
    // and the uid of the model of action !
    // The same model of action can be Drag & Drop several times in the global mapping

    // Unique id of our action in the global mapping
    I2_QML_PROPERTY_READONLY(int, uid)

    // Model of action
    I2_QML_PROPERTY_READONLY(ActionM*, action)

    // View model of link input
    I2_QML_PROPERTY_READONLY(LinkInputVM*, linkInput)

    // View model of link output
    I2_QML_PROPERTY_READONLY(LinkOutputVM*, linkOutput)


public:

    /**
     * @brief Constructor
     * @param uid
     * @param action
     * @param position
     * @param parent
     */
    explicit ActionInMappingVM(int uid,
                               ActionM* action,
                               QPointF position,
                               QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~ActionInMappingVM();


Q_SIGNALS:


private Q_SLOTS:

    /**
     * @brief Slot called when all effects have been executed
     */
    void _onAllEffectsHaveBeenExecuted();


};

QML_DECLARE_TYPE(ActionInMappingVM)

#endif // ACTIONINMAPPINGVM_H
