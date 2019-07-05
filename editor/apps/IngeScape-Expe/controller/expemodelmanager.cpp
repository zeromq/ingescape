/*
 *	IngeScape Expe
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

#include "expemodelmanager.h"

/**
 * @brief Constructor
 * @param jsonHelper
 * @param rootDirectoryPath
 * @param parent
 */
ExpeModelManager::ExpeModelManager(JsonHelper* jsonHelper,
                                   QString rootDirectoryPath,
                                   QObject *parent) : IngeScapeModelManager(jsonHelper,
                                                                            rootDirectoryPath,
                                                                            parent)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New IngeScape Expe Model Manager";

}


/**
 * @brief Destructor
 */
ExpeModelManager::~ExpeModelManager()
{
    qInfo() << "Delete IngeScape Expe Model Manager";


    // Mother class is automatically called
    //IngeScapeModelManager::~IngeScapeModelManager();
}
