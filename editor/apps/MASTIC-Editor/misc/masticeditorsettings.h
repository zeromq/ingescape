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

#ifndef MASTICEDITORSETTINGS_H
#define MASTICEDITORSETTINGS_H

#include <QObject>
#include <QtQml>
#include <QSettings>

#include <I2PropertyHelpers.h>



/**
 * @brief The MasticEditorSettings class is a custom QSettings class that is used as a Meyer's singleton
 *        to share a single settings object in our files
 */
class MasticEditorSettings: public QSettings
{

public:

    /**
     * @brief Get our instance
     * @param settingsFilePath File path of our settings. It will only be use once to set up our instance
     * @return
     */
    static MasticEditorSettings& Instance(QString settingsFilePath = "");


private:

    /**
     * @brief Default constructor
     * @param settingsFilePath File path of our settings
     */
    explicit MasticEditorSettings(QString settingsFilePath = "");


    /**
     * @brief Copy constructor
     */
    MasticEditorSettings(MasticEditorSettings const&);


    /**
     * @brief Assign operator
     * @return
     */
    MasticEditorSettings& operator=(MasticEditorSettings const&);

};

QML_DECLARE_TYPE(MasticEditorSettings)

#endif // MASTICEDITORSETTINGS_H
