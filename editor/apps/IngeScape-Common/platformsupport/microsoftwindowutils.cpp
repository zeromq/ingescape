/*
 *	IngeScape Common
 *
 *  Copyright © 2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Alexandre Lemort   <lemort@ingenuity.io>
 *
 */

#include "microsoftwindowutils.h"


#ifndef Q_OS_WIN

//---------------------------------------------------------------------
//
//  Fake API to avoid compiler errors (empty source file)
//
//---------------------------------------------------------------------


/**
 * @brief Constructor
 * @param parent
 */
MicrosoftWindowUtils::MicrosoftWindowUtils(QObject *parent)
    : QObject(parent)
{
}



#else

//---------------------------------------------------------------------
//
//
//  Microsoft Window API
//
//
//---------------------------------------------------------------------


/**
 * @brief Constructor
 * @param parent
 */
MicrosoftWindowUtils::MicrosoftWindowUtils(QObject *parent)
    : QObject(parent)
{
}


/**
 * @brief Destructor
 */
MicrosoftWindowUtils::~MicrosoftWindowUtils()
{
}


/**
 * @brief Get our singleton instance
 * @return
 */
MicrosoftWindowUtils& MicrosoftWindowUtils::instance()
{
    static MicrosoftWindowUtils instance;
    return instance;
}


/**
 * @brief Init
 */
void MicrosoftWindowUtils::init()
{
}


/**
 * @brief Clean
 */
void MicrosoftWindowUtils::clean()
{
}


/**
 * @brief Enable or disable energy efficiency features
 *
 * @param value
 */
void MicrosoftWindowUtils::energyEfficiencyFeaturesEnabled(bool value)
{
    Q_UNUSED(value)
}


#endif
