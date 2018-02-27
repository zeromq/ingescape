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

#ifndef _MASTICQUICK_H_
#define _MASTICQUICK_H_

#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>
#include <QQmlPropertyMap>

#include "masticquick_global.h"
#include "masticquick_helpers.h"
#include "masticquick_enums.h"


//
// URI associated to our QML plugin (used in import)
//
#define DEFAULT_MASTICQUICK_URI "MasticQuick"






//-------------------------------------------------------------------
//
//
//  MasticQuick
//
//
//-------------------------------------------------------------------


/**
 * @brief The MasticQuick class defines a QML singleton that provides the Mastic API to QML
 */
class MASTICQUICK_EXPORT MasticQuick : public QObject
{
    Q_OBJECT

    // Version of the Mastic API
    MASTIC_QML_PROPERTY_READONLY(int, version)

    // Version of the Mastic API - pretty print string
    MASTIC_QML_PROPERTY_READONLY(QString, versionString)

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

    // When mapping an agent setting we may request the mapped agent
    // to send its outputs (except for data & impulsions) to us through
    // a private communication for our proper initialization
    MASTIC_QML_PROPERTY_CUSTOM_SETTER(bool, requestOutputsFromMappedAgents)

    // Flag indicating if Mastic is verbose or not
    MASTIC_QML_PROPERTY_CUSTOM_SETTER(bool, isVerbose)

    // Flag indicating if Mastic must log stream
    MASTIC_QML_PROPERTY_CUSTOM_SETTER(bool, logStream)

    // Flag indicating if Mastic uses a log file
    MASTIC_QML_PROPERTY_CUSTOM_SETTER(bool, logInFile)

    // Flag indicating if Mastic uses colors in console
    MASTIC_QML_PROPERTY_CUSTOM_SETTER(bool, useColorVerbose)

    // Path of our log file
    MASTIC_QML_PROPERTY_CUSTOM_SETTER(QString, logPath)

    // Log level
    MASTIC_QML_PROPERTY_CUSTOM_SETTER(MasticLogLevel::Value, logLevel)

    // List of input names
    MASTIC_QML_PROPERTY_READONLY(QStringList, inputsList)

    // List of output names
    MASTIC_QML_PROPERTY_READONLY(QStringList, outputsList)

    // List of parameter names
    MASTIC_QML_PROPERTY_READONLY(QStringList, parametersList)

    // Inputs
    MASTIC_QML_PROPERTY_READONLY(QQmlPropertyMap*, inputs)

    // Outputs
    MASTIC_QML_PROPERTY_READONLY(QQmlPropertyMap*, outputs)

    // Parameters
    MASTIC_QML_PROPERTY_READONLY(QQmlPropertyMap*, parameters)



protected:
    /**
     * @brief Default constructor
     * @param parent
     */
    explicit MasticQuick(QObject *parent = 0);



public:
    /**
      * @brief Destructor
      */
    ~MasticQuick();



    /**
     * @brief Get our singleton instance
     * @return
     *
     * @remark Our singleton is owned by the QML engine.
     *         Thus, it is unsafe to keep a reference that points to it if you don't subscribe to is destroyed() signal
     */
    static MasticQuick* instance();



    /**
     * @brief Method used to provide a singleton to QML
     * @param engine
     * @param scriptEngine
     * @return
     */
     static QObject* qmlSingleton(QQmlEngine* engine, QJSEngine* scriptEngine);



     /**
      * @brief Register new QML types
      *
      * @param uri
      *
      * @remark You MUST call this method if you don't use MasticQuick as a QML plugin
      */
     static void registerTypes(const char *uri = DEFAULT_MASTICQUICK_URI);



Q_SIGNALS:
    /**
     * @brief Triggered when our agent is asked to stop on the network
     */
    void forcedStop();



    /**
     * @brief Triggered when a Mastic input changes
     * @param name
     * @param value
     */
    void observeInput(QString name, QVariant value);



    /**
     * @brief Triggered when a Mastic ouput changes
     * @param name
     * @param value
     */
    void observeOutput(QString name, QVariant value);



    /**
     * @brief Triggered when a Mastic parameter changes
     * @param name
     * @param value
     */
    void observeParameter(QString name, QVariant value);



    /**
     * @brief Triggered when our definition is cleared
     */
    void definitionCleared();



//---------------------------------------------------
//
// Start or stop our agent
//
//---------------------------------------------------
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



//---------------------------------------------------
//
// Read per type
//
//---------------------------------------------------
public Q_SLOTS:




//---------------------------------------------------
//
// Write per type
//
//---------------------------------------------------
public Q_SLOTS:

     /**
      * @brief Write a given output as an integer
      *
      * @param name
      * @param value
      *
      * @return  true if everything is ok, false otherwise
      */
     bool writeOutputAsInt(QString name, int value);



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
     bool writeOutputAsBool(QString name, bool value);



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
     bool writeParameterAsInt(QString name, int value);



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
     bool writeParameterAsBool(QString name, bool value);



     /**
      * @brief Write a given parameter as data
      *
      * @param name
      * @param value
      *
      * @return  true if everything is ok, false otherwise
      */
     bool writeParameterAsData(QString name, void* value);



//---------------------------------------------------
//
// Get IOP type and check existence
//
//---------------------------------------------------
public Q_SLOTS:
     /**
      * @brief Get type of a given input
      * @param name
      * @return
      */
     MasticIopType::Value getTypeForInput(QString name);



     /**
      * @brief Get type of a given output
      * @param name
      * @return
      */
     MasticIopType::Value getTypeForOutput(QString name);



     /**
      * @brief Get type of a given parameter
      * @param name
      * @return
      */
     MasticIopType::Value getTypeForParameter(QString name);



     /**
      * @brief Check if our agent has an input with this name
      * @param name
      * @return
      */
     bool checkInputExistence(QString name);



     /**
      * @brief Check if our agent has an output with this name
      * @param name
      * @return
      */
     bool checkOutputExistence(QString name);



     /**
      * @brief Check if our agent has a parameter with this name
      * @param name
      * @return
      */
     bool checkParameterExistence(QString name);



//---------------------------------------------------
//
// Mute/unmuted outputs
//
//---------------------------------------------------
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



//---------------------------------------------------
//
// Load / set / get definition
//
//---------------------------------------------------
public Q_SLOTS:
    /**
     * @brief Clear our definition (i.e. remove inputs, outputs, etc.)
     * @return
     */
    bool clearDefinition();



//---------------------------------------------------
//
// Edit the definition: create or remove IOP (input, output, parameter)
//
//---------------------------------------------------
public Q_SLOTS:
     /**
      * @brief Create a new integer input
      *
      * @param name
      * @param value
      * @param warning Warning message if something went wrong
      *
      * @return true if an input is created or already exists with the same type, false otherwise
      */
     bool createInputInt(QString name, int value = 0, QString* warning = NULL);



     /**
      * @brief Create a new double input
      *
      * @param name
      * @param value
      * @param warning Warning message if something went wrong
      *
      * @return true if an input is created or already exists with the same type, false otherwise
      */
     bool createInputDouble(QString name, double value = 0.0, QString* warning = NULL);



     /**
      * @brief Create a new string input
      *
      * @param name
      * @param value
      * @param warning Warning message if something went wrong
      *
      * @return true if an input is created or already exists with the same type, false otherwise
      */
     bool createInputString(QString name, QString value = "", QString* warning = NULL);



     /**
      * @brief Create a new boolean input
      *
      * @param name
      * @param value
      * @param warning Warning message if something went wrong
      *
      * @return true if an input is created or already exists with the same type, false otherwise
      */
     bool createInputBool(QString name, bool value = false, QString* warning = NULL);



     /**
      * @brief Create a new impulsion input
      *
      * @param name
      * @param warning Warning message if something went wrong
      *
      * @return true if an input is created or already exists with the same type, false otherwise
      */
     bool createInputImpulsion(QString name, QString* warning = NULL);



     /**
      * @brief Create a new data input
      *
      * @param name
      * @param value
      * @param warning Warning message if something went wrong
      *
      * @return true if an input is created or already exists with the same type, false otherwise
      */
     bool createInputData(QString name, void* value = NULL, QString* warning = NULL);



     /**
      * @brief Create a new integer output
      *
      * @param name
      * @param value
      * @param warning Warning message if something went wrong
      *
      * @return true if an output is created or already exists with the same type, false otherwise
      */
     bool createOutputInt(QString name, int value = 0, QString* warning = NULL);



     /**
      * @brief Create a new double output
      *
      * @param name
      * @param value
      * @param warning Warning message if something went wrong
      *
      * @return true if an output is created or already exists with the same type, false otherwise
      */
     bool createOutputDouble(QString name, double value = 0.0, QString* warning = NULL);



     /**
      * @brief Create a new string output
      *
      * @param name
      * @param value
      * @param warning Warning message if something went wrong
      *
      * @return true if an output is created or already exists with the same type, false otherwise
      */
     bool createOutputString(QString name, QString value = "", QString* warning = NULL);



     /**
      * @brief Create a new boolean output
      *
      * @param name
      * @param value
      * @param warning Warning message if something went wrong
      *
      * @return true if an output is created or already exists with the same type, false otherwise
      */
     bool createOutputBool(QString name, bool value = false, QString* warning = NULL);



     /**
      * @brief Create a new impulsion output
      *
      * @param name
      * @param warning Warning message if something went wrong
      *
      * @return true if an output is created or already exists with the same type, false otherwise
      */
     bool createOutputImpulsion(QString name, QString* warning = NULL);



     /**
      * @brief Create a new data output
      *
      * @param name
      * @param value
      * @param warning Warning message if something went wrong
      *
      * @return true if an output is created or already exists with the same type, false otherwise
      */
     bool createOutputData(QString name, void* value = NULL, QString* warning = NULL);



     /**
      * @brief Create a new integer parameter
      *
      * @param name
      * @param value
      * @param warning Warning message if something went wrong
      *
      * @return true if a parameter is created or already exists with the same type, false otherwise
      */
     bool createParameterInt(QString name, int value = 0, QString* warning = NULL);



     /**
      * @brief Create a new double parameter
      *
      * @param name
      * @param value
      * @param warning Warning message if something went wrong
      *
      * @return true if a parameter is created or already exists with the same type, false otherwise
      */
     bool createParameterDouble(QString name, double value = 0.0, QString* warning = NULL);



     /**
      * @brief Create a new string parameter
      *
      * @param name
      * @param value
      * @param warning Warning message if something went wrong
      *
      * @return true if a parameter is created or already exists with the same type, false otherwise
      */
     bool createParameterString(QString name, QString value = "", QString* warning = NULL);



     /**
      * @brief Create a new boolean parameter
      *
      * @param name
      * @param value
      * @param warning Warning message if something went wrong
      *
      * @return true if a parameter is created or already exists with the same type, false otherwise
      */
     bool createParameterBool(QString name, bool value = false, QString* warning = NULL);



     /**
      * @brief Create a new data parameter
      *
      * @param name
      * @param value
      * @param warning Warning message if something went wrong
      *
      * @return true if a parameter is created or already exists with the same type, false otherwise
      */
     bool createParameterData(QString name, void* value = NULL, QString* warning = NULL);



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



//---------------------------------------------------
//
// Mapping
//
//---------------------------------------------------






//---------------------------------------------------
//
// Administration, configuration & utilities
//
//---------------------------------------------------
public Q_SLOTS:
     /**
      * @brief Get network adapters with broadcast capabilities
      *
      * @param qmlUpdateExtraParameter Extra parameter used to call this function in a QML binding
      *
      * @return
      */
     QStringList getNetdevicesList(QVariant qmlUpdateExtraParameter = QVariant());



     /**
      * @brief Print (or save) debugging information
      * @param logLevel
      * @param text
      */
     void log(MasticLogLevel::Value logLevel, QString text);



     /**
      * @brief Print (or save) debugging information (loglevel = trace)
      * @param text
      */
     void trace(QString text);



     /**
      * @brief Print (or save) debugging information (loglevel = debug)
      * @param text
      */
     void debug(QString text);



     /**
      * @brief Print (or save) debugging information (loglevel = info)
      * @param text
      */
     void info(QString text);



     /**
      * @brief Print (or save) debugging information (loglevel = warn)
      * @param text
      */
     void warn(QString text);



     /**
      * @brief Print (or save) debugging information (loglevel = error)
      * @param text
      */
     void error(QString text);



     /**
      * @brief Print (or save) debugging information (loglevel = fatal)
      * @param text
      */
     void fatal(QString text);




//--------------------------------------------------------
//
// Extra QML API
//
//--------------------------------------------------------

public Q_SLOTS:
     /**
      * @brief Check if an IOP name is valid or if it can create conflicts with Qt internal symbols
      * @param name
      * @return
      */
     bool checkIfIopNameIsValid(const QString& name);




//--------------------------------------------------------
//
// Internal API - MUST not be used outside of MasticQuick
//
//--------------------------------------------------------

public:
     /**
      * @brief Set our isMuted flag based on observeMute
      * @param value
      */
     void _internal_setIsMuted(bool value);



     /**
      * @brief Set our isFrozen flag based on observeFreeze
      * @param value
      */
     void _internal_setIsFrozen(bool value);


     /**
      * @brief Check if we need to update QML in our observeOuput callback
      * @return
      */
     bool _internal_needsToUpdateQmlInObserveOutput();


     /**
      * @brief Check if we need to update QML in our observeParameter callback
      * @return
      */
     bool _internal_needsToUpdateQmlInObserveParameter();


//---------------------------------------------------
//
// Protected methods
//
//---------------------------------------------------
protected:
     /**
      * @brief Create a new input
      *
      * @param name
      * @param type
      * @param qmlValue
      * @param cValue
      * @param cSize
      * @param warning Warning message if something went wrong
      *
      * @return true if an input is created or already exists with the same type, false otherwise
      */
     bool _createInput(QString name, MasticIopType::Value type, QVariant qmlValue, void* cValue, long cSize, QString* warning);



     /**
      * @brief Create a new output
      *
      * @param name
      * @param type
      * @param qmlValue
      * @param cValue
      * @param cSize
      * @param warning Warning message if something went wrong
      *
      * @return true if an output is created or already exists with the same type, false otherwise
      */
     bool _createOutput(QString name, MasticIopType::Value type, QVariant qmlValue, void* cValue, long cSize, QString* warning);



     /**
      * @brief Create a new paramater
      *
      * @param name
      * @param type
      * @param qmlValue
      * @param cValue
      * @param cSize
      * @param warning Warning message if something went wrong
      *
      * @return true if a parameter is created or already exists with the same type, false otherwise
      */
     bool _createParameter(QString name, MasticIopType::Value type, QVariant qmlValue, void* cValue, long cSize, QString* warning);



     /**
      * @brief Update our list of inputs
      */
     void _updateInputsList();



     /**
      * @brief Update our list of outputs
      */
     void _updateOutputsList();



     /**
      * @brief Update our list of parameters
      */
     void _updateParametersList();



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
     void _onOutputUpdatedFromQML(const QString &key, const QVariant &value);



     /**
      * @brief Called when a parameter is updated from QML
      * @param key
      * @param value
      */
     void _onParameterUpdatedFromQML(const QString &key, const QVariant &value);


protected:
     // Flag indicating if we need to update QML in observeOutput
     bool _observeOutputNeedToUpdateQML;
     QMutex _observeOutputNeedToUpdateQMLMutex;

     // Flag indicating if we need to update QML in observeParameter
     bool _observeParameterNeedToUpdateQML;
     QMutex _observeParameterNeedToUpdateQMLMutex;
};

QML_DECLARE_TYPE(MasticQuick)

#endif // _MASTICQUICK_H_
