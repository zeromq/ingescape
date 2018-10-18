/*
 *  I2Quick
 *
 *  Copyright (c) 2015-2017 Ingenuity i/o. All rights reserved.
 *
 *  See license terms for the rights and conditions
 *  defined by copyright holders.
 *
 *
 *  Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

#ifndef I2SETTINGS_H
#define I2SETTINGS_H

#include <QSettings>

// To export symbols
#include "i2quick_global.h"


/**
 * @brief The I2Settings class is a custom QSettings class that is used as a Meyer's singleton
 *        to share a single settings object in our files
 */
class I2QUICK_EXPORT I2Settings : public QSettings
{

public:

    /**
     * @brief Get our instance
     * @param settingsFilePath File path of our settings. It will only be use once to set up our instance
     * @return
     */
    static I2Settings& Instance(QString settingsFilePath = "");


private:
    /**
     * @brief Constructor
     * @param settingsFilePath File path of our settings
     */
    explicit I2Settings(QString settingsFilePath = "");


    /**
     * @brief Copy constructor
     */
    I2Settings(I2Settings const&);


    /**
     * @brief Assign operator
     * @return
     */
    I2Settings& operator=(I2Settings const&);
};

//QML_DECLARE_TYPE(I2Settings)

#endif // I2SETTINGS_H
