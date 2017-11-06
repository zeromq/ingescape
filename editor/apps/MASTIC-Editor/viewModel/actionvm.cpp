/*
 *	ActionVM
 *
 *  Copyright (c) 2016-2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *
 */

#include "actionvm.h"


#include <QDebug>



//--------------------------------------------------------------
//
// ActionVM
//
//--------------------------------------------------------------


/**
 * @brief Default constructor
 * @param parent
 */
ActionVM::ActionVM(ActionM *actionModel, QObject *parent) : QObject(parent),
    _actionModel(actionModel),
    _actionsPanelIndex(-1),
    _lineInTimeLine(-1),
    _startDateTime(QDateTime::currentDateTime()),
    _revertAfterTime(false),
    _revertAtTime(false)
{
    if(_actionModel != NULL)
    {
        if(_actionModel->startTime() >= 0)
        {
            _startDateTime.setTime(QTime::fromMSecsSinceStartOfDay(_actionModel->startTime()*1000));
        } else {
             _startDateTime = QDateTime::fromString("00:00:00","HH:mm:ss");
        }
        if(_actionModel->revertWhenValidityIsOver() == true)
        {
            setrevertAfterTime(true);
        } else if(_actionModel->revertAtTime() > -1)
        {
            setrevertAtTime(true);
        }
    } else {
        _startDateTime = QDateTime::fromString("00:00:00","HH:mm:ss");
        _actionModel = NULL;
    }

    _startTimeString = _startDateTime.toString("HH:mm:ss");
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

}


/**
 * @brief Destructor
 */
ActionVM::~ActionVM()
{
    if(_actionModel != NULL)
    {
        delete _actionModel;
        _actionModel = NULL;
    }
}

/**
 * @brief Copy from another action view model
 * @param action VM to copy
 */
void ActionVM::copyFrom(ActionVM* actionVM)
{
    if(actionVM != NULL)
    {
        ActionM* originalModel =  actionVM->actionModel();

        // Copy the model
        if(originalModel != NULL)
        {
            ActionM* model = new ActionM(originalModel->name());
            model->copyFrom(originalModel);

            setactionModel(model);
        } else {
            setactionModel(NULL);
        }

        // Copy the view model attributes
        setcolor(actionVM->color());
        setactionsPanelIndex(actionVM->actionsPanelIndex());
        setlineInTimeLine(actionVM->lineInTimeLine());
        setstartTimeString(actionVM->startTimeString());
        setrevertAfterTime(actionVM->revertAfterTime());
        setrevertAtTime(actionVM->revertAtTime());
    }
}

/**
 * @brief Set the date time in string format
 * @param action VM to copy
 */
void ActionVM::setstartTimeString(QString stringDateTime)
{
    if(_startTimeString.compare(stringDateTime) != 0)
    {
        _startTimeString = stringDateTime;

        // Update the start date time
        if(stringDateTime.isEmpty() == false)
        {
            QDateTime dateTime = QDateTime::fromString(stringDateTime,"HH:mm:ss");
            setstartDateTime(dateTime);
        } else {
            QDateTime dateTime = QDateTime::fromString("00:00:00","HH:mm:ss");
            setstartDateTime(dateTime);
        }

        emit startTimeStringChanged(stringDateTime);
    }
}


/**
 * @brief Set the model start time
 * @param action VM to copy
 */
void ActionVM::setstartDateTime(QDateTime dateTime)
{
    if(_startDateTime != dateTime)
    {
        _startDateTime = dateTime;

        // Update action model start time in seconds
        if(_actionModel != NULL)
        {
            QTime time = _startDateTime.time();
            _actionModel->setstartTime(time.second()+time.minute()*60+time.hour()*60*60);
        }

        emit startDateTimeChanged(dateTime);
    }
}

/**
 * @brief Set the revertAfterTime flag
 * @param revertAfterTime value
 */
void ActionVM::setrevertAfterTime(bool revertAfterTime)
{
    if(_revertAfterTime != revertAfterTime)
    {
        _revertAfterTime = revertAfterTime;

        // Update action model
        if(_actionModel != NULL)
        {
            _actionModel->setrevertWhenValidityIsOver(_revertAfterTime);
        }

        if(_revertAtTime == true && _revertAfterTime == true)
        {
            setrevertAtTime(false);
        }

        emit revertAfterTimeChanged(revertAfterTime);
    }
}

/**
 * @brief Set the revertAtTime flag
 * @param revertAtTime value
 */
void ActionVM::setrevertAtTime(bool revertAtTime)
{
    if(_revertAtTime != revertAtTime)
    {
        _revertAtTime = revertAtTime;

        // Update action model
        if(_actionModel != NULL)
        {
            _actionModel->setrevertWhenValidityIsOver(!_revertAtTime);
        }

        if(_revertAtTime == true && _revertAfterTime == true)
        {
            setrevertAfterTime(false);
        }

        emit revertAtTimeChanged(revertAtTime);
    }
}



