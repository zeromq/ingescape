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

#include "linkvm.h"


/**
 * @brief Constructor
 * @param name
 * @param mappingElement corresponding mapping element
 * @param outputObject The link starts from this object (in the global mapping)
 * @param linkOutput The link starts from this output of the output agent
 * @param inputObject The link ends to this object (in the global mapping)
 * @param linkInput The link ends to this input of the input agent
 * @param isTemporary
 * @param parent
 */
LinkVM::LinkVM(QString name,
               MappingElementVM* mappingElement,
               ObjectInMappingVM* outputObject,
               LinkOutputVM* linkOutput,
               ObjectInMappingVM* inputObject,
               LinkInputVM* linkInput,
               bool isTemporary,
               QObject *parent) : QObject(parent),
    _name(name),
    _uid(""),
    _mappingElement(nullptr),
    _outputObject(outputObject),
    _linkOutput(linkOutput),
    _inputObject(inputObject),
    _linkInput(linkInput),
    _isTemporary(isTemporary)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    if ((_outputObject != nullptr) && (_linkOutput != nullptr) && (_inputObject != nullptr) && (_linkInput != nullptr))
    {
        // Get our link id (with format "outputObjectId##output::outputType-->inputObjectId##input::inputType") from object ids and Input/Output ids
        _uid = LinkVM::getLinkIdFromObjectIdsAndIOids(_outputObject->uid(), _linkOutput->uid(), _inputObject->uid(), _linkInput->uid());

        //qInfo() << "New Link" << _uid << "in the global mapping (" << _name << ")";

        // Output object or Input object is an action (in the global mapping)
        if ((_outputObject->type() == ObjectInMappingTypes::ACTION) || (_inputObject->type() == ObjectInMappingTypes::ACTION))
        {
            // Connect to signal "activated" from the link output
            connect(_linkOutput, &LinkOutputVM::activated, this, &LinkVM::_onOutputActivated);
        }
        // Output object and Input object are agents (in the global mapping)
        /*else {
            // Nothing to do because the library IngeScape already manage mappings between agents
        }*/
    }

    setmappingElement(mappingElement);
}


/**
 * @brief Destructor
 */
LinkVM::~LinkVM()
{
    //qInfo() << "Delete Link" << _uid << "in the global mapping (" << _name << ")";

    setmappingElement(nullptr);

    setoutputObject(nullptr);

    if (_linkOutput != nullptr)
    {
        // DIS-connect to signal "activated" from the link output
        disconnect(_linkOutput, &LinkOutputVM::activated, this, &LinkVM::_onOutputActivated);

        setlinkOutput(nullptr);
    }

    setinputObject(nullptr);

    setlinkInput(nullptr);
}


/**
 * @brief Setter for property "Mapping Element"
 * @param value
 */
void LinkVM::setmappingElement(MappingElementVM *value)
{
    if (_mappingElement != value)
    {
        // Previous value
        if (_mappingElement != nullptr) {
            _mappingElement->sethasCorrespondingLink(false);
        }

        _mappingElement = value;

        // New value
        if (_mappingElement != nullptr) {
            _mappingElement->sethasCorrespondingLink(true);
        }

        mappingElementChanged(value);
    }
}


/**
 * @brief Get the link id (with format "outputObjectId##output::outputType-->inputObjectId##input::inputType") from object ids and Input/Output ids
 * @param outputObjectId
 * @param outputId
 * @param inputObjectId
 * @param inputId
 * @return link id with format "outputObjectId##output::outputType-->inputObjectId##input::inputType"
 */
QString LinkVM::getLinkIdFromObjectIdsAndIOids(QString outputObjectId, QString outputId, QString inputObjectId, QString inputId)
{
    if (!outputObjectId.isEmpty() && !outputId.isEmpty() && !inputObjectId.isEmpty() && !inputId.isEmpty())
    {
        // outputObjectId ## output :: outputType --> inputObjectId ## input :: inputType
        return QString("%1%2%3%4%5%2%6").arg(outputObjectId, SEPARATOR_AGENT_NAME_AND_IOP, outputId, SEPARATOR_LINK_OUTPUT_AND_LINK_INPUT, inputObjectId, inputId);
    }
    else {
        return "";
    }
}


/**
 * @brief Slot called when the (link) output has been activated
 */
void LinkVM::_onOutputActivated()
{
    if ((_inputObject != nullptr) && (_linkInput != nullptr))
    {
        // Emit the signal "Activate Input of Object in Mapping"
        Q_EMIT activateInputOfObjectInMapping(_inputObject, _linkInput);
    }
}
