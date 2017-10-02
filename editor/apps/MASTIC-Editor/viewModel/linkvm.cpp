/*
 *	MASTIC Editor
 *
 *  Copyright © 2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Alexandre Lemort   <lemort@ingenuity.io>
 *
 */

#include "linkvm.h"


#include <QQmlEngine>
#include <QDebug>


/**
 * @brief Default constructor
 * @param parent
 */
LinkVM::LinkVM(QObject *parent) : QObject(parent),
    _outputSlotVM(NULL),
    _inputSlotVM(NULL)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);


    qInfo() << "Link created";
}



/**
 * @brief Destructor
 */
LinkVM::~LinkVM()
{
    qInfo() << "Link destroyed";
}
