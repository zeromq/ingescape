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

#include "dependentvariablem.h"

/**
 * @brief Constructor
 * @param parent
 */
DependentVariableM::DependentVariableM(CassUuid experimentationUuid
                                       , CassUuid taskUuid
                                       , CassUuid cassUuid
                                       , const QString& name
                                       , const QString& description
                                       , const QString& agentName
                                       , const QString& outputName
                                       , QObject *parent)
    : QObject(parent)
    , _name(name)
    , _description(description)
    , _agentName(agentName)
    , _outputName(outputName)
    , _experimentationCassUuid(experimentationUuid)
    , _taskCassUuid(taskUuid)
    , _cassUuid(cassUuid)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Model of Dependent Variable" << _name;
}


/**
 * @brief Destructor
 */
DependentVariableM::~DependentVariableM()
{
    qInfo() << "Delete Model of Dependent Variable" << _name;

}

DependentVariableM* DependentVariableM::createDependentVariableFromCassandraRow(const CassRow* row)
{
    DependentVariableM* dependentVariable = nullptr;

    if (row != nullptr)
    {
        CassUuid experimentationUuid, taskUuid, independentVarUuid;
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id_experimentation"), &experimentationUuid);
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id_task"), &taskUuid);
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id"), &independentVarUuid);

        const char *chrVariableName = "";
        size_t varNameLength = 0;
        cass_value_get_string(cass_row_get_column_by_name(row, "name"), &chrVariableName, &varNameLength);
        QString variableName = QString::fromUtf8(chrVariableName, static_cast<int>(varNameLength));

        const char *chrVariableDescription = "";
        size_t varDescriptionLength = 0;
        cass_value_get_string(cass_row_get_column_by_name(row, "description"), &chrVariableDescription, &varDescriptionLength);
        QString variableDescription(QString::fromUtf8(chrVariableDescription, static_cast<int>(varDescriptionLength)));

        const char *chrAgentName = "";
        size_t agentNameLength = 0;
        cass_value_get_string(cass_row_get_column_by_name(row, "agent_name"), &chrAgentName, &agentNameLength);
        QString agentName(QString::fromUtf8(chrAgentName, static_cast<int>(agentNameLength)));

        const char *chrOutputName = "";
        size_t outputNameLength = 0;
        cass_value_get_string(cass_row_get_column_by_name(row, "output_name"), &chrOutputName, &outputNameLength);
        QString outputName(QString::fromUtf8(chrOutputName, static_cast<int>(outputNameLength)));

        dependentVariable = new DependentVariableM(experimentationUuid, taskUuid, independentVarUuid, variableName, variableDescription, agentName, outputName);
    }

    return dependentVariable;
}
