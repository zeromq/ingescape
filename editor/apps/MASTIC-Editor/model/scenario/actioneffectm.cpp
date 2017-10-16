/*
 *	ActionEffectM
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

#include "actioneffectm.h"


#include <QDebug>



//--------------------------------------------------------------
//
// ActionEffectM
//
//--------------------------------------------------------------


/**
 * @brief Default constructor
 * @param parent
 */
ActionEffectM::ActionEffectM(QObject *parent) : QObject(parent),
    _model(NULL),
    _effect(ActionEffectType::ON)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

}


/**
 * @brief Destructor
 */
ActionEffectM::~ActionEffectM()
{
    // Reset agent model to null
    setmodel(NULL);
}


