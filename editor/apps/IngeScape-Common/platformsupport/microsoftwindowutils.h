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

#ifndef MICROSOFTWINDOWUTILS_H
#define MICROSOFTWINDOWUTILS_H

#include <QtGlobal>
#include <QObject>


#ifndef Q_OS_WIN
//
// Non-Window definition
//
class MicrosoftWindowUtils : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     */
    explicit MicrosoftWindowUtils(QObject *parent = nullptr);

};


#else
//
// Microsoft window definition
//

/**
 * @brief The MicrosoftWindowUtils class defines a set of utility functions to perform Windows specific operations
 */
class MicrosoftWindowUtils : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     */
    explicit MicrosoftWindowUtils(QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~MicrosoftWindowUtils();


    /**
     * @brief Get our singleton instance
     * @return
     */
    static MicrosoftWindowUtils& instance();


    /**
     * @brief Init
     */
    void init();


    /**
     * @brief Clean
     */
    void clean();


    /**
     * @brief Enable or disable energy efficiency features
     *
     * @param value
     */
    void energyEfficiencyFeaturesEnabled(bool value);


Q_SIGNALS:
    /**
     * @brief Triggered when our machine will go to sleep
     */
    void systemSleep();


    /**
     * @ brief Triggered when our machine did wake from sleep
     */
    void systemWake();
};
#endif


#endif // MICROSOFTWINDOWUTILS_H
