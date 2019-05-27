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

#include "actionmappingm.h"
#include <model/enums.h>

/**
 * @brief Constructor
 * @param parent
 */
ActionMappingM::ActionMappingM(QObject *parent) : QObject(parent),
    _mappingIdsList_FromAgentToAction(QStringList()),
    _uidsListOfOutputActionsInMapping(QStringList()),
    _mappingIdsList_FromActionToAgent(QStringList())
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}


/**
 * @brief Destructor
 */
/*ActionMappingM::~ActionMappingM()
{
}*/


/**
 * @brief Get the mapping id (with format "outputObjectInMapping##output-->inputObjectInMapping##input") from the list of UIDs (each parts of a mapping)
 * @param outputObjectInMapping
 * @param output
 * @param inputObjectInMapping
 * @param input
 * @return mappingId with format "outputObjectInMapping##output-->inputObjectInMapping##input"
 */
QString ActionMappingM::getMappingIdFromUIDsList(QString outputObjectInMapping, QString output, QString inputObjectInMapping, QString input)
{
    if (!outputObjectInMapping.isEmpty() && !output.isEmpty() && !inputObjectInMapping.isEmpty() && !input.isEmpty())
    {
        // outputObjectInMapping##output-->inputObjectInMapping##input
        return QString("%1%2%3%4%5%2%6").arg(outputObjectInMapping, SEPARATOR_AGENT_NAME_AND_IOP, output, SEPARATOR_LINK_OUTPUT_AND_LINK_INPUT, inputObjectInMapping, input);
    }
    else {
        return "";
    }
}


/**
 * @brief Get the list of UIDs (each parts of a mapping) from the mapping id (with format "outputObjectInMapping##output-->inputObjectInMapping##input")
 * @param mappingId with format "outputObjectInMapping##output-->inputObjectInMapping##input"
 * @return
 */
QStringList ActionMappingM::getUIDsListFromMappingId(QString mappingId)
{
    QStringList namesList;

    if (mappingId.contains(SEPARATOR_LINK_OUTPUT_AND_LINK_INPUT))
    {
        QStringList outputAndInput = mappingId.split(SEPARATOR_LINK_OUTPUT_AND_LINK_INPUT);
        if (outputAndInput.count() == 2)
        {
            QString outputObjectInMappingAndOuput = outputAndInput.at(0);
            QString inputObjectInMappingAndInput = outputAndInput.at(1);

            if (outputObjectInMappingAndOuput.contains(SEPARATOR_AGENT_NAME_AND_IOP)
                    && inputObjectInMappingAndInput.contains(SEPARATOR_AGENT_NAME_AND_IOP))
            {
                QStringList outputObjectInMappingAndOuputList = outputObjectInMappingAndOuput.split(SEPARATOR_AGENT_NAME_AND_IOP);
                QStringList inputObjectInMappingAndInputList = inputObjectInMappingAndInput.split(SEPARATOR_AGENT_NAME_AND_IOP);

                if ((outputObjectInMappingAndOuputList.count() == 2) && (inputObjectInMappingAndInputList.count() == 2))
                {
                    namesList.append(outputObjectInMappingAndOuputList);
                    namesList.append(inputObjectInMappingAndInputList);
                }
            }
        }
    }
    return namesList;
}
