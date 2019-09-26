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


#ifndef OSUTILS_H
#define OSUTILS_H

#include <QObject>
#include <QNetworkConfigurationManager>

#ifndef OSUTILS_NO_QML

#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>
#include <QQuickWindow>

#endif




/**
 * @brief The OSUtils class defines a set of utility functions to perform OS specific operations
 */
class OSUtils : public QObject
{
    Q_OBJECT

    // Current window
    Q_PROPERTY(QQuickWindow* currentWindow READ currentWindow WRITE setcurrentWindow NOTIFY currentWindowChanged)

    // Flag indicating if our system is awake
    Q_PROPERTY(bool isAwake READ isAwake NOTIFY isAwakeChanged)

    // Flag indicating if our user session is locked
    Q_PROPERTY(bool isUserSessionLocked READ isUserSessionLocked NOTIFY isUserSessionLockedChanged)

    // Flag indicating if our system has energy efficency features (Macos App Nap, etc.)
    Q_PROPERTY(bool hasEnergyEfficiencyFeatures READ hasEnergyEfficiencyFeatures NOTIFY hasEnergyEfficiencyFeaturesChanged)

    // Flag indicating if we want to prevent energy efficency features (Macos App Nap, etc.)
    Q_PROPERTY(bool preventEnergyEfficiencyFeatures READ preventEnergyEfficiencyFeatures WRITE setpreventEnergyEfficiencyFeatures NOTIFY preventEnergyEfficiencyFeaturesChanged)


public:
    /**
     * @brief Constructor
     * @param parent
     */
    explicit OSUtils(QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~OSUtils();


    /**
     * @brief Get our singleton instance
     * @return
     */
    static OSUtils* instance();


    /**
     * @brief Method used to provide a singleton to QML
     * @param engine
     * @param scriptEngine
     * @return
     */
     static QObject* qmlSingleton(QQmlEngine* engine, QJSEngine* scriptEngine);



#ifndef OSUTILS_NO_QML
     /**
     * @brief get our current window
     * @return
     */
     QQuickWindow* currentWindow() const
     {
        return _currentWindow;
     }


     /**
      * @brief Set our current window
      * @param value
      * @return
      */
     virtual bool setcurrentWindow (QQuickWindow* value) {
         bool hasChanged = false;
         if (_currentWindow != value)
         {
             if (_currentWindow != nullptr)
             {
                 disconnect(_currentWindow, &QQuickWindow::destroyed, this, nullptr);
             }
             _currentWindow = value;
             if (_currentWindow != nullptr)
             {
                 connect(_currentWindow,  &QQuickWindow::destroyed, this, &OSUtils::_oncurrentWindowDestroyed);
             }
             hasChanged = true;
             Q_EMIT currentWindowChanged(value);
         }
         return hasChanged;
     }
#endif


     /**
      * @brief isAwake
      * @return
      */
     bool isAwake() const
     {
         return _isAwake;
     }


     /**
      * @brief setisAwake
      * @param value
      * @return
      */
     virtual bool setisAwake(bool value)
     {
         bool result = false;

         if (_isAwake != value)
         {
             _isAwake = value;
             result = true;
             Q_EMIT isAwakeChanged(value);
         }

         return result;
     }


     /**
      * @brief isAwake
      * @return
      */
     bool isUserSessionLocked() const
     {
         return _isUserSessionLocked;
     }


     /**
      * @brief setisUserSessionLocked
      * @param value
      * @return
      */
     virtual bool setisUserSessionLocked(bool value)
     {
         bool result = false;

         if (_isUserSessionLocked != value)
         {
             _isUserSessionLocked = value;
             result = true;
             Q_EMIT isUserSessionLockedChanged(value);
         }

         return result;
     }


     /**
      * @brief hasEnergyEfficiencyFeatures
      * @return
      */
     bool hasEnergyEfficiencyFeatures() const
     {
         return _hasEnergyEfficiencyFeatures;
     }


     /**
      * @brief sethasEnergyEfficiencyFeatures
      * @param value
      * @return
      */
     virtual bool sethasEnergyEfficiencyFeatures(bool value)
     {
         bool result = false;

         if (_hasEnergyEfficiencyFeatures != value)
         {
             _hasEnergyEfficiencyFeatures = value;
             result = true;
             Q_EMIT hasEnergyEfficiencyFeaturesChanged(value);
         }

         return result;
     }


     /**
      * @brief preventEnergyEfficiencyFeatures
      * @return
      */
     bool preventEnergyEfficiencyFeatures() const
     {
         return _preventEnergyEfficiencyFeatures;
     }


     /**
      * @brief setpreventEnergyEfficiencyFeatures
      * @param value
      * @return
      */
     virtual bool setpreventEnergyEfficiencyFeatures(bool value);


     /**
      * @brief Clean-up our menu
      */
     virtual void removeOSGeneratedMenuItems();


Q_SIGNALS:
#ifndef OSUTILS_NO_QML
     /**
      * @brief Triggered when our currentWindow property has changed
      * @param value
      */
     void currentWindowChanged(QQuickWindow* value);
#endif

     /**
      * @brief Triggered when our isAwake property has changed
      * @param value
      */
     void isAwakeChanged(bool value);


     /**
      * @brief Triggered when our isUserSessionLocked property has changed
      * @param value
      */
     void isUserSessionLockedChanged(bool value);


     /**
      * @brief Triggered when our hasEnergyEfficiencyFeatures property has changed
      * @param value
      */
     void hasEnergyEfficiencyFeaturesChanged(bool value);


     /**
      * @brief Triggered when our preventEnergyEfficiencyFeatures property has changed
      * @param value
      */
     void preventEnergyEfficiencyFeaturesChanged(bool value);


     /**
      * @brief Triggered when our machine will go to sleep
      */
     void systemSleep();


     /**
      * @ brief Triggered when our machine did wake from sleep
      */
     void systemWake();


     /**
      * @brief Triggered when our user session is locked
      */
     void userSessionLocked();


     /**
      * @brief Triggered when our user session is unlocked
      */
     void userSessionUnlocked();


     /**
      * @brief Triggered when a network configuration is added, removed or changed
      */
     void systemNetworkConfigurationsUpdated();


private Q_SLOTS:
#ifndef OSUTILS_NO_QML
     /**
     * @brief Called when our current window is destroyed
     */
    void _oncurrentWindowDestroyed(QObject*)
    {
        _currentWindow = nullptr;
        Q_EMIT currentWindowChanged(nullptr);
    }
#endif

protected:
    /**
     * @brief Called when we receive a systemSleep signal
     */
    void _onSystemSleep();


    /**
     * @brief Called when we receive a systemWake signal
     */
    void _onSystemWake();


    /**
     * @brief Called when we receive a userSessionLocked signal
     */
    void _onUserSessionLocked();


    /**
     * @brief Called when we receive a userSessionUnlocked signal
     */
    void _onUserSessionUnlocked();


    /**
     * @brief Called when a configuration is added to the system
     * @param config
     */
    void _onConfigurationAdded(const QNetworkConfiguration &config);


    /**
     * @brief Called when a configuration is about to be removed from the system
     * @param config
     */
    void _onConfigurationRemoved(const QNetworkConfiguration &config);


    /**
     * @brief Called when a configuration has changed (ex: Wifi on <=> off)
     * @param config
     */
    void _onConfigurationChanged(const QNetworkConfiguration &config);


protected:
    /**
     * @brief Enable energy efficiency features
     */
    virtual void _enableEnergyEfficiencyFeatures();


    /**
     * @brief Disable energy efficiency features
     */
    virtual void _disableEnergyEfficiencyFeatures();


protected:
#ifndef OSUTILS_NO_QML
    // Current windiw
    QQuickWindow* _currentWindow;
#endif

    // Flag indicating if our system is awake
    bool _isAwake;

    // Flag indicating if our user session is locked
    bool _isUserSessionLocked;

    // Flag indicating if our system has energy efficency features (Macos App Nap, etc.)
    bool _hasEnergyEfficiencyFeatures;

    // Flag indicating if we want to prevent energy efficency features (Macos App Nap, etc.)
    bool _preventEnergyEfficiencyFeatures;

    // Used to manages the network configurations provided by the system
    QNetworkConfigurationManager _networkConfigurationManager;
};


#ifndef OSUTILS_NO_QML
QML_DECLARE_TYPE(OSUtils)
#endif

#endif // OSUTILS_H
