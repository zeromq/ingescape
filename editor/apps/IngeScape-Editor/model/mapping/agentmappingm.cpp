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

#include "agentmappingm.h"

/**
 * @brief Constructor
 * @param name
 * @param version
 * @param description
 * @param parent
 */
AgentMappingM::AgentMappingM(QString name,
                             QString version,
                             QString description,
                             QObject *parent) : QObject(parent),
    _name(name),
    _version(version),
    _description(description)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    //qInfo() << "New Model of Agent Mapping" << _name << "with version" << _version << "about" << _description;

    // Connect to signal "Count Changed" from the list of mapping elements
    connect(&_mappingElements, &AbstractI2CustomItemListModel::countChanged, this, &AgentMappingM::_onMappingElementsListChanged);
}


/**
 * @brief Destructor
 */
AgentMappingM::~AgentMappingM()
{
    //qInfo() << "Delete Model of Agent Mapping" << _name;

    // DIS-connect to signal "Count Changed" from the list of mapping elements
    disconnect(&_mappingElements, &AbstractI2CustomItemListModel::countChanged, this, &AgentMappingM::_onMappingElementsListChanged);

    // FIXME: Clear the list (do not delete all)
    //_mappingElements.deleteAllItems();
    _mappingElements.clear();

    _namesOfMappingElements.clear();
}


/**
 * @brief Slot called when the list of mapping elements changed
 */
void AgentMappingM::_onMappingElementsListChanged()
{
    _namesOfMappingElements.clear();

    for (ElementMappingM* mappingElement : _mappingElements.toList())
    {
        if ((mappingElement != nullptr) && !mappingElement->name().isEmpty()) {
            _namesOfMappingElements.append(mappingElement->name());
        }
    }

    //qDebug() << "Mapping" << _name << "Names:" << _namesOfMappingElements;
}
