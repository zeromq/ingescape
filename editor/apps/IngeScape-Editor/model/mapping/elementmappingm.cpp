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

#include "elementmappingm.h"
#include <model/enums.h>

/**
 * @brief Constructor with input and output agent and IOP
 * @param inputAgent
 * @param input
 * @param outputAgent
 * @param output
 * @param parent
 */
ElementMappingM::ElementMappingM(QString inputAgent,
                                 QString input,
                                 QString outputAgent,
                                 QString output,
                                 QObject *parent) : QObject(parent),
    _name(""),
    _inputAgent(inputAgent),
    _input(input),
    _outputAgent(outputAgent),
    _output(output)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    // Get our link name (with format "outputAgent##output-->inputAgent##input") from the list of names (each parts of our mapping element)
    _name = ElementMappingM::getLinkNameFromNamesList(_outputAgent, _output, _inputAgent, _input);

    //qInfo() << "New Model of Element Mapping" << _name;
}


/**
 * @brief Destructor
 */
ElementMappingM::~ElementMappingM()
{
    //qInfo() << "Delete Model of Element Mapping" << _name;
}


/**
 * @brief Get the link name (with format "outputAgent##output-->inputAgent##input") from the list of names (each parts of a mapping element)
 * @param outputAgent
 * @param output
 * @param inputAgent
 * @param input
 * @return link name with format "outputAgent##output-->inputAgent##input"
 */
QString ElementMappingM::getLinkNameFromNamesList(QString outputAgent, QString output, QString inputAgent, QString input)
{
    if (!inputAgent.isEmpty() && !input.isEmpty() && !outputAgent.isEmpty() && !output.isEmpty())
    {
        // outputAgent##output-->inputAgent##input
        return QString("%1%2%3%4%5%2%6").arg(outputAgent, SEPARATOR_AGENT_NAME_AND_IOP, output, SEPARATOR_LINK_OUTPUT_AND_LINK_INPUT, inputAgent, input);
    }
    else {
        return "";
    }
}


/**
 * @brief Get the list of names (each parts of a mapping element) from the link name (with format "outputAgent##output-->inputAgent##input")
 * @param link name with format "outputAgent##output-->inputAgent##input"
 * @return
 */
QStringList ElementMappingM::getNamesListFromLinkName(QString linkName)
{
    QStringList namesList;

    if (linkName.contains(SEPARATOR_LINK_OUTPUT_AND_LINK_INPUT))
    {
        QStringList outputAndInput = linkName.split(SEPARATOR_LINK_OUTPUT_AND_LINK_INPUT);
        if (outputAndInput.count() == 2)
        {
            QString outputAgentAndOuput = outputAndInput.at(0);
            QString inputAgentAndInput = outputAndInput.at(1);

            if (outputAgentAndOuput.contains(SEPARATOR_AGENT_NAME_AND_IOP)
                    && inputAgentAndInput.contains(SEPARATOR_AGENT_NAME_AND_IOP))
            {
                QStringList outputAgentAndOuputList = outputAgentAndOuput.split(SEPARATOR_AGENT_NAME_AND_IOP);
                QStringList inputAgentAndInputList = inputAgentAndInput.split(SEPARATOR_AGENT_NAME_AND_IOP);

                if ((outputAgentAndOuputList.count() == 2) && (inputAgentAndInputList.count() == 2))
                {
                    namesList.append(outputAgentAndOuputList);
                    namesList.append(inputAgentAndInputList);
                }
            }
        }
    }

    return namesList;
}
