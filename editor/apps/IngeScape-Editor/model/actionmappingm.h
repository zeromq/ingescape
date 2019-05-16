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

#ifndef ACTIONMAPPINGM_H
#define ACTIONMAPPINGM_H

#include <QObject>
#include <I2PropertyHelpers.h>


/**
 * @brief The ActionMappingM class defines a model of action mapping
 */
class ActionMappingM : public QObject
{
    Q_OBJECT

    // List of names of (output) agents in the global mapping (and its output) on which our action is mapped
    I2_CPP_PROPERTY(QStringList, namesListOfOutputAgentInMappingAndOuputName)

    // List of unique ids of (output) actions in the global mapping on which our action is mapped
    I2_CPP_PROPERTY(QList<int>, uidsListOfOutputActionsInMapping)


public:
    /**
     * @brief Constructor
     * @param namesListOfOutputAgentInMappingAndOuputName
     * @param uidsListOfOutputActionsInMapping
     * @param parent
     */
    explicit ActionMappingM(QStringList namesListOfOutputAgentInMappingAndOuputName,
                            QList<int> uidsListOfOutputActionsInMapping,
                            QObject *parent = nullptr);


};

QML_DECLARE_TYPE(ActionMappingM)

#endif // ACTIONMAPPINGM_H
