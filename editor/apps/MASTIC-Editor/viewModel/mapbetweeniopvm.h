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
    I2_QML_PROPERTY_DELETE_PROOF(AgentInMappingVM*, agentFrom)

    // View model of the output slot associated to our link
    I2_QML_PROPERTY_DELETE_PROOF(OutputVM*, pointFrom)

    // View model of the input agent of our link (link ends to this agent)
    I2_QML_PROPERTY_DELETE_PROOF(AgentInMappingVM*, agentTo)

    // View model of the input slot associated to our link
    I2_QML_PROPERTY_DELETE_PROOF(InputVM*, pointTo)

    // Flag indicating if our link is virtual (agents are not really linked on the network)
    I2_QML_PROPERTY_READONLY(bool, isVirtual)


public:

    /**
     * @brief Default constructor
     * @param agentFrom The link starts from this agent
     * @param pointFrom The link starts from this output of the agentFrom
     * @param agentTo The link ends to this agent
     * @param pointTo The link ends to this input of the agentTo
     * @param isVirtual
     * @param parent
     */
    explicit MapBetweenIOPVM(AgentInMappingVM* agentFrom,
                             OutputVM *pointFrom,
                             AgentInMappingVM* agentTo,
                             InputVM *pointTo,
                             bool isVirtual,
                             QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~MapBetweenIOPVM();
};

QML_DECLARE_TYPE(MapBetweenIOPVM)

#endif // MAPBETWEENIOPVM_H
