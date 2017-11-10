/*
 *	MASTIC Editor
 *
 *  Copyright © 2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *      Alexandre Lemort   <lemort@ingenuity.io>
 *
 */

#ifndef INPUTVM_H
#define INPUTVM_H

#include <QObject>

#include <I2PropertyHelpers.h>

#include <viewModel/pointmapvm.h>
#include <model/iop/agentiopm.h>


/**
 * @brief The InputVM class defines a view model of input
 */
class InputVM : public PointMapVM
{
    Q_OBJECT

    // First model of our agent Input
    I2_QML_PROPERTY_READONLY_DELETE_PROOF(AgentIOPM*, firstModel)

    // Models of our agent input
    I2_QOBJECT_LISTMODEL(AgentIOPM, models)


public:
    explicit InputVM(QString inputName,
                     QString inputId,
                     AgentIOPM* modelM,
                     QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~InputVM();


Q_SIGNALS:

public Q_SLOTS:

    /**
     * @brief Return true if our input can link with the output (types are compatible)
     * @param pointMap
     * @return
     */
    bool canLinkWith(PointMapVM* pointMap) Q_DECL_OVERRIDE;


private Q_SLOTS:
    /**
     * @brief Slot when the list of models changed
     */
    void _onModelsChanged();


private:
    // Previous list of models
    //QList<AgentIOPM*> _previousModelsList;

};

QML_DECLARE_TYPE(InputVM)

#endif // INPUTVM_H
