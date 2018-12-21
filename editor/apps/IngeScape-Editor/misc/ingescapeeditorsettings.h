/*
 *	IngeScape Editor
 *
 *  Copyright © 2017-2018 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou  <peyruqueou@ingenuity.io>
 *
 */

#ifndef INGESCAPEEDITORSETTINGS_H
#define INGESCAPEEDITORSETTINGS_H

#include <QObject>
#include <QtQml>
#include <QSettings>

#include <I2PropertyHelpers.h>



/**
 * @brief The IngeScapeEditorSettings class is a custom QSettings class that is used as a Meyer's singleton
 *        to share a single settings object in our files
 */
class IngeScapeEditorSettings: public QSettings
{

public:

    /**
     * @brief Get our instance
     * @param settingsFilePath File path of our settings. It will only be use once to set up our instance
     * @return
     */
    static IngeScapeEditorSettings& Instance(QString settingsFilePath = "");


private:

    /**
     * @brief Default constructor
     * @param settingsFilePath File path of our settings
     */
    explicit IngeScapeEditorSettings(QString settingsFilePath = "");


    /**
     * @brief Copy constructor
     */
    IngeScapeEditorSettings(IngeScapeEditorSettings const&);


    /**
     * @brief Assign operator
     * @return
     */
    IngeScapeEditorSettings& operator=(IngeScapeEditorSettings const&);

};

QML_DECLARE_TYPE(IngeScapeEditorSettings)

#endif // INGESCAPEEDITORSETTINGS_H
