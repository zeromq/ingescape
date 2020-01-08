/*
 *	IngeScape Assessments
 *
 *  Copyright © 2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

#include "protocolscontroller.h"

#include <controller/assessmentsmodelmanager.h>
#include "model/protocol/independentvariablevaluem.h"

/**
 * @brief Constructor
 * @param parent
 */
ProtocolsController::ProtocolsController(QObject *parent) : QObject(parent),
    _currentExperimentation(nullptr),
    _selectedProtocol(nullptr),
    _temporaryIndependentVariable(nullptr),
    _temporaryDependentVariable(nullptr)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Protocols Controller";

    // Init temporary independent and dependant variable as empty variables
    _temporaryIndependentVariable = new IndependentVariableM(CassUuid(), CassUuid(), CassUuid(), "", "", IndependentVariableValueTypes::UNKNOWN);
    _temporaryDependentVariable = new DependentVariableM(CassUuid(), CassUuid(), CassUuid(), "", "", "", "");

    // Fill without type "UNKNOWN"
    _allIndependentVariableValueTypes.fillWithAllEnumValues();
    _allIndependentVariableValueTypes.removeEnumValue(IndependentVariableValueTypes::UNKNOWN);

    // Fill without type "ENUM" and "UNKNOWN"
    _independentVariableValueTypesWithoutEnum.fillWithAllEnumValues();
    _independentVariableValueTypesWithoutEnum.removeEnumValue(IndependentVariableValueTypes::INDEPENDENT_VARIABLE_ENUM);
    _independentVariableValueTypesWithoutEnum.removeEnumValue(IndependentVariableValueTypes::UNKNOWN);
}


/**
 * @brief Destructor
 */
ProtocolsController::~ProtocolsController()
{
    qInfo() << "Delete Protocols Controller";

    if (_selectedProtocol != nullptr) {
        setselectedProtocol(nullptr);
    }

    // Reset the model of the current experimentation
    if (_currentExperimentation != nullptr)
    {
        setcurrentExperimentation(nullptr);
    }

    // Reset the model of the temporary independant variable
    if (_temporaryIndependentVariable != nullptr)
    {
        IndependentVariableM* tmp = _temporaryIndependentVariable;
        settemporaryIndependentVariable(nullptr);
        delete tmp;
    }

    // Reset the model of the temporary dependant variable
    if (_temporaryDependentVariable != nullptr)
    {
        DependentVariableM* tmp = _temporaryDependentVariable;
        settemporaryDependentVariable(nullptr);
        delete tmp;
    }
}


/**
 * @brief Return true if the user can create a protocol with the name
 * Check if the name is not empty and if a protocol with the same name does not already exist
 * @param protocolName
 * @return
 */
bool ProtocolsController::canCreateProtocolWithName(QString protocolName)
{
    const QList<ProtocolM*>& protocolsList = _currentExperimentation->allProtocols()->toList();
    auto hasGivenName = [protocolName](ProtocolM* protocol) {
        return (protocol != nullptr) && (protocol->name() == protocolName);
    };

    return !protocolName.isEmpty() && (_currentExperimentation != nullptr)
            && std::none_of(protocolsList.begin(), protocolsList.end(), hasGivenName);
}


/**
 * @brief Create a new protocol with an IngeScape platform file path
 * @param protocolName
 * @param platformFilePath
 */
void ProtocolsController::createNewProtocolWithIngeScapePlatformFilePath(QString protocolName, QString platformFilePath)
{
    if (!protocolName.isEmpty() && !platformFilePath.isEmpty())
    {
        qInfo() << "Create new protocol" << protocolName << "with file" << platformFilePath;

        QUrl platformFileUrl = QUrl(platformFilePath);

        if (platformFileUrl.isValid())
        {
            // Create a new protocol with an IngeScape platform file URL
            _createNewProtocolWithIngeScapePlatformFileUrl(protocolName, platformFileUrl);
        }
        else {
            qWarning() << "Failed to create the protocol" << protocolName << "because the path" << platformFilePath << "is wrong !";
        }
    }
}


/**
 * @brief Delete a protocol
 * @param protocol
 */
void ProtocolsController::deleteProtocol(ProtocolM* protocol)
{
    if ((protocol != nullptr) && (_currentExperimentation != nullptr) && (AssessmentsModelManager::instance() != nullptr))
    {
        if (protocol == _selectedProtocol) {
            setselectedProtocol(nullptr);
        }

        // Remove sessions related to the protocol
        QList<CassUuid> subjectUuidList;
        for (SubjectM* subject : *(_currentExperimentation->allSubjects())) {
            if (subject != nullptr)
            {
                subjectUuidList.append(subject->getCassUuid());
            }
        }

        AssessmentsModelManager::deleteEntry<SessionM>({ { _currentExperimentation->getCassUuid() }, subjectUuidList, { protocol->getCassUuid() } });

        // Remove from DB
        ProtocolM::deleteProtocolFromCassandraRow(*protocol);

        // Remove the protocol from the current experimentation
        _currentExperimentation->removeProtocol(protocol);

        // Free memory
        delete protocol;
    }
}


/**
 * @brief Duplicate a protocol
 * @param protocol
 */
void ProtocolsController::duplicateProtocol(ProtocolM* protocol)
{
    if (protocol != nullptr)
    {
        QString protocolName = QString("%1_copy").arg(protocol->name());

        // Create a new protocol with an IngeScape platform file URL
        ProtocolM* newProtocol = _createNewProtocolWithIngeScapePlatformFileUrl(protocolName, protocol->platformFileUrl());

        if (newProtocol != nullptr)
        {
            // Copy each independent variables
            for (IndependentVariableM* independentVariable : protocol->independentVariables()->toList())
            {
                if (independentVariable != nullptr)
                {
                    // Create the new independent variable
                    IndependentVariableM* newIndependentVariable = _insertIndependentVariableIntoDB(newProtocol->getExperimentationCassUuid(),
                                                                                                    newProtocol->getCassUuid(),
                                                                                                    independentVariable->name(),
                                                                                                    independentVariable->description(),
                                                                                                    independentVariable->valueType(),
                                                                                                    independentVariable->enumValues() );

                    if (newIndependentVariable != nullptr)
                    {
                        // Add the independent variable to the new protocol
                        newProtocol->addIndependentVariable(newIndependentVariable);
                    }
                }
            }

            // Copy each dependent variables
            for (DependentVariableM* dependentVariable : newProtocol->dependentVariables()->toList())
            {
                if (dependentVariable != nullptr)
                {
                    // Create the new dependent variable
                    DependentVariableM* newDependentVariable = _insertDependentVariableIntoDB(newProtocol->getExperimentationCassUuid(),
                                                                                              newProtocol->getCassUuid(),
                                                                                              dependentVariable->name(),
                                                                                              dependentVariable->description(),
                                                                                              dependentVariable->agentName(),
                                                                                              dependentVariable->outputName() );

                    if (newDependentVariable != nullptr)
                    {
                        // Add the dependent variable to the new protocol
                        newProtocol->addDependentVariable(newDependentVariable);
                    }

                }
            }
        }
    }
}


/**
 * @brief Return true if the user can create an independent variable with the name
 * Check if the name is not empty and if an independent variable with the same name does not already exist
 * @param independentVariableName
 * @return
 */
bool ProtocolsController::canCreateIndependentVariableWithName(QString independentVariableName)
{
    const QList<IndependentVariableM*>& varList = _selectedProtocol->independentVariables()->toList();
    auto hasGivenName = [independentVariableName](IndependentVariableM* independentVariable) {
        return (independentVariable != nullptr) && (independentVariable->name() == independentVariableName);
    };

    return !independentVariableName.isEmpty() && (_selectedProtocol != nullptr) && std::none_of(varList.begin(), varList.end(), hasGivenName);
}


/**
 * @brief Return true if the user can create a dependent variable with the name
 * Check if the name is not empty and if a dependent variable with the same name does not already exist
 * @param dependentVariableName
 * @return
 */
bool ProtocolsController::canCreateDependentVariableWithName(QString dependentVariableName)
{
    const QList<DependentVariableM*>& varList = _selectedProtocol->dependentVariables()->toList();
    auto hasGivenName = [dependentVariableName](DependentVariableM* dependentVariable) {
        return (dependentVariable != nullptr) && (dependentVariable->name() == dependentVariableName);
    };

    return !dependentVariableName.isEmpty() && (_selectedProtocol != nullptr) && std::none_of(varList.begin(), varList.end(), hasGivenName);
}


/**
 * @brief Create a new independent variable from the Independent Variable currently edited (stored in _temporaryIndependentVariable)
 */
void ProtocolsController::createNewIndependentVariableFromTemporary()
{
    if ((_temporaryIndependentVariable != nullptr) && (!_temporaryIndependentVariable->name().isEmpty()) && (_selectedProtocol != nullptr))
    {
        // Create and insert the new independent variable
        IndependentVariableM* independentVariable = _insertIndependentVariableIntoDB(_selectedProtocol->getExperimentationCassUuid(), _selectedProtocol->getCassUuid(), _temporaryIndependentVariable->name(),
                                                                                     _temporaryIndependentVariable->description(), _temporaryIndependentVariable->valueType(), _temporaryIndependentVariable->enumValues());
        if (independentVariable != nullptr)
        {
            // Add the independent variable to the selected protocol
            _selectedProtocol->addIndependentVariable(independentVariable);
        }
    }
}


/**
 * @brief Save the modifications of the Independent Variable currently edited (stored in _temporaryIndependentVariable)
 * @param independentVariableCurrentlyEdited
 */
void ProtocolsController::saveModificationsOfIndependentVariableFromTemporary(IndependentVariableM* independentVariableCurrentlyEdited)
{
    if ((independentVariableCurrentlyEdited != nullptr) && (_temporaryIndependentVariable != nullptr) && (!_temporaryIndependentVariable->name().isEmpty())
            && (AssessmentsModelManager::instance() != nullptr))
    {
        independentVariableCurrentlyEdited->setname(_temporaryIndependentVariable->name());
        independentVariableCurrentlyEdited->setdescription(_temporaryIndependentVariable->description());
        independentVariableCurrentlyEdited->setvalueType(_temporaryIndependentVariable->valueType());
        independentVariableCurrentlyEdited->setenumValues(_temporaryIndependentVariable->enumValues());

        AssessmentsModelManager::update(*independentVariableCurrentlyEdited);
    }
}


/**
 * @brief Delete an independent variable
 * @param independentVariable
 */
void ProtocolsController::deleteIndependentVariable(IndependentVariableM* independentVariable)
{
    if ((independentVariable != nullptr) && (_selectedProtocol != nullptr) && (AssessmentsModelManager::instance() != nullptr))
    {
        // Delete independent variable values from Cassandra DB
        QList<CassUuid> sessionUuidList;
        for (SessionM* session : _currentExperimentation->allSessions()->toList())
        {
            if (session != nullptr)
            {
                sessionUuidList.append(session->getCassUuid());
            }
        }

        AssessmentsModelManager::deleteEntry<IndependentVariableValueM>({ { _currentExperimentation->getCassUuid() },
                                                                          sessionUuidList,
                                                                          { independentVariable->getCassUuid() } });

        // Delete independent variable from Cassandra DB
        IndependentVariableM::deleteIndependentVariableFromCassandra(*independentVariable);

        // Remove the independent variable from the selected protocol
        _selectedProtocol->removeIndependentVariable(independentVariable);

        // Free memory
        delete independentVariable;
    }
}


/**
 * @brief Create a new dependent variable from the Dependent Variable currently edited (stored in _temporaryDependentVariable)
 */
void ProtocolsController::createNewDependentVariableFromTemporary()
{
    if ((_temporaryDependentVariable != nullptr) && (!_temporaryDependentVariable->name().isEmpty()) && (!_temporaryDependentVariable->outputName().isEmpty())
            && (_selectedProtocol != nullptr) && (AssessmentsModelManager::instance() != nullptr))
    {
        // Create and insert the new dependent variable
        DependentVariableM* dependentVariable = _insertDependentVariableIntoDB(_selectedProtocol->getExperimentationCassUuid(),
                                                                               _selectedProtocol->getCassUuid(),
                                                                               _temporaryDependentVariable->name(),
                                                                               _temporaryDependentVariable->description(),
                                                                               _temporaryDependentVariable->agentName(),
                                                                               _temporaryDependentVariable->outputName());

        if (dependentVariable != nullptr)
        {
            // Add the dependent variable to the selected protocol
            _selectedProtocol->addDependentVariable(dependentVariable);
        }
    }
}


/**
 * @brief Save the modifications of the Dependent Variable currently edited (stored in _temporaryDependentVariable)
 * @param independentVariableCurrentlyEdited
 */
void ProtocolsController::saveModificationsOfDependentVariableFromTemporary(DependentVariableM* dependentVariableCurrentlyEdited)
{
    if ((dependentVariableCurrentlyEdited != nullptr) && (_temporaryDependentVariable != nullptr) && (!_temporaryDependentVariable->name().isEmpty()))
    {
        dependentVariableCurrentlyEdited->setname(_temporaryDependentVariable->name());
        dependentVariableCurrentlyEdited->setdescription(_temporaryDependentVariable->description());
        dependentVariableCurrentlyEdited->setagentName(_temporaryDependentVariable->agentName());
        dependentVariableCurrentlyEdited->setoutputName(_temporaryDependentVariable->outputName());

        AssessmentsModelManager::update(*dependentVariableCurrentlyEdited);
    }
}



/**
 * @brief Delete a dependent variable
 * @param dependentVariable
 */
void ProtocolsController::deleteDependentVariable(DependentVariableM* dependentVariable)
{
    if ((dependentVariable != nullptr) && (_selectedProtocol != nullptr))
    {
        // Remove the dependent variable from the selected protocol
        _selectedProtocol->removeDependentVariable(dependentVariable);

        // Remove from DB
        AssessmentsModelManager::deleteEntry<DependentVariableM>({ _selectedProtocol->getExperimentationCassUuid(),
                                                                   _selectedProtocol->getCassUuid() });

        // Free memory
        delete dependentVariable;
    }
}


/**
 * @brief Create a new protocol with an IngeScape platform file URL
 * @param protocolName
 * @param platformFileUrl
 * @return
 */
ProtocolM* ProtocolsController::_createNewProtocolWithIngeScapePlatformFileUrl(QString protocolName, QUrl platformFileUrl)
{
    ProtocolM* protocol = nullptr;

    if (!protocolName.isEmpty() && platformFileUrl.isValid() && (_currentExperimentation != nullptr) && (AssessmentsModelManager::instance() != nullptr))
    {
        // Create the new protocol
        protocol = new ProtocolM(_currentExperimentation->getCassUuid(),
                                 AssessmentsModelManager::genCassUuid(),
                                 protocolName,
                                 platformFileUrl);
        if ((protocol == nullptr) || !AssessmentsModelManager::insert(*protocol)) {
            delete protocol;
            protocol = nullptr;
        }
        else
        {
            // Add the protocol to the current experimentation
            _currentExperimentation->addProtocol(protocol);

            // Select this new protocol
            setselectedProtocol(protocol);
        }
    }
    else {
        qWarning() << "Cannot create new protocol because name is empty (" << protocolName << ") or platform file URL is wrong !";
    }

    return protocol;
}


/**
 * @brief Creates a new independent variable with the given parameters and insert it into the Cassandra DB
 * A nullptr is returned if the operation failed.
 * @param experimentationUuid
 * @param protocolUuid
 * @param name
 * @param description
 * @param valueType
 * @param enumValues
 * @return
 */
IndependentVariableM* ProtocolsController::_insertIndependentVariableIntoDB(CassUuid experimentationUuid, CassUuid protocolUuid, const QString& variableName, const QString& variableDescription, IndependentVariableValueTypes::Value valueType, const QStringList& enumValues)
{
    IndependentVariableM* independentVariable = nullptr;

    if (!variableName.isEmpty() && (AssessmentsModelManager::instance() != nullptr))
    {
        independentVariable = new IndependentVariableM(experimentationUuid, protocolUuid, AssessmentsModelManager::genCassUuid(), variableName, variableDescription, valueType, enumValues);
        if (independentVariable == nullptr || !AssessmentsModelManager::insert(*independentVariable))
        {
            delete independentVariable;
            independentVariable = nullptr;
        }
    }

    return independentVariable;
}


/**
 * @brief Creates a new dependent variable with the given parameters and insert it into the Cassandra DB
 * A nullptr is returned if the operation failed.
 * @param experimentationUuid
 * @param protocolUuid
 * @param name
 * @param description
 * @param valueType
 * @param enumValues
 * @return
 */
DependentVariableM* ProtocolsController::_insertDependentVariableIntoDB(CassUuid experimentationUuid,
                                                                    CassUuid protocolUuid,
                                                                    const QString& name,
                                                                    const QString& description,
                                                                    const QString& agentName,
                                                                    const QString& outputName)
{
    DependentVariableM* dependentVariable = new DependentVariableM(experimentationUuid,
                                                                   protocolUuid,
                                                                   AssessmentsModelManager::genCassUuid(),
                                                                   name,
                                                                   description,
                                                                   agentName,
                                                                   outputName);
    if (dependentVariable == nullptr || !AssessmentsModelManager::insert(*dependentVariable))
    {
        delete dependentVariable;
        dependentVariable = nullptr;
    }

    return dependentVariable;
}

/**
 * @brief Initialize the temporary independent variable with the given independent variable
 * @param baseVariable, if null, init empty temporary independent variable
 */
void ProtocolsController::initTemporaryIndependentVariable(IndependentVariableM* baseVariable)
{
    if (_temporaryIndependentVariable != nullptr)
    {
        if (baseVariable != nullptr)
        {
            _temporaryIndependentVariable->setname(baseVariable->name());
            _temporaryIndependentVariable->setdescription(baseVariable->description());
            _temporaryIndependentVariable->setvalueType(baseVariable->valueType());
            _temporaryIndependentVariable->setenumValues(baseVariable->enumValues());
        }
        else {
            _temporaryIndependentVariable->setname("");
            _temporaryIndependentVariable->setdescription("");
            _temporaryIndependentVariable->setvalueType(IndependentVariableValueTypes::UNKNOWN);
            _temporaryIndependentVariable->setenumValues({});
        }
    }
}


/**
 * @brief Initialize the temporary dependent variable with the given dependent variable
 * @param baseVariable, if null, init empty temporary dependent variable
 */
void ProtocolsController::initTemporaryDependentVariable(DependentVariableM* baseVariable)
{
    if (_temporaryDependentVariable != nullptr)
    {
        if (baseVariable != nullptr)
        {
            _temporaryDependentVariable->setname(baseVariable->name());
            _temporaryDependentVariable->setdescription(baseVariable->description());
            _temporaryDependentVariable->setagentName(baseVariable->agentName());
            _temporaryDependentVariable->setoutputName(baseVariable->outputName());
        }
        else {
            _temporaryDependentVariable->setname("");
            _temporaryDependentVariable->setdescription("");
            _temporaryDependentVariable->setagentName("");
            _temporaryDependentVariable->setoutputName("");
        }
    }
}
