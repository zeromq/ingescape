/*
 *	IngeScape Editor
 *
 *  Copyright (c) 2016-2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Deliencourt  <deliencourt@ingenuity.io>
 *
 */

#include "ingescapeeditorsettings.h"

#include <QDebug>


/**
 * @brief Get our instance
 * @param settingsFilePath File path of our settings. It will only be use once to set up our instance
 * @return
 */
IngeScapeEditorSettings& IngeScapeEditorSettings::Instance(QString settingsFilePath)
{
    static IngeScapeEditorSettings instance(settingsFilePath);
    return instance;
}


/**
 * @brief Default constructor
 * @param settingsFilePath File path of our settings
 */
IngeScapeEditorSettings::IngeScapeEditorSettings(QString settingsFilePath) : QSettings(settingsFilePath, QSettings::IniFormat)
{
    if (!settingsFilePath.isEmpty())
    {
        qDebug() << "Nouveau Fichier de configuration:" << settingsFilePath;
    }
    else
    {
        qFatal("Erreur Fatale: La première instance de IngeScapeEditorSettings::Instance() doit pointer vers un chemin de fichier valide !");
    }
}


