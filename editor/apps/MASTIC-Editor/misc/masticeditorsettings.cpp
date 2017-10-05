/*
 *	MASTIC Editor
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

#include "masticeditorsettings.h"

#include <QDebug>


/**
 * @brief Get our instance
 * @param settingsFilePath File path of our settings. It will only be use once to set up our instance
 * @return
 */
MasticEditorSettings& MasticEditorSettings::Instance(QString settingsFilePath)
{
    static MasticEditorSettings instance(settingsFilePath);
    return instance;
}


/**
 * @brief Default constructor
 * @param settingsFilePath File path of our settings
 */
MasticEditorSettings::MasticEditorSettings(QString settingsFilePath) : QSettings(settingsFilePath, QSettings::IniFormat)
{
    if (!settingsFilePath.isEmpty())
    {
        qDebug() << "Nouveau Fichier de configuration:" << settingsFilePath;
    }
    else
    {
        qFatal("Erreur Fatale: La première instance de MasticEditorSettings::Instance() doit pointer vers un chemin de fichier valide !");
    }
}


