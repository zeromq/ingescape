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
        IndependentVariableM* independentVariable = new IndependentVariableM();
        independentVariable->setname(QString("VI %1").arg(i));
        independentVariable->setdescription(QString("description of VI %1").arg(i));
        independentVariable->setvalueType(IndependentVariableValueTypes::DOUBLE);

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
