/*
 *	IngeScape Editor
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

#ifndef MAPBETWEENIOPVM_H
#define MAPBETWEENIOPVM_H

#include <QObject>
#include <QtQml>

#include <I2PropertyHelpers.h>
#include <viewModel/agentinmappingvm.h>


/**
 * @brief The MapBetweenIOPVM class defines the view model of a link between two agents
 */
class MapBetweenIOPVM : public QObject
{
    Q_OBJECT

    // View model of the output agent of our link (link starts from this agent)
    I2_QML_PROPERTY_DELETE_PROOF(AgentInMappingVM*, outputAgent)

    // View model of the output slot associated to our link
    I2_QML_PROPERTY_DELETE_PROOF(OutputVM*, output)

    // View model of the input agent of our link (link ends to this agent)
    I2_QML_PROPERTY_DELETE_PROOF(AgentInMappingVM*, inputAgent)

    // View model of the input slot associated to our link
    I2_QML_PROPERTY_DELETE_PROOF(InputVM*, input)

    // Flag indicating if our link is virtual (agents are not really linked on the network)
    I2_QML_PROPERTY_READONLY(bool, isVirtual)

    // Identifier with all names: [outputAgent##output-->inputAgent##input]
    I2_CPP_NOSIGNAL_PROPERTY(QString, id)

    // Identifier with agents names and Input/Output ids: [outputAgent##output::type-->inputAgent##input::type]
    //I2_CPP_NOSIGNAL_PROPERTY(QString, uid)

public:

    /**
     * @brief Default constructor
     * @param outputAgent The link starts from this agent
     * @param output The link starts from this output of the output agent
     * @param inputAgent The link ends to this agent
     * @param input The link ends to this input of the input agent
     * @param isVirtual
     * @param parent
     */
    explicit MapBetweenIOPVM(AgentInMappingVM* outputAgent,
                             OutputVM* output,
                             AgentInMappingVM* inputAgent,
                             InputVM* input,
                             bool isVirtual,
                             QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~MapBetweenIOPVM();
};

QML_DECLARE_TYPE(MapBetweenIOPVM)

#endif // MAPBETWEENIOPVM_H
