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
 *
 */

#ifndef AGENTIOPVM_H
#define AGENTIOPVM_H

#include <QObject>
#include <I2PropertyHelpers.h>

#include <model/iop/agentiopm.h>

/**
 * @brief The AgentIOPVM is the base class for derived class of View Model of agent Input / Output / Parameter
 */
class AgentIOPVM : public QObject
{
    Q_OBJECT

    // Model of our agent Input / Output / Parameter
    I2_QML_PROPERTY(AgentIOPM*, modelM)

    // Mapping value of our Input / Output / Parameter
    I2_CPP_PROPERTY(QVariant, mappingValue)

    // Displayable mapping value of our Input / Output / Parameter
    I2_QML_PROPERTY(QString, displayableMappingValue)

    // current value of our Input / Output / Parameter
    I2_CPP_PROPERTY(QVariant, currentValue)

    // Displayable current value of our Input / Output / Parameter
    I2_QML_PROPERTY(QString, displayableCurrentValue)

    // Past values
    //I2_QOBJECT_LISTMODEL(QPair<QDateTime, QString>, pastValues)


public:
    explicit AgentIOPVM(AgentIOPM* model, QObject *parent = 0);


    /**
     * @brief Destructor
     */
    ~AgentIOPVM();


signals:

public slots:
};

QML_DECLARE_TYPE(AgentIOPVM)

#endif // AGENTIOPVM_H
