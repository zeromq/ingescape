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
 *
 */

#ifndef INGESCAPESETTINGS_H
#define INGESCAPESETTINGS_H

#include <QObject>
#include <QSettings>
#include <I2PropertyHelpers.h>


/**
 * @brief The IngeScapeSettings class is a custom QSettings class that is used as a Meyer's singleton
 *        to share a single settings object in our files
 */
class IngeScapeSettings : public QSettings
{

public:

    /**
     * @brief Get our instance
     * @param settingsFilePath File path of our settings. It will only be use once to set up our instance
     * @return
     */
    static IngeScapeSettings& Instance(QString settingsFilePath = "");


private:
    /**
     * @brief Default constructor
     * @param settingsFilePath File path of our settings
     */
    explicit IngeScapeSettings(QString settingsFilePath = "");


    /**
     * @brief Copy constructor
     */
    IngeScapeSettings(IngeScapeSettings const&);


    /**
     * @brief Assign operator
     * @return
     */
    IngeScapeSettings& operator=(IngeScapeSettings const&);

};

QML_DECLARE_TYPE(IngeScapeSettings)

#endif // INGESCAPESETTINGS_H
