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

    // Clear the map
    //_mapFromNameToMappingElement.clear();

    _namesOfMappingElements.clear();

    // Delete all models of mapping elements
    //_previousListOfMappingElements.clear();
    // FIXME: Delete all mapping elements
    //_mappingElements.deleteAllItems();
    _mappingElements.clear();
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

    /*QList<ElementMappingM*> newMappingElementsList = _mappingElements.toList();

    // Mapping Element added
    if (_previousListOfMappingElements.count() < newMappingElementsList.count())
    {
        for (ElementMappingM* mappingElement : newMappingElementsList)
        {
            if ((mappingElement != nullptr) && !_previousListOfMappingElements.contains(mappingElement))
            {
                _namesOfMappingElements.append(mappingElement->name());
                _mapFromNameToMappingElement.insert(mappingElement->name(), mappingElement);
            }
        }
    }
    // Mapping Element removed
    else if (_previousListOfMappingElements.count() > newMappingElementsList.count())
    {
        for (ElementMappingM* mappingElement : _previousListOfMappingElements)
        {
            if ((mappingElement != nullptr) && !newMappingElementsList.contains(mappingElement))
            {
                _namesOfMappingElements.removeOne(mappingElement->name());
                _mapFromNameToMappingElement.remove(mappingElement->name());
            }
        }
    }

    _previousListOfMappingElements = newMappingElementsList;*/

    //qDebug() << "Mapping" << _name << "has elements:" << _namesOfMappingElements;
}
