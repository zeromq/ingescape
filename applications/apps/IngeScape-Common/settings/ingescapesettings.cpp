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
 *      Alexandre Lemort    <lemort@ingenuity.io>
 *      Vincent Peyruqueou  <peyruqueou@ingenuity.io>
 *      Chloé Roumieu       <roumieu@ingenuity.io>
 */

#include "ingescapesettings.h"

#include <QDebug>

/**
 * @brief Get our instance
 * @param settingsFilePath File path of our settings. It will only be use once to set up our instance
 * @return
 */
IngeScapeSettings& IngeScapeSettings::Instance(QString settingsFilePath)
{
    static IngeScapeSettings instance(settingsFilePath);
    return instance;
}


/**
 * @brief Constructor
 * @param settingsFilePath File path of our settings
 */
IngeScapeSettings::IngeScapeSettings(QString settingsFilePath) : QSettings(settingsFilePath, QSettings::IniFormat),
    _areDefaultSettings(false)
{
    if (!settingsFilePath.isEmpty())
    {
        qDebug() << "Nouveau Fichier de configuration:" << settingsFilePath;
    }
    else
    {
        qFatal("Erreur Fatale: La première instance de IngeScapeSettings::Instance() doit pointer vers un chemin de fichier valide !");
    }
}
