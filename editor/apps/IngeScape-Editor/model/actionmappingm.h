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

    // List of mapping ids from an Agent to an Action in the global mapping (with format "outputObjectInMapping##output-->inputObjectInMapping##input")
    I2_CPP_PROPERTY(QStringList, mappingIdsList_FromAgentToAction)

    // List of mapping ids from an Action to an Action in the global mapping (with format "outputObjectInMapping##output-->inputObjectInMapping##input")
    //I2_CPP_PROPERTY(QStringList, mappingIdsList_FromActionToAction)

    // List of unique ids of (output) actions in the global mapping on which our action is mapped
    I2_CPP_PROPERTY(QList<int>, uidsListOfOutputActionsInMapping)

    // List of mapping ids from an Action to an Agent in the global mapping (with format "outputObjectInMapping##output-->inputObjectInMapping##input")
    I2_CPP_PROPERTY(QStringList, mappingIdsList_FromActionToAgent)


public:
    /**
     * @brief Constructor
     * @param parent
     */
    explicit ActionMappingM(QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    //~ActionMappingM();


    /**
     * @brief Get the mapping id (with format "outputObjectInMapping##output-->inputObjectInMapping##input") from the list of names (each parts of a mapping)
     * @param outputObjectInMapping
     * @param output
     * @param inputObjectInMapping
     * @param input
     * @return mappingId with format "outputObjectInMapping##output-->inputObjectInMapping##input"
     */
    static QString getMappingIdFromNamesList(QString outputObjectInMapping, QString output, QString inputObjectInMapping, QString input);


    /**
     * @brief Get the list of names (each parts of a mapping) from the mapping id (with format "outputObjectInMapping##output-->inputObjectInMapping##input")
     * @param mappingId with format "outputObjectInMapping##output-->inputObjectInMapping##input"
     * @return
     */
    static QStringList getNamesListFromMappingId(QString mappingId);

};

QML_DECLARE_TYPE(ActionMappingM)

#endif // ACTIONMAPPINGM_H
