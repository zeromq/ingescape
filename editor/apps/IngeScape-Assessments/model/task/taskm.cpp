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

#include "taskm.h"

/**
 * @brief Constructor
 * @param name
 * @param parent
 */
TaskM::TaskM(QString name,
             QObject *parent) : QObject(parent),
    _name(name),
    _platformFileUrl(QUrl())
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Model of Task" << _name;

    //
    // FIXME for tests
    //
    for (int i = 1; i < 6; i++)
    {
        IndependentVariableM* independentVariable = new IndependentVariableM(QString("VI %1").arg(i), QString("description of VI %1").arg(i), IndependentVariableValueTypes::DOUBLE);

        _independentVariables.append(independentVariable);


        DependentVariableM* dependentVariable = new DependentVariableM();
        dependentVariable->setname(QString("VD %1").arg(i));
        dependentVariable->setdescription(QString("description of VD %1").arg(i));
        dependentVariable->setagentName("Leap Motion");
        dependentVariable->setoutputName(QString("Finger %1").arg(i));

        _dependentVariables.append(dependentVariable);
    }
}


/**
 * @brief Destructor
 */
TaskM::~TaskM()
{
    qInfo() << "Delete Model of Task" << _name;

}


/**
 * @brief Setter for property "Platform File Url"
 * @param value
 */
void TaskM::setplatformFileUrl(QUrl value)
{
    if (_platformFileUrl != value)
    {
        _platformFileUrl = value;

        // Update file name
        if (_platformFileUrl.isValid()) {
            setplatformFileName(_platformFileUrl.fileName());

            // Update the list of dependent variables
            _updateDependentVariables();
        }
        else {
            setplatformFileName("");
        }

        Q_EMIT platformFileUrlChanged(value);
    }
}


/**
 * @brief Return true if the user can create an independent variable with the name
 * Check if the name is not empty and if a independent variable with the same name does not already exist
 * @param independentVariableName
 * @return
 */
bool TaskM::canCreateIndependentVariableWithName(QString independentVariableName)
{
    if (!independentVariableName.isEmpty())
    {
        for (IndependentVariableM* independentVariable : _independentVariables.toList())
        {
            if ((independentVariable != nullptr) && (independentVariable->name() == independentVariableName))
            {
                return false;
            }
        }
        return true;
    }
    else {
        return false;
    }
}


/**
 * @brief Create a new independent variable
 * @param independentVariableName
 * @param independentVariableDescription
 * @param nIndependentVariableValueType
 */
void TaskM::createNewIndependentVariable(QString independentVariableName, QString independentVariableDescription, int nIndependentVariableValueType)
{
    if (!independentVariableName.isEmpty() && (nIndependentVariableValueType > -1))
    {
        IndependentVariableValueTypes::Value independentVariableValueType = static_cast<IndependentVariableValueTypes::Value>(nIndependentVariableValueType);

        qInfo() << "Create new independent variable" << independentVariableName << "of type" << IndependentVariableValueTypes::staticEnumToString(independentVariableValueType);

        // Create the new independent variable
        IndependentVariableM* independentVariable = new IndependentVariableM(independentVariableName, independentVariableDescription, independentVariableValueType);

        // Add the independent variable to the list
        _independentVariables.append(independentVariable);
    }
}


/**
 * @brief Create a new independent variable of type enum
 * @param independentVariableName
 * @param independentVariableDescription
 * @param enumValues
 */
void TaskM::createNewIndependentVariableEnum(QString independentVariableName, QString independentVariableDescription, QStringList enumValues)
{
    if (!independentVariableName.isEmpty() && !enumValues.isEmpty())
    {
        qInfo() << "Create new independent variable" << independentVariableName << "of type" << IndependentVariableValueTypes::staticEnumToString(IndependentVariableValueTypes::INDEPENDENT_VARIABLE_ENUM) << "with values:" << enumValues;

        // Create the new independent variable
        IndependentVariableM* independentVariable = new IndependentVariableM(independentVariableName, independentVariableDescription, IndependentVariableValueTypes::INDEPENDENT_VARIABLE_ENUM);
        independentVariable->setenumValues(enumValues);

        // Add the independent variable to the list
        _independentVariables.append(independentVariable);
    }
}


/**
 * @brief Update the list of dependent variables
 */
void TaskM::_updateDependentVariables()
{
    if (_platformFileUrl.isValid())
    {
        // FIXME TODO: _updateDependentVariables
        qDebug() << "TODO _updateDependentVariables...";
    }
}
