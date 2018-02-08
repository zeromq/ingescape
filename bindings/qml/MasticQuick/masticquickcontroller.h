/*
 *  Mastic - QML binding
 *
 *  Copyright (c) 2018 Ingenuity i/o. All rights reserved.
 *
 *  See license terms for the rights and conditions
 *  defined by copyright holders.
 *
 *
 *  Contributors:
 *      Alexandre Lemort <lemort@ingenuity.io>
 *
 */

#ifndef _MASTICQUICKCONTROLLER_H_
#define _MASTICQUICKCONTROLLER_H_

#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>

#include "masticquick_helpers.h"
#include "masticquickqmlpropertymap.h"


/**
  * @brief Enum for IOP types
  */
MASTIC_QML_ENUM_CUSTOM(MasticIopType, INTEGER, DOUBLE, STRING, BOOLEAN, IMPULSION, DATA)


/**
 * @brief Enum for log levels
 */
MASTIC_QML_ENUM(MasticLogLevel, LOG_TRACE, LOG_DEBUG, LOG_INFO, LOG_WARNING, LOG_ERROR, LOG_FATAL)


/**
 * @brief The MasticQuickController class defines a QML singleton that provides the Mastic API to QML
 */
class MasticQuickController : public QObject
{
    Q_OBJECT

    // Agent name
    MASTIC_QML_PROPERTY_CUSTOM_SETTER(QString, agentName)

    // Agent state
    MASTIC_QML_PROPERTY_CUSTOM_SETTER(QString, agentState)

    // Definition name
    MASTIC_QML_PROPERTY_CUSTOM_SETTER(QString, definitionName)

    // Definition version
    MASTIC_QML_PROPERTY_CUSTOM_SETTER(QString, definitionVersion)

    // Definition description
    MASTIC_QML_PROPERTY_CUSTOM_SETTER(QString, definitionDescription)

    // Flag indicating if our agent is muted
    MASTIC_QML_PROPERTY_CUSTOM_SETTER(bool, isMuted)

    // Flag indicating if our agent is frozen
    MASTIC_QML_PROPERTY_CUSTOM_SETTER(bool, isFrozen)

    // Flag indicating if our agent can be frozen
    MASTIC_QML_PROPERTY_CUSTOM_SETTER(bool, canBeFrozen)

    // Flag indicating if our agent is started
    MASTIC_QML_PROPERTY_READONLY(bool, isStarted)

    // Flag indicating if Mastic is verbose or not
    MASTIC_QML_PROPERTY_CUSTOM_SETTER(bool, isVerbose)

    // Log level
    MASTIC_QML_PROPERTY_CUSTOM_SETTER(MasticLogLevel::Value, logLevel)

    // Inputs
    MASTIC_QML_PROPERTY_READONLY(MasticQuickQmlPropertyMap*, inputs)

    // Outputs
    MASTIC_QML_PROPERTY_READONLY(MasticQuickQmlPropertyMap*, outputs)

    // Parameters
    MASTIC_QML_PROPERTY_READONLY(MasticQuickQmlPropertyMap*, parameters)


public:
    /**
     * @brief Default constructor
     * @param parent
     */
    explicit MasticQuickController(QObject *parent = 0);


    /**
      * @brief Destructor
      */
    ~MasticQuickController();


    /**
     * @brief Method used to provide a singleton to QML
     * @param engine
     * @param scriptEngine
     * @return
     */
     static QObject* qmlSingleton(QQmlEngine* engine, QJSEngine* scriptEngine);


Q_SIGNALS:
     /**
     * @brief Triggered when our agent is asked to stop on the network
     */
    void forcedStop();



//
// Start or stop our agent
//
public Q_SLOTS:
     /**
      * @brief Start our agent with a given network device and port
      *
      * @param networkDevice
      * @param port
      */
     bool startWithDevice(QString networkDevice, int port = 5670);


     /**
      * @brief Start our agent with a given IP address and port
      *
      * @param ipAddress
      * @param port
      */
     bool startWithIP(QString ipAddress, int port = 5670);


     /**
      * @brief Stop our agent
      */
     bool stop();





//
// Create or remove IOP (input, output, parameter)
//
public Q_SLOTS:
     /**
      * @brief Create a new integer input
      *
      * @param name
      * @param value
      *
      * @return true if an input is created, false otherwise (i.e. we already have an input with this name)
      */
     bool createInputInteger(QString name, int value = 0);


     /**
      * @brief Create a new double input
      *
      * @param name
      * @param value
      *
      * @return true if an input is created, false otherwise (i.e. we already have an input with this name)
      */
     bool createInputDouble(QString name, double value = 0.0);


     /**
      * @brief Create a new string input
      *
      * @param name
      * @param value
      *
      * @return true if an input is created, false otherwise (i.e. we already have an input with this name)
      */
     bool createInputString(QString name, QString value = "");


     /**
      * @brief Create a new boolean input
      *
      * @param name
      * @param value
      *
      * @return true if an input is created, false otherwise (i.e. we already have an input with this name)
      */
     bool createInputBoolean(QString name, bool value = false);


     /**
      * @brief Create a new impulsion input
      *
      * @param name
      *
      * @return true if an input is created, false otherwise (i.e. we already have an input with this name)
      */
     bool createInputImpulsion(QString name);


     /**
      * @brief Create a new data input
      *
      * @param name
      * @param value
      *
      * @return true if an input is created, false otherwise (i.e. we already have an input with this name)
      */
     bool createInputData(QString name, void* value = NULL);


     /**
      * @brief Create a new integer output
      *
      * @param name
      * @param value
      *
      * @return true if an output is created, false otherwise (i.e. we already have an output with this name)
      */
     bool createOutputInteger(QString name, int value = 0);


     /**
      * @brief Create a new double output
      *
      * @param name
      * @param value
      *
      * @return true if an output is created, false otherwise (i.e. we already have an output with this name)
      */
     bool createOutputDouble(QString name, double value = 0.0);


     /**
      * @brief Create a new string output
      *
      * @param name
      * @param value
      *
      * @return true if an output is created, false otherwise (i.e. we already have an output with this name)
      */
     bool createOutputString(QString name, QString value = "");


     /**
      * @brief Create a new boolean output
      *
      * @param name
      * @param value
      *
      * @return true if an output is created, false otherwise (i.e. we already have an output with this name)
      */
     bool createOutputBoolean(QString name, bool value = false);


     /**
      * @brief Create a new impulsion output
      *
      * @param name
      *
      * @return true if an output is created, false otherwise (i.e. we already have an output with this name)
      */
     bool createOutputImpulsion(QString name);


     /**
      * @brief Create a new data output
      *
      * @param name
      * @param value
      *
      * @return true if an output is created, false otherwise (i.e. we already have an output with this name)
      */
     bool createOutputData(QString name, void* value = NULL);


     /**
      * @brief Create a new integer parameter
      *
      * @param name
      * @param value
      *
      * @return true if a parameter is created, false otherwise (i.e. we already have a parameter with this name)
      */
     bool createParameterInteger(QString name, int value = 0);


     /**
      * @brief Create a new double parameter
      *
      * @param name
      * @param value
      *
      * @return true if a parameter is created, false otherwise (i.e. we already have a parameter with this name)
      */
     bool createParameterDouble(QString name, double value = 0.0);


     /**
      * @brief Create a new string parameter
      *
      * @param name
      * @param value
      *
      * @return true if a parameter is created, false otherwise (i.e. we already have a parameter with this name)
      */
     bool createParameterString(QString name, QString value = "");


     /**
      * @brief Create a new boolean parameter
      *
      * @param name
      * @param value
      *
      * @return true if a parameter is created, false otherwise (i.e. we already have a parameter with this name)
      */
     bool createParameterBoolean(QString name, bool value = false);


     /**
      * @brief Create a new data parameter
      *
      * @param name
      * @param value
      *
      * @return true if a parameter is created, false otherwise (i.e. we already have a parameter with this name)
      */
     bool createParameterData(QString name, void* value = NULL);


     /**
      * @brief Remove an input
      *
      * @param name
      *
      * @return true if this input is removed, false otherwise
      */
     bool removeInput(QString name);


     /**
      * @brief Remove an output
      *
      * @param name
      *
      * @return true if this output is removed, false otherwise
      */
     bool removeOutput(QString name);


     /**
      * @brief Remove a parameter
      *
      * @param name
      *
      * @return true if this parameter is removed, false otherwise
      */
     bool removeParameter(QString name);



//
// Write outputs and parameters
//
public Q_SLOTS:
     /**
      * @brief Write a given output as an integer
      *
      * @param name
      * @param value
      *
      * @return  true if everything is ok, false otherwise
      */
     bool writeOutputAsInteger(QString name, int value);


     /**
      * @brief Write a given output as a double
      *
      * @param name
      * @param value
      *
      * @return  true if everything is ok, false otherwise
      */
     bool writeOutputAsDouble(QString name, double value);


     /**
      * @brief Write a given output as a string
      *
      * @param name
      * @param value
      *
      * @return  true if everything is ok, false otherwise
      */
     bool writeOutputAsString(QString name, QString value);


     /**
      * @brief Write a given output as a boolean
      *
      * @param name
      * @param value
      *
      * @return  true if everything is ok, false otherwise
      */
     bool writeOutputAsBoolean(QString name, bool value);


     /**
      * @brief Write a given output as an impulsion
      *
      * @param name
      *
      * @return  true if everything is ok, false otherwise
      */
     bool writeOutputAsImpulsion(QString name);


     /**
      * @brief Write a given output as data
      *
      * @param name
      * @param value
      *
      * @return  true if everything is ok, false otherwise
      */
     bool writeOutputAsData(QString name, void* value);


     /**
      * @brief Write a given parameter as an integer
      *
      * @param name
      * @param value
      *
      * @return  true if everything is ok, false otherwise
      */
     bool writeParameterAsInteger(QString name, int value);


     /**
      * @brief Write a given parameter as a double
      *
      * @param name
      * @param value
      *
      * @return  true if everything is ok, false otherwise
      */
     bool writeParameterAsDouble(QString name, double value);


     /**
      * @brief Write a given parameter as a string
      *
      * @param name
      * @param value
      *
      * @return  true if everything is ok, false otherwise
      */
     bool writeParameterAsString(QString name, QString value);


     /**
      * @brief Write a given parameter as a boolean
      *
      * @param name
      * @param value
      *
      * @return  true if everything is ok, false otherwise
      */
     bool writeParameterAsBoolean(QString name, bool value);


     /**
      * @brief Write a given parameter as data
      *
      * @param name
      * @param value
      *
      * @return  true if everything is ok, false otherwise
      */
     bool writeParameterAsData(QString name, void* value);



//
// Mute/unmuted outputs
//
public Q_SLOTS:
     /**
      * @brief Mute a given output
      *
      * @param name
      *
      * @return  true if everything is ok, false otherwise
      */
     bool muteOuput(QString name);


     /**
      * @brief Mute a given output
      *
      * @param name
      *
      * @return  true if everything is ok, false otherwise
      */
     bool unmuteOuput(QString name);


     /**
      * @brief Check if a given output is muted
      *
      * @param name
      * @param qmlUpdateExtraParameter Extra parameter used to call this function in a QML binding
      *
      * @return true if this output is muted, false otherwise
      */
     bool isOutputMuted(QString name, QVariant qmlUpdateExtraParameter = QVariant());


protected:
     /**
      * @brief Check if an IOP name is valid or if it can create conflicts with Qt internal symbols
      * @param name
      * @return
      */
     bool _validIopName(const QString& name);


     /**
      * @brief Create a new input
      *
      * @param name
      * @param type
      * @param qmlValue
      * @param cValue
      * @param cSize
      *
      * @return true if an input is created, false otherwise
      */
     bool _createInput(QString name, MasticIopType::Value type, QVariant qmlValue, void* cValue, long cSize);


     /**
      * @brief Create a new output
      *
      * @param name
      * @param type
      * @param qmlValue
      * @param cValue
      * @param cSize
      *
      * @return true if an output is created, false otherwise
      */
     bool _createOutput(QString name, MasticIopType::Value type, QVariant qmlValue, void* cValue, long cSize);


     /**
      * @brief Create a new paramater
      *
      * @param name
      * @param type
      * @param qmlValue
      * @param cValue
      * @param cSize
      *
      * @return true if a parameter is created, false otherwise
      */
     bool _createParameter(QString name, MasticIopType::Value type, QVariant qmlValue, void* cValue, long cSize);


     /**
      * @brief Update a QML output property
      *
      * @param name
      *
      * @return true if everything is ok, false otherwise
      */
     bool _updateQmlOutput(QString name);


     /**
      * @brief Update a QML parameter property
      *
      * @param name
      *
      * @return true if everything is ok, false otherwise
      */
     bool _updateQmlParameter(QString name);



protected Q_SLOTS:
     /**
      * @brief Called when our agent is asked to stop on the network
      */
     void _onForcedStop();


     /**
      * @brief Called when an output is updated from QML
      * @param key
      * @param value
      */
     void _onOutputUpdatedFromFromQML(const QString &key, const QVariant &value);


     /**
      * @brief Called when a parameter is updated from QML
      * @param key
      * @param value
      */
     void _onParameterUpdatedFromFromQML(const QString &key, const QVariant &value);
};

QML_DECLARE_TYPE(MasticQuickController)

#endif // _MASTICQUICKCONTROLLER_H_
