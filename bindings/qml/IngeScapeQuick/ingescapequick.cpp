/*
 *  IngeScape - QML binding
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

#include "IngeScapeQuick.h"

#include <QDebug>
#include <QCoreApplication>
#include <QMutex>


#include "ingescapequickinputspropertymap.h"
#include "ingescapequickoutputspropertymap.h"
#include "ingescapequickparameterspropertymap.h"

#include "ingescapequickbindingsingleton.h"
#include "ingescapequickinputbinding.h"
#include "ingescapequickoutputbinding.h"



extern "C" {
#include <ingescape.h>
}





//-------------------------------------------------------------------
//
//
//  Static functions
//
//
//-------------------------------------------------------------------



/**
 * @brief Convert a IngeScapeIopType::Value into an iopType_t value
 * @param value
 * @return
 */
static iopType_t enumIngeScapeIopTypeToEnumIopType_t(IngeScapeIopType::Value value)
{
    iopType_t result = IGS_INTEGER_T;

    switch(value)
    {
        case IngeScapeIopType::INTEGER:
            result = IGS_INTEGER_T;
            break;

        case IngeScapeIopType::DOUBLE:
            result = IGS_DOUBLE_T;
            break;

        case IngeScapeIopType::STRING:
            result = IGS_STRING_T;
            break;

        case IngeScapeIopType::BOOLEAN:
            result = IGS_BOOL_T;
            break;

        case IngeScapeIopType::IMPULSION:
            result = IGS_IMPULSION_T;
            break;

        case IngeScapeIopType::DATA:
            result = IGS_DATA_T;
            break;

        default:
            break;
    }

    return result;
}



/**
 * @brief Convert an iopType_t value into a IngeScapeIopType::Value
 * @param value
 * @return
 */
static IngeScapeIopType::Value enumIopType_tToIngeScapeIopType(iopType_t value)
{
    IngeScapeIopType::Value result = IngeScapeIopType::INVALID;

    switch (value)
    {
        case IGS_INTEGER_T:
            result = IngeScapeIopType::INTEGER;
            break;

        case IGS_DOUBLE_T:
            result = IngeScapeIopType::DOUBLE;
            break;

        case IGS_STRING_T:
            result = IngeScapeIopType::STRING;
            break;

        case IGS_BOOL_T:
            result = IngeScapeIopType::BOOLEAN;
            break;

        case IGS_IMPULSION_T:
            result = IngeScapeIopType::IMPULSION;
            break;

        case IGS_DATA_T:
            result = IngeScapeIopType::DATA;
            break;

        default:
            break;
    }

    return result;
}



/**
 * @brief Convert a IngeScapeLogLevel::Value into a igs_logLevel_t value
 * @param value
 * @return
 */
static igs_logLevel_t enumIngeScapeLogLevelToEnumMticLogLevel_t(IngeScapeLogLevel::Value value)
{
    igs_logLevel_t result = IGS_LOG_TRACE;

    switch (value)
    {
        case IngeScapeLogLevel::LOG_TRACE:
            result = IGS_LOG_TRACE;
            break;

        case IngeScapeLogLevel::LOG_DEBUG:
            result = IGS_LOG_DEBUG;
            break;

        case IngeScapeLogLevel::LOG_INFO:
            result = IGS_LOG_INFO;
            break;

        case IngeScapeLogLevel::LOG_WARN:
            result = IGS_LOG_WARN;
            break;

        case IngeScapeLogLevel::LOG_ERROR:
            result = IGS_LOG_ERROR;
            break;

        case IngeScapeLogLevel::LOG_FATAL:
            result = IGS_LOG_FATAL;
            break;

        default:
            break;
    }

    return result;
}



/**
 * @brief Convert a igs_logLevel_t value into a IngeScapeLogLevel::Value
 * @param value
 * @return
 */
static IngeScapeLogLevel::Value enumMticLogLevel_tToIngeScapeLogLevel(igs_logLevel_t value)
{
    IngeScapeLogLevel::Value result = IngeScapeLogLevel::LOG_TRACE;

    switch(value)
    {
        case IGS_LOG_TRACE:
            result = IngeScapeLogLevel::LOG_TRACE;
            break;

        case IGS_LOG_DEBUG:
            result = IngeScapeLogLevel::LOG_DEBUG;
            break;

        case IGS_LOG_INFO:
            result = IngeScapeLogLevel::LOG_INFO;
            break;

        case IGS_LOG_WARN:
            result = IngeScapeLogLevel::LOG_WARN;
            break;

        case IGS_LOG_ERROR:
            result = IngeScapeLogLevel::LOG_ERROR;
            break;

        case IGS_LOG_FATAL:
            result = IngeScapeLogLevel::LOG_FATAL;
            break;

        default:
            break;
    }

    return result;
}



/**
 * @brief Callback used to observe inputs
 *
 * @param iopType
 * @param name
 * @param valueType
 * @param value
 * @param valueSize
 * @param customData
 */
void IngeScapeQuick_callbackObserveInput(iop_t iopType, const char *name, iopType_t valueType, void* value, size_t valueSize, void *customData)
{
    Q_UNUSED(valueSize)

    // Ensure that our callback is called by the required IOP category
    if (iopType == IGS_INPUT_T)
    {
        // Try to cast our custom data
        IngeScapeQuick* controller = (IngeScapeQuick *)customData;
        if ((controller != NULL) && (controller->inputs() != NULL))
        {
            QString qmlName(name);
            if (!qmlName.isEmpty())
            {
                switch(valueType)
                {
                    case IGS_INTEGER_T:
                        {
                            int newValue = *((int *)value);
                            QVariant qmlValue = QVariant(newValue);

                            controller->inputs()->insert(qmlName, qmlValue);
                            Q_EMIT controller->observeInput(qmlName, qmlValue);
                        }
                        break;

                    case IGS_DOUBLE_T:
                        {
                            double newValue = *((double *)value);
                            QVariant qmlValue = QVariant(newValue);

                            controller->inputs()->insert(qmlName, QVariant(newValue));
                            Q_EMIT controller->observeInput(qmlName, qmlValue);
                        }
                        break;

                    case IGS_STRING_T:
                        {
                            char* newCValue = (char *)value;
                            if (newCValue != NULL)
                            {
                                QString newValue(newCValue);
                                QVariant qmlValue = QVariant(newValue);

                                controller->inputs()->insert(qmlName, qmlValue);
                                Q_EMIT controller->observeInput(qmlName, qmlValue);
                                // NB: we don't need to free newValue because we don't own it
                            }
                            else
                            {
                                controller->inputs()->insert(qmlName, QVariant(""));
                                Q_EMIT controller->observeInput(qmlName, QVariant(""));
                            }
                        }
                        break;

                    case IGS_BOOL_T:
                        {
                            bool newValue = *((bool *)value);
                            QVariant qmlValue = QVariant(newValue);

                            controller->inputs()->insert(qmlName, qmlValue);
                            Q_EMIT controller->observeInput(qmlName, qmlValue);
                        }
                        break;

                    case IGS_IMPULSION_T:
                        {
                            // Hack to force the update of our property
                            // We disable signals then we clear its value to detect a valud change when we set an empty value
                            controller->inputs()->blockSignals(true);
                            controller->inputs()->clear(qmlName);
                            controller->inputs()->blockSignals(false);

                            // Set an empty value to trigger an update
                            controller->inputs()->insert(qmlName, QVariant(""));
                            Q_EMIT controller->observeInput(qmlName, QVariant(""));
                        }
                        break;

                    case IGS_DATA_T:
                        {
                            qWarning() << "IngeScapeQuick warning: can not update input" << qmlName <<  "with type DATA (not yet implemented)";
                        }
                        break;

                    default:
                        break;
                }
            }
            else
            {
                qWarning() << "IngeScapeQuick warning: can not update an input with an empty name";
            }
        }
        // Else: invalid custom data
    }
    // Else: something went wrong
}



/**
 * @brief Callback used to observe outputs
 *
 * @param iopType
 * @param name
 * @param valueType
 * @param value
 * @param valueSize
 * @param customData
 */
void IngeScapeQuick_callbackObserveOutput(iop_t iopType, const char *name, iopType_t valueType, void* value, size_t valueSize, void *customData)
{
    Q_UNUSED(valueSize)

    // Ensure that our callback is called by the required IOP category
    if (iopType == IGS_OUTPUT_T)
    {
        // Try to cast our custom data
        IngeScapeQuick* controller = (IngeScapeQuick *)customData;
        if ((controller != NULL) && (controller->outputs() != NULL))
        {
            // Check if we have a valid name
            QString qmlName(name);
            if (!qmlName.isEmpty())
            {
                // Check if we need to update QML
                bool needToUpdateQML = controller->_internal_needsToUpdateQmlInObserveOutput();

                // Check type of output
                switch(valueType)
                {
                    case IGS_INTEGER_T:
                        {
                            // Cast our new value
                            int newValue = *((int *)value);
                            QVariant qmlValue = QVariant(newValue);

                            // Update QML
                            if (needToUpdateQML)
                            {
                                controller->outputs()->insert(qmlName, qmlValue);
                            }

                            // Notify that an output has changed
                            Q_EMIT controller->observeOutput(qmlName, qmlValue);
                        }
                        break;

                    case IGS_DOUBLE_T:
                        {
                            // Cast our new value
                            double newValue = *((double *)value);
                            QVariant qmlValue = QVariant(newValue);

                            // Update QML
                            if (needToUpdateQML)
                            {
                                controller->outputs()->insert(qmlName, QVariant(newValue));
                            }

                            // Notify that an output has changed
                            Q_EMIT controller->observeOutput(qmlName, qmlValue);
                        }
                        break;

                    case IGS_STRING_T:
                        {
                            // Check if we have a non NULL value
                            char* newCValue = (char *)value;
                            if (newCValue != NULL)
                            {
                                // Cast our new value
                                QString newValue(newCValue);
                                QVariant qmlValue = QVariant(newValue);

                                // Update QML
                                if (needToUpdateQML)
                                {
                                    controller->outputs()->insert(qmlName, qmlValue);
                                }

                                // Notify that an output has changed
                                Q_EMIT controller->observeOutput(qmlName, qmlValue);


                                // NB: we don't need to free newValue because we don't own it
                            }
                            else
                            {
                                // Update QML
                                if (needToUpdateQML)
                                {
                                    controller->outputs()->insert(qmlName, QVariant(""));
                                }

                                // Notify that an output has changed
                                Q_EMIT controller->observeOutput(qmlName, QVariant(""));
                            }
                        }
                        break;

                    case IGS_BOOL_T:
                        {
                            // Cast our new value
                            bool newValue = *((bool *)value);
                            QVariant qmlValue = QVariant(newValue);

                            // Update QML
                            if (needToUpdateQML)
                            {
                                controller->outputs()->insert(qmlName, qmlValue);
                            }

                            // Notify that an output has changed
                            Q_EMIT controller->observeOutput(qmlName, qmlValue);
                        }
                        break;

                    case IGS_IMPULSION_T:
                        {
                            // Update QML
                            if (needToUpdateQML)
                            {
                                // Hack to force the update of our property
                                // We disable signals then we clear its value to detect a valud change when we set an empty value
                                controller->outputs()->blockSignals(true);
                                controller->outputs()->clear(qmlName);
                                controller->outputs()->blockSignals(false);

                                // Set an empty value to trigger an update
                                controller->outputs()->insert(qmlName, QVariant(""));
                            }

                            // Notify that an output has changed
                            Q_EMIT controller->observeOutput(qmlName, QVariant(""));
                        }
                        break;

                    case IGS_DATA_T:
                        {
                            qWarning() << "IngeScapeQuick warning: can not update output" << qmlName << "with type DATA (not yet implemented)";
                        }
                        break;

                    default:
                        break;
                }
            }
            else
            {
                qWarning() << "IngeScapeQuick warning: can not update an output with an empty name";
            }
        }
        // Else: invalid custom data
    }
    // Else: something went wrong
}



/**
 * @brief Callback used to observe parameters
 *
 * @param iopType
 * @param name
 * @param valueType
 * @param value
 * @param valueSize
 * @param customData
 */
void IngeScapeQuick_callbackObserveParameter(iop_t iopType, const char *name, iopType_t valueType, void* value, size_t valueSize, void *customData)
{
    Q_UNUSED(valueSize)

    // Ensure that our callback is called by the required IOP category
    if (iopType == IGS_PARAMETER_T)
    {
        // Try to cast our custom data
        IngeScapeQuick* controller = (IngeScapeQuick *)customData;
        if ((controller != NULL) && (controller->parameters() != NULL))
        {
            // Check if we have a valid name
            QString qmlName(name);
            if (!qmlName.isEmpty())
            {
                // Check if we need to update QML
                bool needToUpdateQML = controller->_internal_needsToUpdateQmlInObserveParameter();

                // Check type of parameter
                switch(valueType)
                {
                    case IGS_INTEGER_T:
                        {
                            // Cast our new value
                            int newValue = *((int *)value);
                            QVariant qmlValue = QVariant(newValue);

                            // Update QML
                            if (needToUpdateQML)
                            {
                                controller->parameters()->insert(qmlName, qmlValue);
                            }

                            // Notify that a parameter has changed
                            Q_EMIT controller->observeParameter(qmlName, qmlValue);
                        }
                        break;

                    case IGS_DOUBLE_T:
                        {
                            // Cast our new value
                            double newValue = *((double *)value);
                            QVariant qmlValue = QVariant(newValue);

                            // Update QML
                            if (needToUpdateQML)
                            {
                                controller->parameters()->insert(qmlName, QVariant(newValue));
                            }

                            // Notify that a parameter has changed
                            Q_EMIT controller->observeParameter(qmlName, qmlValue);
                        }
                        break;

                    case IGS_STRING_T:
                        {
                            // Check if our new value is NULL or not
                            char* newCValue = (char *)value;
                            if (newCValue != NULL)
                            {
                                // Cast our new value
                                QString newValue(newCValue);
                                QVariant qmlValue = QVariant(newValue);

                                // Update QML
                                if (needToUpdateQML)
                                {
                                    controller->parameters()->insert(qmlName, qmlValue);
                                }

                                // Notify that a parameter has changed
                                Q_EMIT controller->observeParameter(qmlName, qmlValue);

                                // NB: we don't need to free newValue because we don't own it
                            }
                            else
                            {
                                // Update QML
                                if (needToUpdateQML)
                                {
                                    controller->parameters()->insert(qmlName, QVariant(""));
                                }

                                // Notify that a parameter has changed
                                Q_EMIT controller->observeParameter(qmlName, QVariant(""));
                            }
                        }
                        break;

                    case IGS_BOOL_T:
                        {
                            // Cast our new value
                            bool newValue = *((bool *)value);
                            QVariant qmlValue = QVariant(newValue);

                            // Update QML
                            if (needToUpdateQML)
                            {
                                controller->parameters()->insert(qmlName, qmlValue);
                            }

                            // Notify that a parameter has changed
                            Q_EMIT controller->observeParameter(qmlName, qmlValue);
                        }
                        break;

                    case IGS_IMPULSION_T:
                        {
                            // Should not happen because a paramater can not be an impulsion
                            qWarning() << "IngeScapeQuick warning: can not update parameter" << qmlName << "with type IMPULSION";
                        }
                        break;

                    case IGS_DATA_T:
                        {
                            qWarning() << "IngeScapeQuick warning: can not update parameter" << qmlName << "with type DATA (not yet implemented)";
                        }
                        break;

                    default:
                        break;
                }
            }
            else
            {
                qWarning() << "IngeScapeQuick warning: can not update a parameter with an empty name";
            }
        }
        // Else: invalid custom data
    }
    // Else: something went wrong
}



/**
 * @brief Callback used to observe freeze/unfreeze
 * @param isPause
 * @param customData
 */
void IngeScapeQuick_callbackObserveFreeze(bool isFrozen, void* customData)
{
    // Try to cast our custom data
    IngeScapeQuick* controller = (IngeScapeQuick *)customData;
    if (controller != NULL)
    {
        controller->_internal_setIsFrozen(isFrozen);
    }
}



/**
 * @brief Callback used to observe mute/unmute
 * @param isPause
 * @param customData
 */
void IngeScapeQuick_callbackObserveMute(bool isMuted, void* customData)
{
    // Try to cast our custom data
    IngeScapeQuick* controller = (IngeScapeQuick *)customData;
    if (controller != NULL)
    {
        controller->_internal_setIsMuted(isMuted);
    }
}



/**
 * @brief Callback used to observe force stop
 * @param customData
 */
void IngeScapeQuick_callbackForcedStop(void *customData)
{
    // Try to cast our custom data
    IngeScapeQuick* controller = (IngeScapeQuick *)customData;
    if (controller != NULL)
    {
        Q_EMIT controller->forcedStop();
    }
}




//-------------------------------------------------------------------
//
//
//  IngeScapeQuick
//
//
//-------------------------------------------------------------------


//
// Define our singleton instance
//
// NB: we can not use Q_GLOBAL_STATIC because IngeScape MUST be stopped in our mainloop.
//     Otherwise, IngeScape can not terminate ZMQ safely and the application will crash
//
static IngeScapeQuick* _INGESCAPEQUICK_SINGLETON = NULL;


//
// Flag indicating if we have already instantiate a singleton
//
static bool _INGESCAPEQUICK_SINGLETON_INITIALIZED = false;


//
// Define a mutex to protect our singleton instance
//
static QMutex _INGESCAPEQUICK_SINGLETON_MUTEX;




/**
 * @brief Default constructor
 * @param parent
 */
IngeScapeQuick::IngeScapeQuick(QObject *parent) : QObject(parent),
    _isStarted(false),
    _inputs(NULL),
    _outputs(NULL),
    _parameters(NULL),
    _observeOutputNeedToUpdateQML(true),
    _observeOutputNeedToUpdateQMLMutex(QMutex::Recursive),
    _observeParameterNeedToUpdateQML(true),
    _observeParameterNeedToUpdateQMLMutex(QMutex::Recursive)
{    

    //-------------------------------------------------
    //
    // IngeScape agent info
    //
    //-------------------------------------------------


    // Set our default agent name
    setagentName(QCoreApplication::applicationName());

    // Set our default definition
    setdefinitionName(QCoreApplication::applicationName());
    setdefinitionVersion("0.0");
    setdefinitionDescription(tr("Definition of %1").arg(QCoreApplication::applicationName()));


    //-------------------------------------------------
    //
    // Get initial values of IngeScape internal properties
    //
    //-------------------------------------------------

    // - version of IngeScape  
    _version = igs_version();
    // INGESCAPE_VERSION ((INGESCAPE_MAJOR * 10000) + (INGESCAPE_MINOR * 100) + INGESCAPE_MICRO)
    int versionMajor = _version/10000;
    int versionMinor = (_version - versionMajor * 10000)/100;
    int versionMicro = (_version - (versionMajor * 10000 + versionMinor * 100));
    _versionString = QString("%1.%2.%3").arg(versionMajor).arg(versionMinor).arg(versionMicro);

    // - agent state
    char* cAgentState = igs_getAgentState();
    if (cAgentState != NULL)
    {
        _agentState = QString(cAgentState);
        free(cAgentState);
        cAgentState = NULL;
    }

    // - isFrozen flag
    _isFrozen = igs_isFrozen();

    // - canBeFrozen flag
    _canBeFrozen = igs_canBeFrozen();

    // - isMuted flag
    _isMuted = igs_isMuted();

    // - requestOutputsFromMappedAgents
    _requestOutputsFromMappedAgents = igs_getRequestOutputsFromMappedAgents();

    // - isVerbose
    _isVerbose = igs_isVerbose();

    // - log stream
    _logStream = igs_getLogStream();

    // - log in file
    _logInFile = igs_getLogInFile();

    // - use color
    _useColorVerbose = igs_getUseColorVerbose();

    // - log path
    char* cLogPath = igs_getLogPath();
    if (cLogPath != NULL)
    {
        _logPath = QString(cLogPath);
        delete cLogPath;
        cLogPath = NULL;
    }

    // - log level
    _logLevel = enumMticLogLevel_tToIngeScapeLogLevel( igs_getLogLevel() );



    //-------------------------------------------------
    //
    // Add ingescape observers
    //
    //-------------------------------------------------

    // Observe mute/unmute
    igs_observeMute(&IngeScapeQuick_callbackObserveMute, this);

    // Observe freeze/unfreeze
    igs_observeFreeze(&IngeScapeQuick_callbackObserveFreeze, this);


    // Observe forced stop
    connect(this, &IngeScapeQuick::forcedStop, this, &IngeScapeQuick::_onForcedStop);
    igs_observeForcedStop(&IngeScapeQuick_callbackForcedStop, this);



    //-------------------------------------------------
    //
    // Init dynamic properties
    //
    //-------------------------------------------------

    // - inputs
    _inputs = new IngeScapeQuickInputsPropertyMap(this);

    // - outputs
    _outputs = new IngeScapeQuickOutputsPropertyMap(this);
    if (_outputs != NULL)
    {
        connect(_outputs, &QQmlPropertyMap::valueChanged, this, &IngeScapeQuick::_onOutputUpdatedFromQML);
    }

    // - parameters
    _parameters = new IngeScapeQuickParametersPropertyMap(this);
    if (_parameters != NULL)
    {
        connect(_parameters, &QQmlPropertyMap::valueChanged, this, &IngeScapeQuick::_onParameterUpdatedFromQML);
    }
}


/**
 * @brief Destructor
 */
IngeScapeQuick::~IngeScapeQuick()
{
    // Unsubscribe to IngeScape signals
    disconnect(this, &IngeScapeQuick::forcedStop, this, &IngeScapeQuick::_onForcedStop);

    // Stop our agent
    stop();

    // Clean-up inputs
    if (_inputs != NULL)
    {
        // Save our value
        QQmlPropertyMap* temp = _inputs;

        // QML clean-up
        _inputs = NULL;
        Q_EMIT inputsChanged(NULL);

        // Memory clean-up
        delete temp;
    }

    // Clean-up outputs
    if (_outputs != NULL)
    {
        // Unsubscribe to signals
        disconnect(_outputs, &QQmlPropertyMap::valueChanged, this, &IngeScapeQuick::_onOutputUpdatedFromQML);

        // Save our value
        QQmlPropertyMap* temp = _outputs;

        // QML clean-up
        _outputs = NULL;
        Q_EMIT outputsChanged(NULL);

        // Memory clean-up
        delete temp;
    }

    // Clean-up parameters
    if (_parameters != NULL)
    {
        // Unsubcribe to signals
        disconnect(_parameters, &QQmlPropertyMap::valueChanged, this, &IngeScapeQuick::_onParameterUpdatedFromQML);

        // Save our value
        QQmlPropertyMap* temp = _parameters;

        // QML clean-up
        _parameters = NULL;
        Q_EMIT parametersChanged(NULL);

        // Memory clean-up
        delete temp;
    }


    //
    // Clean-up our simple lists
    //
    _inputsList.clear();
    _outputsList.clear();
    _parametersList.clear();



    //
    // Clean-up our singleton
    //
    _INGESCAPEQUICK_SINGLETON_MUTEX.lock();
    if (_INGESCAPEQUICK_SINGLETON == this)
    {
        _INGESCAPEQUICK_SINGLETON = NULL;
    }
    _INGESCAPEQUICK_SINGLETON_MUTEX.unlock();
}



/**
 * @brief Get our singleton instance
 * @return
 *
 * @remark Our singleton is owned by the QML engine. Thus, it is unsafe to keep a reference that points to it
 */
IngeScapeQuick* IngeScapeQuick::instance()
{
    // Thread-safe init
    _INGESCAPEQUICK_SINGLETON_MUTEX.lock();

    if (_INGESCAPEQUICK_SINGLETON == NULL)
    {
        // Check if it is our first singleton
        if (_INGESCAPEQUICK_SINGLETON_INITIALIZED)
        {
            qWarning() << "IngeScapeQuick warning: a singleton has already been created and destroyed."
                       << "Please do not try to access IngeScapeQuick after its destruction by QML";
        }
        else
        {
            // First init, we can create a singleton
            _INGESCAPEQUICK_SINGLETON = new IngeScapeQuick();
            _INGESCAPEQUICK_SINGLETON_INITIALIZED = true;
        }
    }

    _INGESCAPEQUICK_SINGLETON_MUTEX.unlock();

    return _INGESCAPEQUICK_SINGLETON;
}



/**
 * @brief Method used to provide a singleton to QML
 * @param engine
 * @param scriptEngine
 * @return
 */
QObject* IngeScapeQuick::qmlSingleton(QQmlEngine* engine, QJSEngine* scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine);

    // NOTE: our singleton is owned by the QML engine
    return instance();
}




/**
 * @brief Fonction called to register all new types which are provided by this extension plugin
 * @param uri The URI of our extension plugin
 */
void IngeScapeQuick::registerTypes(const char* uri)
{
    Q_UNUSED(uri)


    //
    // Register enums
    //
    // - IOP types
    qmlRegisterSingletonType<IngeScapeIopType>("IngeScapeQuick", 1, 0, "IngeScapeIopType", &IngeScapeIopType::qmlSingleton);
    IngeScapeIopType::registerMetaType();
    // - Log levels
    qmlRegisterSingletonType<IngeScapeLogLevel>("IngeScapeQuick", 1, 0, "IngeScapeLogLevel", &IngeScapeLogLevel::qmlSingleton);
    IngeScapeLogLevel::registerMetaType();


    //
    // Register uncreatable types
    //
    qmlRegisterUncreatableType<IngeScapeQuickInputsPropertyMap>("IngeScapeQuick", 1, 0, "IngeScapeQuickInputsPropertyMap", "Internal class");
    qmlRegisterUncreatableType<IngeScapeQuickOutputsPropertyMap>("IngeScapeQuick", 1, 0, "IngeScapeQuickOutputsPropertyMap", "Internal class");
    qmlRegisterUncreatableType<IngeScapeQuickParametersPropertyMap>("IngeScapeQuick", 1, 0, "IngeScapeQuickParametersPropertyMap", "Internal class");


    //
    // Register creatable types
    //
    qmlRegisterType<IngeScapeQuickInputBinding>("IngeScapeQuick", 1, 0, "IngeScapeInputBinding");
    qmlRegisterType<IngeScapeQuickOutputBinding>("IngeScapeQuick", 1, 0, "IngeScapeOutputBinding");


    //
    // Register singletons
    //
    qmlRegisterSingletonType<IngeScapeQuickBindingSingleton>("IngeScapeQuick", 1, 0, "IngeScapeBinding", &IngeScapeQuickBindingSingleton::qmlSingleton);
    qmlRegisterSingletonType<IngeScapeQuick>("IngeScapeQuick", 1, 0, "IngeScape", &IngeScapeQuick::qmlSingleton);
}





//-------------------------------------------------------------------
//
// Custom getters
//
//-------------------------------------------------------------------







//-------------------------------------------------------------------
//
// Custom setters
//
//-------------------------------------------------------------------



/**
 * @brief Set our agent name
 * @param value
 */
void IngeScapeQuick::setagentName(QString value)
{
    if (_agentName != value)
    {
        // Save value
        _agentName = value;

        // Set our agent name
        igs_setAgentName(value.toStdString().c_str());

        // Notify change
        Q_EMIT agentNameChanged(value);
    }
}



/**
 * @brief Set our agent state
 * @param value
 */
void IngeScapeQuick::setagentState(QString value)
{
    if (_agentState != value)
    {
        // Save our new state
        _agentState = value;

        // Set our new state
        igs_setAgentState(_agentState.toStdString().c_str());

        // Notify change
        Q_EMIT agentStateChanged(value);
    }
}



/**
 * @brief Set the name of our definition
 * @param value
 */
void IngeScapeQuick::setdefinitionName(QString value)
{
    if (_definitionName != value)
    {
        // Save value
        _definitionName = value;

        // Set our definition name
        igs_setDefinitionName(value.toStdString().c_str());

        // Notify change
        Q_EMIT definitionNameChanged(value);
    }
}



/**
 * @brief Set the version of our definition
 * @param value
 */
void IngeScapeQuick::setdefinitionVersion(QString value)
{
    if (_definitionVersion != value)
    {
        // Save value
        _definitionVersion = value;

        // Set our definition version
        igs_setDefinitionVersion(value.toStdString().c_str());

        // Notify change
        Q_EMIT definitionVersionChanged(value);
    }
}



/**
 * @brief Set the description of our definition
 * @param value
 */
void IngeScapeQuick::setdefinitionDescription(QString value)
{
    if (_definitionDescription != value)
    {
        // Save value
        _definitionDescription = value;

        // Set our definition description
        igs_setDefinitionDescription(value.toStdString().c_str());

        // Notify change
        Q_EMIT definitionDescriptionChanged(value);
    }
}



/**
 * @brief Set our isMuted flag
 * @param value
 */
void IngeScapeQuick::setisMuted(bool value)
{
    if (_isMuted != value)
    {
        // Set our isMuted flag
        if (value)
        {
            igs_mute();
        }
        else
        {
            igs_unmute();
        }

        // Get our new value
        _isMuted = igs_isMuted();

        // Notify change
        Q_EMIT isMutedChanged(_isMuted);
    }
}



/**
 * @brief Set our isFrozen flag
 * @param value
 */
void IngeScapeQuick::setisFrozen(bool value)
{
    if (_isFrozen != value)
    {
        // Set our isFrozen flag
        if (value)
        {
            igs_freeze();
        }
        else
        {
            igs_unfreeze();
        }

        // Get our new value
        _isFrozen = igs_isFrozen();

        // Notify change
        Q_EMIT isFrozenChanged(_isFrozen);
    }
}



/**
 * @brief Set if our agent can be frozen or not
 * @param value
 */
void IngeScapeQuick::setcanBeFrozen(bool value)
{
    if (_canBeFrozen != value)
    {
        // Save our new value
        _canBeFrozen = value;

        // Set our canBeFrozen property
        igs_setCanBeFrozen(value);

        // Notify change
        Q_EMIT canBeFrozenChanged(value);
    }
}



/**
 * @brief When mapping an agent setting we may request the mapped agent
 *         to send its outputs (except for data & impulsions) to us through
 *         a private communication for our proper initialization
 *
 * @param value
 */
void IngeScapeQuick::setrequestOutputsFromMappedAgents(bool value)
{
    if (_requestOutputsFromMappedAgents != value)
    {
        // Save value
        _requestOutputsFromMappedAgents = value;

        // Set our flag
        igs_setRequestOutputsFromMappedAgents(value);

        // Notify change
        Q_EMIT requestOutputsFromMappedAgentsChanged(value);
    }
}



/**
 * @brief Set our isVerbose flag
 * @param value
 */
void IngeScapeQuick::setisVerbose(bool value)
{
    if (_isVerbose != value)
    {
        // Save value
        _isVerbose = value;

        // Set our verbose flag
        igs_setVerbose(value);

        // Notify change
        Q_EMIT isVerboseChanged(value);
    }
}



/**
 * @briefSet our logStream flag
 * @param value
 */
void IngeScapeQuick::setlogStream(bool value)
{
    if (_logStream != value)
    {
        // Save value
        _logStream = value;

        // Set our flag
        igs_setLogStream(value);

        // Notify change
        Q_EMIT logStreamChanged(value);
    }
}



/**
 * @brief Set our logInFile flag
 * @param value
 */
void IngeScapeQuick::setlogInFile(bool value)
{
    if (_logInFile != value)
    {
        // Save value
        _logInFile = value;

        // Set our flag
        igs_setLogInFile(value);

        // Notify change
        Q_EMIT logInFileChanged(value);
    }
}



/**
 * @brief Set our useColorVerbose flag
 * @param value
 */
void IngeScapeQuick::setuseColorVerbose(bool value)
{
    if (_useColorVerbose != value)
    {
        // Save value
        _useColorVerbose = value;

        // Set our flag
        igs_setUseColorVerbose(value);

        // Notify change
        Q_EMIT useColorVerboseChanged(value);
    }
}



/**
 * @brief Set path of our log file
 * @param value
 */
void IngeScapeQuick::setlogPath(QString value)
{
    if (_logPath != value)
    {
        // Save value
        _logPath = value;

        // Set our new log path
        igs_setLogPath(value.toStdString().c_str());

        // Notify change
        Q_EMIT logPathChanged(value);
    }
}



/**
 * @brief Set our log level
 * @param value
 */
void IngeScapeQuick::setlogLevel(IngeScapeLogLevel::Value value)
{
    if (_logLevel != value)
    {
        // Save value
        _logLevel = value;

        // Set out log level
        igs_setLogLevel( enumIngeScapeLogLevelToEnumMticLogLevel_t(value) );

        // Notify change
        Q_EMIT logLevelChanged(value);
    }
}





//-------------------------------------------------------------------
//
// Start or stop our agent
//
//-------------------------------------------------------------------

/**
 * @brief Start our agent with a given network device and port
 *
 * @param networkDevice
 * @param port
 */
bool IngeScapeQuick::startWithDevice(QString networkDevice, int port)
{
    bool result = false;

    // Check if we must stop our agent
    if (_isStarted)
    {
        stop();
    }

    // Try to start IngeScape
    if (igs_startWithDevice(networkDevice.toStdString().c_str(), port) == 1)
    {
        qInfo() << "IngeScapeQuick: IngeScape started on device" << networkDevice << "with port" << port;
        setisStarted(true);
        result = true;
    }
    else
    {
        qWarning() << "IngeScapeQuick warning: fail to start our agent via startWithDevice("
                   << networkDevice << "," << port << ")";
    }

    return result;
}



/**
 * @brief Start our agent with a given IP address and port
 *
 * @param ipAddress
 * @param port
 */
bool IngeScapeQuick::startWithIP(QString ipAddress, int port)
{
    bool result = false;

    // Check if we must stop our agent
    if (_isStarted)
    {
        stop();
    }

    // Try to start IngeScape
    if (igs_startWithIP(ipAddress.toStdString().c_str(), port) == 1)
    {
        qInfo() << "IngeScapeQuick: IngeScape started on IP" << ipAddress << "with port" << port;
        setisStarted(true);
        result = true;
    }
    else
    {
        qWarning() << "IngeScapeQuick warning: fail to start our agent via startWithIP("
                   << ipAddress << "," << port << ")";
    }

    return result;
}



/**
 * @brief Stop our agent
 */
bool IngeScapeQuick::stop()
{
    bool result = false;

    // Check if our agent is started
    if (_isStarted)
    {
        if (igs_stop() == 1)
        {
            qInfo() << "IngeScapeQuick: IngeScape stopped";
            setisStarted(false);
            result = true;
        }
        else
        {
            qWarning() << "IngeScapeQuick warning: fail to stop our agent";
        }
    }

    return result;
}




//-------------------------------------------------------------------
//
// Read per type
//
//-------------------------------------------------------------------





//-------------------------------------------------------------------
//
// Write per type
//
//-------------------------------------------------------------------


/**
 * @brief Write a given output as an integer
 *
 * @param name
 * @param value
 *
 * @return  true if everything is ok, false otherwise
 */
bool IngeScapeQuick::writeOutputAsInt(QString name, int value)
{
    bool result = false;

    // Ensure that we have a valid name
    if (!name.isEmpty())
    {
        // Try to write value
        if (igs_writeOutputAsInt(name.toStdString().c_str(), value) == 1)
        {
            result = true;
        }
        else
        {
            qWarning() << "IngeScapeQuick warning: fail to write IngeScape output"
                       << name << "via writeOutputAsInt";
        }
    }
    else
    {
        qWarning() << "IngeScapeQuick warning: writeOutputAsInt() can not write an output without a name";
    }

    return result;
}



/**
 * @brief Write a given output as a double
 *
 * @param name
 * @param value
 *
 * @return  true if everything is ok, false otherwise
 */
bool IngeScapeQuick::writeOutputAsDouble(QString name, double value)
{
    bool result = false;

    // Ensure that we have a valid name
    if (!name.isEmpty())
    {
        // Try to write value
        if (igs_writeOutputAsDouble(name.toStdString().c_str(), value) == 1)
        {
            result = true;
        }
        else
        {
            qWarning() << "IngeScapeQuick warning: fail to write output"
                       << name << "via writeOutputAsDouble";
        }
    }
    else
    {
        qWarning() << "IngeScapeQuick warning: writeOutputAsDouble() can not write an output without a name";
    }

    return result;
}



/**
 * @brief Write a given output as a string
 *
 * @param name
 * @param value
 *
 * @return  true if everything is ok, false otherwise
 */
bool IngeScapeQuick::writeOutputAsString(QString name, QString value)
{
    bool result = false;

    // Ensure that we have a valid name
    if (!name.isEmpty())
    {
        // Try to write value
        if (igs_writeOutputAsString(name.toStdString().c_str(), (char *)value.toStdString().c_str()) == 1)
        {
            result = true;
        }
        else
        {
            qWarning() << "IngeScapeQuick warning: fail to write output"
                       << name << "via writeOutputAsString";
        }
    }
    else
    {
        qWarning() << "IngeScapeQuick warning: writeOutputAsString() can not write an output without a name";
    }

    return result;
}



/**
 * @brief Write a given output as a boolean
 *
 * @param name
 * @param value
 *
 * @return  true if everything is ok, false otherwise
 */
bool IngeScapeQuick::writeOutputAsBool(QString name, bool value)
{
    bool result = false;

    // Ensure that we have a valid name
    if (!name.isEmpty())
    {
        // Try to write value
        if (igs_writeOutputAsBool(name.toStdString().c_str(), value) == 1)
        {
            result = true;
        }
        else
        {
            qWarning() << "IngeScapeQuick warning: fail to write output"
                       << name << "via writeOutputAsBool";
        }
    }
    else
    {
        qWarning() << "IngeScapeQuick warning: writeOutputAsBool() can not write an output without a name";
    }

    return result;
}



/**
 * @brief Write a given output as an impulsion
 *
 * @param name
 *
 * @return  true if everything is ok, false otherwise
 */
bool IngeScapeQuick::writeOutputAsImpulsion(QString name)
{
    bool result = false;

    // Ensure that we have a valid name
    if (!name.isEmpty())
    {
        // Try to write value
        if (igs_writeOutputAsImpulsion(name.toStdString().c_str()) == 1)
        {
            result = true;
        }
        else
        {
            qWarning() << "IngeScapeQuick warning: fail to write output"
                       << name << "via writeOutputAsImpulsion";
        }
    }
    else
    {
        qWarning() << "IngeScapeQuick warning: writeOutputAsImpulsion() can not write an output without a name";
    }

    return result;
}



/**
 * @brief Write a given output as data
 *
 * @param name
 * @param value
 *
 * @return  true if everything is ok, false otherwise
 */
bool IngeScapeQuick::writeOutputAsData(QString name, void* value)
{
    Q_UNUSED(name)
    Q_UNUSED(value)

    bool result = false;

    qWarning() << "IngeScapeQuick warning: NOT YET IMPLEMENTED. Can not write output"
               << name << "with type DATA" << "via writeOutputAsData()";

    return result;
}



/**
 * @brief Write a given parameter as an integer
 *
 * @param name
 * @param value
 *
 * @return  true if everything is ok, false otherwise
 */
bool IngeScapeQuick::writeParameterAsInt(QString name, int value)
{
    bool result = false;

    // Ensure that we have a valid name
    if (!name.isEmpty())
    {
        // Try to write value
        if (igs_writeParameterAsInt(name.toStdString().c_str(), value) == 1)
        {
            result = true;
        }
        else
        {
            qWarning() << "IngeScapeQuick warning: fail to write parameter"
                       << name << "via writeParameterAsInt";
        }
    }
    else
    {
        qWarning() << "IngeScapeQuick warning: writeParameterAsInt() can not write a parameter without a name";
    }

    return result;
}



/**
 * @brief Write a given parameter as a double
 *
 * @param name
 * @param value
 *
 * @return  true if everything is ok, false otherwise
 */
bool IngeScapeQuick::writeParameterAsDouble(QString name, double value)
{
    bool result = false;

    // Ensure that we have a valid name
    if (!name.isEmpty())
    {
        // Try to write value
        if (igs_writeParameterAsDouble(name.toStdString().c_str(), value) == 1)
        {
            result = true;
        }
        else
        {
            qWarning() << "IngeScapeQuick warning: fail to write parameter"
                       << name << "via writeParameterAsDouble";
        }
    }
    else
    {
        qWarning() << "IngeScapeQuick warning: writeParameterAsDouble() can not write a parameter without a name";
    }

    return result;
}



/**
 * @brief Write a given parameter as a string
 *
 * @param name
 * @param value
 *
 * @return  true if everything is ok, false otherwise
 */
bool IngeScapeQuick::writeParameterAsString(QString name, QString value)
{
    bool result = false;

    // Ensure that we have a valid name
    if (!name.isEmpty())
    {
        // Try to write value
        if (igs_writeParameterAsString(name.toStdString().c_str(), (char *)value.toStdString().c_str()) == 1)
        {
            result = true;
        }
        else
        {
            qWarning() << "IngeScapeQuick warning: fail to write parameter"
                       << name << "via writeParameterAsString";
        }
    }
    else
    {
        qWarning() << "IngeScapeQuick warning: writeParameterAsString() can not write a parametrer without a name";
    }

    return result;
}



/**
 * @brief Write a given parameter as a boolean
 *
 * @param name
 * @param value
 *
 * @return  true if everything is ok, false otherwise
 */
bool IngeScapeQuick::writeParameterAsBool(QString name, bool value)
{
    bool result = false;

    // Ensure that we have a valid name
    if (!name.isEmpty())
    {
        // Try to write value
        if (igs_writeParameterAsBool(name.toStdString().c_str(), value) == 1)
        {
            result = true;
        }
        else
        {
            qWarning() << "IngeScapeQuick warning: fail to write parameter"
                       << name << "via writeParameterAsBool";
        }
    }
    else
    {
        qWarning() << "IngeScapeQuick warning: writeParameterAsBool() can not write a parameter without a name";
    }

    return result;
}



/**
 * @brief Write a given parameter as data
 *
 * @param name
 * @param value
 *
 * @return  true if everything is ok, false otherwise
 */
bool IngeScapeQuick::writeParameterAsData(QString name, void* value)
{
    Q_UNUSED(name)
    Q_UNUSED(value)

    bool result = false;

    qWarning() << "IngeScapeQuick warning: NOT YET IMPLEMENTED. Can not write parameter"
               << name << "with type DATA via writeParameterAsData";

    return result;
}



//-------------------------------------------------------------------
//
// Check IOP type and existence
//
//-------------------------------------------------------------------


/**
 * @brief Get type of a given input
 * @param name
 * @return
 */
IngeScapeIopType::Value IngeScapeQuick::getTypeForInput(QString name)
{
    IngeScapeIopType::Value result = IngeScapeIopType::INVALID;

    if (!name.isEmpty())
    {
        result = enumIopType_tToIngeScapeIopType( igs_getTypeForInput(name.toStdString().c_str()) );
    }
    else
    {
        qWarning() << "IngeScapeQuick warning: getTypeForInput( name ) - name can not be empty";
    }

    return result;
}



/**
 * @brief Get type of a given output
 * @param name
 * @return
 */
IngeScapeIopType::Value IngeScapeQuick::getTypeForOutput(QString name)
{
    IngeScapeIopType::Value result = IngeScapeIopType::INVALID;

    if (!name.isEmpty())
    {
        result = enumIopType_tToIngeScapeIopType( igs_getTypeForOutput(name.toStdString().c_str()) );
    }
    else
    {
        qWarning() << "IngeScapeQuick warning: getTypeForOutput( name ) - name can not be empty";
    }

    return result;
}



/**
 * @brief Get type of a given parameter
 * @param name
 * @return
 */
IngeScapeIopType::Value IngeScapeQuick::getTypeForParameter(QString name)
{
    IngeScapeIopType::Value result = IngeScapeIopType::INVALID;

    if (!name.isEmpty())
    {
        result = enumIopType_tToIngeScapeIopType( igs_getTypeForParameter(name.toStdString().c_str()) );
    }
    else
    {
        qWarning() << "IngeScapeQuick warning: getTypeForParameter() - name can not be empty";
    }

    return result;
}



/**
 * @brief Check if our agent has an input with this name
 * @param name
 * @return
 */
bool IngeScapeQuick::checkInputExistence(QString name)
{
    return igs_checkInputExistence(name.toStdString().c_str());
}



/**
 * @brief Check if our agent has an output with this name
 * @param name
 * @return
 */
bool IngeScapeQuick::checkOutputExistence(QString name)
{
    return igs_checkOutputExistence(name.toStdString().c_str());
}



/**
 * @brief Check if our agent has a parameter with this name
 * @param name
 * @return
 */
bool IngeScapeQuick::checkParameterExistence(QString name)
{
    return igs_checkParameterExistence(name.toStdString().c_str());
}




//-------------------------------------------------------------------
//
// Mute/unmute outputs
//
//-------------------------------------------------------------------


/**
 * @brief Mute a given output
 *
 * @param name
 *
 * @return  true if everything is ok, false otherwise
 */
bool IngeScapeQuick::muteOuput(QString name)
{
    bool result = false;

    if (!name.isEmpty())
    {
        if (igs_muteOutput(name.toStdString().c_str()) == 1)
        {
            result = true;
        }
    }
    else
    {
        qWarning() << "IngeScapeQuick warning: muteOuput() - name can not be empty";
    }

    return result;
}



/**
 * @brief Mute a given output
 *
 * @param name
 *
 * @return  true if everything is ok, false otherwise
 */
bool IngeScapeQuick::unmuteOuput(QString name)
{
    bool result = false;

    if (!name.isEmpty())
    {
        if (igs_unmuteOutput(name.toStdString().c_str()) == 1)
        {
            result = true;
        }
    }
    else
    {
        qWarning() << "IngeScapeQuick warning: unmuteOuput() - name can not be empty";
    }

    return result;
}



/**
 * @brief Check if a given output is muted
 *
 * @param name
 * @param qmlUpdateExtraParameter Extra parameter used to call this function in a QML binding
 *
 * @return true if this output is muted, false otherwise
 */
bool IngeScapeQuick::isOutputMuted(QString name, QVariant qmlUpdateExtraParameter)
{
    Q_UNUSED(qmlUpdateExtraParameter)
    bool result = false;

    if (!name.isEmpty())
    {
        result = igs_isOutputMuted(name.toStdString().c_str());
    }
    else
    {
        qWarning() << "IngeScapeQuick warning: isOutputMuted() - name can not be empty";
    }

    return result;
}




//-------------------------------------------------------------------
//
// Load / set / get definition
//
//-------------------------------------------------------------------


/**
 * @brief Clear our definition (i.e. remove inputs, outputs, etc.)
 * @return
 */
bool IngeScapeQuick::clearDefinition()
{
    bool result = false;

    if (igs_clearDefinition() == 1)
    {
        //-------------------------------
        //
        // Clean-up internal data
        //
        //-------------------------------

        // Clean-up inputs
        if (_inputs != NULL)
        {
            // Save our previous value
            QQmlPropertyMap* temp = _inputs;

            // Create a new inputs map
            setinputs( new IngeScapeQuickInputsPropertyMap(this) );

            // Clean-up our previous value
            delete temp;
        }
        else
        {
            // Create a new inputs map
            setinputs( new IngeScapeQuickInputsPropertyMap(this) );
        }
        // NB: OR simply clear our property map ??


        // Clean-up outputs
        if (_outputs != NULL)
        {
            // Unsubscribe to signals
            disconnect(_outputs, &QQmlPropertyMap::valueChanged, this, &IngeScapeQuick::_onOutputUpdatedFromQML);

            // Save our previous value
            QQmlPropertyMap* temp = _outputs;

            // Create a new outputs map
            QQmlPropertyMap* newValue = new IngeScapeQuickOutputsPropertyMap(this);
            setoutputs( newValue );
            if (newValue != NULL)
            {
                connect(newValue, &QQmlPropertyMap::valueChanged, this, &IngeScapeQuick::_onOutputUpdatedFromQML);
            }

            // Clean-up our previous value
            delete temp;
        }
        else
        {
            // Create a new outputs map
            QQmlPropertyMap* newValue = new IngeScapeQuickOutputsPropertyMap(this);
            setoutputs( newValue );

            if (newValue != NULL)
            {
                connect(newValue, &QQmlPropertyMap::valueChanged, this, &IngeScapeQuick::_onOutputUpdatedFromQML);
            }
        }
        // NB: OR simply clear our property map ??


        // Clean-up parameters
        if (_parameters != NULL)
        {
            // Unsubcribe to signals
            disconnect(_parameters, &QQmlPropertyMap::valueChanged, this, &IngeScapeQuick::_onParameterUpdatedFromQML);

            // Save our previous value
            QQmlPropertyMap* temp = _parameters;

            // Create a new parameter map
            QQmlPropertyMap* newValue = new IngeScapeQuickParametersPropertyMap(this);
            setparameters( newValue );
            if (newValue != NULL)
            {
               connect(newValue, &QQmlPropertyMap::valueChanged, this, &IngeScapeQuick::_onParameterUpdatedFromQML);
            }

            // Clean-up our previous value
            delete temp;
        }
        else
        {
             // Create a new parameter map
             QQmlPropertyMap* newValue = new IngeScapeQuickParametersPropertyMap(this);
             setparameters( newValue );
             if (newValue != NULL)
             {
                connect(newValue, &QQmlPropertyMap::valueChanged, this, &IngeScapeQuick::_onParameterUpdatedFromQML);
             }
        }
        // NB: OR simply clear our property map ??


        //
        // Clean-up our simple lists
        //
        _inputsList.clear();
        Q_EMIT inputsListChanged(_inputsList);
        _outputsList.clear();
        Q_EMIT outputsListChanged(_outputsList);
        _parametersList.clear();
        Q_EMIT parametersListChanged(_parametersList);


        //
        // Restore data
        //
        // - Name of our definition
        QString tempDefinitionName = definitionName();
        if (!tempDefinitionName.isEmpty())
        {
            setdefinitionName(tempDefinitionName.toStdString().c_str());
        }
        // - Descripton of our definition
        QString tempDefinitionDescription = definitionDescription();
        if (!tempDefinitionDescription.isEmpty())
        {
            igs_setDefinitionDescription(tempDefinitionDescription.toStdString().c_str());
        }
        // - Version of our description
        QString tempDefinitionVersion = definitionVersion();
        if (!tempDefinitionVersion.isEmpty())
        {
            igs_setDefinitionVersion(tempDefinitionVersion.toStdString().c_str());
        }



        // Notify that our definition has been cleared
        Q_EMIT definitionCleared();
    }
    else
    {
        qWarning() << "IngeScapeQuick warning: failed to clear definition";
    }

    return result;
}




//-------------------------------------------------------------------
//
// Edit the definition: create or remove IOP (inoput, output, parameter)
//
//-------------------------------------------------------------------

/**
 * @brief Create a new integer input
 *
 * @param name
 * @param value
 *
 * @return true if an input is created, false otherwise (i.e. we already have an input with this name)
 */
bool IngeScapeQuick::createInputInt(QString name, int value)
{
    return createInputInt(name, value, NULL);
}


/**
 * @brief Create a new integer input
 *
 * @param name
 * @param value
 * @param warning
 *
 * @return true if an input is created, false otherwise (i.e. we already have an input with this name)
 */
bool IngeScapeQuick::createInputInt(QString name, int value, QString* warning)
{
    return _createInput(name, IngeScapeIopType::INTEGER, QVariant(value), &value, sizeof(int), warning);
}


/**
 * @brief Create a new double input
 *
 * @param name
 * @param value
 *
 * @return true if an input is created, false otherwise (i.e. we already have an input with this name)
 */
bool IngeScapeQuick::createInputDouble(QString name, double value)
{
    return createInputDouble(name, value, NULL);
}


/**
 * @brief Create a new double input
 *
 * @param name
 * @param value
 * @param warning
 *
 * @return true if an input is created, false otherwise (i.e. we already have an input with this name)
 */
bool IngeScapeQuick::createInputDouble(QString name, double value, QString* warning)
{
    return _createInput(name, IngeScapeIopType::DOUBLE, QVariant(value), &value, sizeof(double), warning);
}


/**
 * @brief Create a new string input
 *
 * @param name
 * @param value
 * @param warning
 *
 * @return true if an input is created, false otherwise (i.e. we already have an input with this name)
 */
bool IngeScapeQuick::createInputString(QString name, QString value)
{
    return createInputString(name, value, NULL);
}


/**
 * @brief Create a new string input
 *
 * @param name
 * @param value
 * @param warning
 *
 * @return true if an input is created, false otherwise (i.e. we already have an input with this name)
 */
bool IngeScapeQuick::createInputString(QString name, QString value, QString* warning)
{
    std::string stdString = value.toStdString();
    const char* cValue = stdString.c_str();
    int cValueLength = ((cValue != NULL) ? strlen(cValue) : 0);

    return _createInput(name, IngeScapeIopType::STRING, QVariant(value), (void *)cValue, (cValueLength + 1) * sizeof(char), warning);
}


/**
 * @brief Create a new boolean input
 *
 * @param name
 * @param value
 *
 * @return true if an input is created, false otherwise (i.e. we already have an input with this name)
 */
bool IngeScapeQuick::createInputBool(QString name, bool value)
{
    return createInputBool(name, value, NULL);
}


/**
 * @brief Create a new boolean input
 *
 * @param name
 * @param value
 * @param warning Warning message if something went wrong
 *
 * @return true if an input is created, false otherwise (i.e. we already have an input with this name)
 */
bool IngeScapeQuick::createInputBool(QString name, bool value, QString* warning)
{
    return _createInput(name, IngeScapeIopType::BOOLEAN, QVariant(value), &value, sizeof(bool), warning);
}


/**
 * @brief Create a new impulsion input
 *
 * @param name
 *
 * @return true if an input is created, false otherwise (i.e. we already have an input with this name)
 */
bool IngeScapeQuick::createInputImpulsion(QString name)
{
    return createInputImpulsion(name, NULL);
}


/**
 * @brief Create a new impulsion input
 *
 * @param name
 * @param warning Warning message if something went wrong
 *
 * @return true if an input is created, false otherwise (i.e. we already have an input with this name)
 */
bool IngeScapeQuick::createInputImpulsion(QString name, QString* warning)
{
    return _createInput(name, IngeScapeIopType::IMPULSION, QVariant(""), NULL, 0, warning);
}


/**
 * @brief Create a new data input
 *
 * @param name
 * @param value
 *
 * @return true if an input is created, false otherwise (i.e. we already have an input with this name)
 */
bool IngeScapeQuick::createInputData(QString name, void* value)
{
    return createInputData(name, value, NULL);
}


/**
 * @brief Create a new data input
 *
 * @param name
 * @param value
 * @param warning Warning message if something went wrong
 *
 * @return true if an input is created, false otherwise (i.e. we already have an input with this name)
 */
bool IngeScapeQuick::createInputData(QString name, void* value, QString* warning)
{
    Q_UNUSED(name)
    Q_UNUSED(value)
    Q_UNUSED(warning)

    bool result = false;

    qDebug() << Q_FUNC_INFO << "NOT YET IMPLEMENTED. Can not create input" << name << "with type DATA";

    return result;
}



/**
 * @brief Create a new integer output
 *
 * @param name
 * @param value
 *
 * @return true if an output is created, false otherwise (i.e. we already have an output with this name)
 */
bool IngeScapeQuick::createOutputInt(QString name, int value)
{
    return createOutputInt(name, value, NULL);
}


/**
 * @brief Create a new integer output
 *
 * @param name
 * @param value
 * @param warning Warning message if something went wrong
 *
 * @return true if an output is created, false otherwise (i.e. we already have an output with this name)
 */
bool IngeScapeQuick::createOutputInt(QString name, int value, QString* warning)
{
    return _createOutput(name, IngeScapeIopType::INTEGER, QVariant(value), &value, sizeof(int), warning);
}


/**
 * @brief Create a new double output
 *
 * @param name
 * @param value
 *
 * @return true if an output is created, false otherwise (i.e. we already have an output with this name)
 */
bool IngeScapeQuick::createOutputDouble(QString name, double value)
{
    return createOutputDouble(name, value, NULL);
}


/**
 * @brief Create a new double output
 *
 * @param name
 * @param value
 * @param warning Warning message if something went wrong
 *
 * @return true if an output is created, false otherwise (i.e. we already have an output with this name)
 */
bool IngeScapeQuick::createOutputDouble(QString name, double value, QString* warning)
{
    return _createOutput(name, IngeScapeIopType::DOUBLE, QVariant(value), &value, sizeof(double), warning);
}


/**
 * @brief Create a new string output
 *
 * @param name
 * @param value
 *
 * @return true if an output is created, false otherwise (i.e. we already have an output with this name)
 */
bool IngeScapeQuick::createOutputString(QString name, QString value)
{
    return createOutputString(name, value, NULL);
}


/**
 * @brief Create a new string output
 *
 * @param name
 * @param value
 * @param warning Warning message if something went wrong
 *
 * @return true if an output is created, false otherwise (i.e. we already have an output with this name)
 */
bool IngeScapeQuick::createOutputString(QString name, QString value, QString* warning)
{
    std::string stdString = value.toStdString();
    const char* cValue = stdString.c_str();
    int cValueLength = ((cValue != NULL) ? strlen(cValue) : 0);

    return _createOutput(name, IngeScapeIopType::STRING, QVariant(value), (void *)cValue, (cValueLength + 1) * sizeof(char), warning);
}


/**
 * @brief Create a new boolean output
 *
 * @param name
 * @param value
 *
 * @return true if an output is created, false otherwise (i.e. we already have an output with this name)
 */
bool IngeScapeQuick::createOutputBool(QString name, bool value)
{
    return createOutputBool(name, value, NULL);
}


/**
 * @brief Create a new boolean output
 *
 * @param name
 * @param value
 * @param warning Warning message if something went wrong
 *
 * @return true if an output is created, false otherwise (i.e. we already have an output with this name)
 */
bool IngeScapeQuick::createOutputBool(QString name, bool value, QString* warning)
{
    return _createOutput(name, IngeScapeIopType::BOOLEAN, QVariant(value), &value, sizeof(bool), warning);
}


/**
 * @brief Create a new impulsion output
 *
 * @param name
 *
 * @return true if an output is created, false otherwise (i.e. we already have an output with this name)
 */
bool IngeScapeQuick::createOutputImpulsion(QString name)
{
    return createOutputImpulsion(name, NULL);
}


/**
 * @brief Create a new impulsion output
 *
 * @param name
 * @param warning Warning message if something went wrong
 *
 * @return true if an output is created, false otherwise (i.e. we already have an output with this name)
 */
bool IngeScapeQuick::createOutputImpulsion(QString name, QString* warning)
{
    return _createOutput(name, IngeScapeIopType::IMPULSION, QVariant(""), 0, 0, warning);
}


/**
 * @brief Create a new data output
 *
 * @param name
 * @param value
 *
 * @return true if an output is created, false otherwise (i.e. we already have an output with this name)
 */
bool IngeScapeQuick::createOutputData(QString name, void* value)
{
    return createOutputData(name, value, NULL);
}


/**
 * @brief Create a new data output
 *
 * @param name
 * @param value
 * @param warning Warning message if something went wrong
 *
 * @return true if an output is created, false otherwise (i.e. we already have an output with this name)
 */
bool IngeScapeQuick::createOutputData(QString name, void* value, QString* warning)
{
    Q_UNUSED(name)
    Q_UNUSED(value)
    Q_UNUSED(warning)

    bool result = false;

    qWarning() << "IngeScapeQuick warning: createOutputData() NOT YET IMPLEMENTED. Can not create output" << name << "with type DATA";

    return result;
}




/**
 * @brief Create a new integer parameter
 *
 * @param name
 * @param value
 *
 * @return true if a parameter is created, false otherwise (i.e. we already have a parameter with this name)
 */
bool IngeScapeQuick::createParameterInt(QString name, int value)
{
    return createParameterInt(name, value, NULL);
}


/**
 * @brief Create a new integer parameter
 *
 * @param name
 * @param value
 * @param warning Warning message if something went wrong
 *
 * @return true if a parameter is created, false otherwise (i.e. we already have a parameter with this name)
 */
bool IngeScapeQuick::createParameterInt(QString name, int value, QString* warning)
{
    return _createParameter(name, IngeScapeIopType::INTEGER, QVariant(value), &value, sizeof(int), warning);
}


/**
 * @brief Create a new double parameter
 *
 * @param name
 * @param value
 *
 * @return true if a parameter is created, false otherwise (i.e. we already have a parameter with this name)
 */
bool IngeScapeQuick::createParameterDouble(QString name, double value)
{
    return createParameterDouble(name, value, NULL);
}


/**
 * @brief Create a new double parameter
 *
 * @param name
 * @param value
 * @param warning Warning message if something went wrong
 *
 * @return true if a parameter is created, false otherwise (i.e. we already have a parameter with this name)
 */
bool IngeScapeQuick::createParameterDouble(QString name, double value, QString* warning)
{
    return _createParameter(name, IngeScapeIopType::DOUBLE, QVariant(value), &value, sizeof(double), warning);
}


/**
 * @brief Create a new string parameter
 *
 * @param name
 * @param value
 *
 * @return true if a parameter is created, false otherwise (i.e. we already have a parameter with this name)
 */
bool IngeScapeQuick::createParameterString(QString name, QString value)
{
    return createParameterString(name, value, NULL);
}


/**
 * @brief Create a new string parameter
 *
 * @param name
 * @param value
 * @param warning Warning message if something went wrong
 *
 * @return true if a parameter is created, false otherwise (i.e. we already have a parameter with this name)
 */
bool IngeScapeQuick::createParameterString(QString name, QString value, QString* warning)
{
    std::string stdString = value.toStdString();
    const char* cValue = stdString.c_str();
    int cValueLength = ((cValue != NULL) ? strlen(cValue) : 0);

    return _createParameter(name, IngeScapeIopType::STRING, QVariant(value), (void *)cValue, (cValueLength + 1) * sizeof(char), warning);
 }


/**
 * @brief Create a new boolean parameter
 *
 * @param name
 * @param value
 *
 * @return true if a parameter is created, false otherwise (i.e. we already have a parameter with this name)
 */
bool IngeScapeQuick::createParameterBool(QString name, bool value)
{
    return createParameterBool(name, value, NULL);
}


/**
 * @brief Create a new boolean parameter
 *
 * @param name
 * @param value
 * @param warning Warning message if something went wrong
 *
 * @return true if a parameter is created, false otherwise (i.e. we already have a parameter with this name)
 */
bool IngeScapeQuick::createParameterBool(QString name, bool value, QString* warning)
{
    return _createParameter(name, IngeScapeIopType::BOOLEAN, QVariant(value), &value, sizeof(bool), warning);
}


/**
 * @brief Create a new data parameter
 *
 * @param name
 * @param value
 *
 * @return true if a parameter is created, false otherwise (i.e. we already have a parameter with this name)
 */
bool IngeScapeQuick::createParameterData(QString name, void* value)
{
    return createParameterData(name, value, NULL);
}


/**
 * @brief Create a new data parameter
 *
 * @param name
 * @param value
 * @param warning Warning message if something went wrong
 *
 * @return true if a parameter is created, false otherwise (i.e. we already have a parameter with this name)
 */
bool IngeScapeQuick::createParameterData(QString name, void* value, QString* warning)
{
    Q_UNUSED(name)
    Q_UNUSED(value)
    Q_UNUSED(warning)

    bool result = false;

    qDebug() << "IngeScapeQuick warning: createParameterData() NOT YET IMPLEMENTED. Can not create parameter" << name << "with type DATA";

    return result;
}



/**
 * @brief Remove an input
 *
 * @param name
 *
 * @return true if this input is removed, false otherwise
 */
bool IngeScapeQuick::removeInput(QString name)
{
    bool result = false;

    // Ensure that we have a valid name
    if (!name.isEmpty())
    {
        // Try to remove input
        if (igs_removeInput(name.toStdString().c_str()) == 1)
        {
            result = true;
        }
        else
        {
            qWarning() << "IngeScapeQuick warning: failed to remove input"
                       << name << "via removeInput";
        }
    }
    else
    {
        qWarning() << "IngeScapeQuick warning: removeInput() can not remove an input without a name";
    }

    return result;
}



/**
 * @brief Remove an output
 *
 * @param name
 *
 * @return true if this output is removed, false otherwise
 */
bool IngeScapeQuick::removeOutput(QString name)
{
    bool result = false;

    // Ensure that we have a valid name
    if (!name.isEmpty())
    {
        // Try to remove ouput
        if (igs_removeOutput(name.toStdString().c_str()) == 1)
        {
            result = true;
        }
        else
        {
            qWarning() << "IngeScapeQuick warning: failed to remove output"
                       << name << "via removeOutput";
        }
    }
    else
    {
        qWarning() << "IngeScapeQuick warning: removeOutput() can not remove an output without a name";
    }

    return result;
}



/**
 * @brief Remove a parameter
 *
 * @param name
 *
 * @return true if this parameter is removed, false otherwise
 */
bool IngeScapeQuick::removeParameter(QString name)
{
    bool result = false;

    // Ensure that we have a valid name
    if (!name.isEmpty())
    {
        // Try to remove parameter
        if (igs_removeParameter(name.toStdString().c_str()) == 1)
        {
            result = true;
        }
        else
        {
            qWarning() << "IngeScapeQuick warning: failed to remove parameter"
                       << name << "via removeParameter";
        }
    }
    else
    {
        qWarning() << "IngeScapeQuick warning: removeParameter() can not remove a parameter without a name";
    }

    return result;
}




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

/**
 * @brief Get network adapters with broadcast capabilities
 *
 * @param qmlUpdateExtraParameter Extra parameter used to call this function in a QML binding
 *
 * @return
 */
QStringList IngeScapeQuick::getNetdevicesList(QVariant qmlUpdateExtraParameter)
{
    Q_UNUSED(qmlUpdateExtraParameter)

    QStringList result;

    // Get all devices
    char **devices = NULL;
    int numberOfDevices = 0;

    igs_getNetdevicesList(&devices, &numberOfDevices);
    for (int index = 0; index < numberOfDevices; index++)
    {
        result.append( QString(devices[index]) );
    }

    // Clean-up
    igs_freeNetdevicesList(devices, numberOfDevices);

    return result;
}



/**
 * @brief Print (or save) debugging information (loglevel = trace)
 * @param text
 */
void IngeScapeQuick::trace(QString text)
{
    igs_trace(text.toStdString().c_str());
}



/**
 * @brief Print (or save) debugging information (loglevel = debug)
 * @param text
 */
void IngeScapeQuick::debug(QString text)
{
    igs_debug(text.toStdString().c_str());
}



/**
 * @brief Print (or save) debugging information (loglevel = info)
 * @param text
 */
void IngeScapeQuick::info(QString text)
{
    igs_info(text.toStdString().c_str());
}



/**
 * @brief Print (or save) debugging information (loglevel = warn)
 * @param text
 */
void IngeScapeQuick::warn(QString text)
{
    igs_warn(text.toStdString().c_str());
}



/**
 * @brief Print (or save) debugging information (loglevel = error)
 * @param text
 */
void IngeScapeQuick::error(QString text)
{
    igs_error(text.toStdString().c_str());
}



/**
 * @brief Print (or save) debugging information (loglevel = fatal)
 * @param text
 */
void IngeScapeQuick::fatal(QString text)
{
    igs_fatal(text.toStdString().c_str());
}




//-------------------------------------------------------------------
//
// Extra QML API
//
//-------------------------------------------------------------------


/**
 * @brief Check if an IOP name is valid or if it can create conflicts with Qt internal symbols
 * @param name
 * @return
 */
bool IngeScapeQuick::checkIfIopNameIsValid(const QString& name)
{
    return (
             (name != QLatin1String("QObject"))
             &
             (name != QLatin1String("destroyed"))
             &&
             (name != QLatin1String("deleteLater"))
             &&
             (name != QLatin1String("keys"))
             &&
             (name != QLatin1String("valueChanged"))
            );
}




//--------------------------------------------------------
//
// Internal API - MUST not be used outside of IngeScapeQuick
//
//--------------------------------------------------------


/**
 * @brief Set our isMuted flag based on observeMute
 * @param value
 */
void IngeScapeQuick::_internal_setIsMuted(bool value)
{
    if (_isMuted != value)
    {
        // Save value
        _isMuted = value;

        // Notify change
        Q_EMIT isMutedChanged(value);
    }
}



/**
 * @brief Set our isFrozen flag based on observeFreeze
 * @param value
 */
void IngeScapeQuick::_internal_setIsFrozen(bool value)
{
    if (_isFrozen != value)
    {
        // Save value
        _isFrozen = value;

        // Notify change
        Q_EMIT isFrozenChanged(value);
    }
}


/**
 * @brief Check if we need to update QML in our observeOuput callback
 * @return
 */
bool IngeScapeQuick::_internal_needsToUpdateQmlInObserveOutput()
{
    bool result = false;

    _observeOutputNeedToUpdateQMLMutex.lock();
    result = _observeOutputNeedToUpdateQML;
    _observeOutputNeedToUpdateQMLMutex.unlock();

    return result;
}


/**
 * @brief Check if we need to update QML in our observeParameter callback
 * @return
 */
bool IngeScapeQuick::_internal_needsToUpdateQmlInObserveParameter()
{
    bool result = false;

    _observeParameterNeedToUpdateQMLMutex.lock();
    result = _observeParameterNeedToUpdateQML;
    _observeParameterNeedToUpdateQMLMutex.unlock();

    return result;
}



//-------------------------------------------------------------------
//
// Protected methods
//
//-------------------------------------------------------------------



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
bool IngeScapeQuick::_createInput(QString name, IngeScapeIopType::Value type, QVariant qmlValue, void* cValue, size_t cSize, QString* warning)
{
    bool result = false;

    // Check if we have a name
    if (!name.isEmpty())
    {
        // Check if it is a valid IOP name
        if (checkIfIopNameIsValid(name))
        {
            std::string stdName = name.toStdString();
            const char* cName = stdName.c_str();

            // Check if we must create a IngeScape input
            iopType_t existingIopType = igs_getTypeForInput(cName);
            if (existingIopType <= 0)
            {
                //
                // This ingescape input does not exists
                //

                // Try to create a new input
                if (igs_createInput(cName, enumIngeScapeIopTypeToEnumIopType_t(type), cValue, cSize) == 1)
                {
                    // Observe this new input
                    if (igs_observeInput(cName, IngeScapeQuick_callbackObserveInput, this) != 1)
                    {
                        QString warningMessage = QString("failed to observe input '%1' with type %2").arg(name).arg(IngeScapeIopType::staticEnumToKey(type));

                        qWarning() << "IngeScapeQuick warning:" << warningMessage;

                        if (warning != NULL)
                        {
                            *warning = warningMessage;
                        }
                    }


                    // Add it to the list of QML dynamic properties
                    if (_inputs != NULL)
                    {
                        // Update QML
                        // NB: special case for impulsion properties because we don't want to trigger them at startup
                        if (type == IngeScapeIopType::IMPULSION)
                        {
                            _inputs->blockSignals(true);
                            _inputs->insert(name, qmlValue);
                            _inputs->blockSignals(false);
                        }
                        else
                        {
                            _inputs->insert(name, qmlValue);
                        }
                    }
                    // Else: should not happen. Otherwise, it means that our controller is destroyed


                    // Update our list of inputs
                    _inputsList.append(name);
                    Q_EMIT inputsListChanged(_inputsList);


                    // Everything is ok
                    result = true;
                }
                else
                {
                    QString warningMessage = QString("failed to create input '%1' with type %2").arg(name).arg(IngeScapeIopType::staticEnumToKey(type));

                    qWarning() << "IngeScapeQuick warning:" << warningMessage;

                    if (warning != NULL)
                    {
                        *warning = warningMessage;
                    }
                }
            }
            else
            {
                //
                // This ingescape input already exists
                //

                // Check if the existing property
                IngeScapeIopType::Value existingIngeScapeIopType = enumIopType_tToIngeScapeIopType(existingIopType);
                if (type == existingIngeScapeIopType)
                {
                    //
                    // Same type
                    //

                    // Check if we must add it to the list of QML dynamic properties
                    if (_inputs != NULL)
                    {
                        // Update QML if needed
                        if (!_inputs->contains(name))
                        {
                            // NB: special case for impulsion properties because we don't want to trigger them at startup
                            if (type == IngeScapeIopType::IMPULSION)
                            {
                                _inputs->blockSignals(true);
                                _inputs->insert(name, qmlValue);
                                _inputs->blockSignals(false);
                            }
                            else
                            {
                                _inputs->insert(name, qmlValue);
                            }
                        }
                        else
                        {
                            // Nothing to do

                            QString warningMessage = QString("input '%1' already exists with the same type (%2)").arg(name).arg(IngeScapeIopType::staticEnumToKey(type));

                            qWarning() << "IngeScapeQuick warning:" << warningMessage << "- Its initial value will not be changed to" << qmlValue;

                            if (warning != NULL)
                            {
                                *warning = warningMessage;
                            }
                        }
                    }
                    // Else: should not happen. Otherwise, it means that our controller is destroyed


                    // Update our list of inputs if needed
                    if (!_inputsList.contains(name))
                    {
                        _inputsList.append(name);
                        Q_EMIT inputsListChanged(_inputsList);
                    }


                    // Everything is ok
                    result = true;
                }
                else
                {
                    //
                    // Different types
                    //

                    // Check if types are compatible
                    if (IngeScapeQuickUtils::checkIfIopTypesAreCompatible(type, existingIngeScapeIopType))
                    {
                        //
                        // Compatible types
                        //

                        // Check if we must add it to the list of QML dynamic properties
                        if (_inputs != NULL)
                        {
                            // Update QML if needed
                            if (!_inputs->contains(name))
                            {
                                // NB: special case for impulsion properties because we don't want to trigger them at startup
                                if (type == IngeScapeIopType::IMPULSION)
                                {
                                    _inputs->blockSignals(true);
                                    _inputs->insert(name, qmlValue);
                                    _inputs->blockSignals(false);
                                }
                                else
                                {
                                    _inputs->insert(name, qmlValue);
                                }
                            }
                            else
                            {
                                // Nothing to do

                                QString warningMessage = QString("input '%1' already exists with a compatible type %2 - Its type will not be changed to %3")
                                        .arg(name)
                                        .arg(IngeScapeIopType::staticEnumToKey(existingIngeScapeIopType))
                                        .arg(IngeScapeIopType::staticEnumToKey(type));

                                qWarning() << "IngeScapeQuick warning:" << warningMessage <<  "and its initial value will not be changed to" << qmlValue;

                                if (warning != NULL)
                                {
                                    *warning = warningMessage;
                                }
                            }
                        }
                        // Else: should not happen. Otherwise, it means that our controller is destroyed


                        // Update our list of inputs if needed
                        if (!_inputsList.contains(name))
                        {
                            _inputsList.append(name);
                            Q_EMIT inputsListChanged(_inputsList);
                        }


                        // Everything is ok
                        result = true;
                    }
                    else
                    {
                        //
                        // Incompatible types
                        //

                        QString warningMessage = QString("input '%1' already exists with type %2 that is not compatible with the required type %3")
                                .arg(name)
                                .arg(IngeScapeIopType::staticEnumToKey(existingIngeScapeIopType))
                                .arg(IngeScapeIopType::staticEnumToKey(type));

                        qWarning() << "IngeScapeQuick warning:" << warningMessage;

                        if (warning != NULL)
                        {
                            *warning = warningMessage;
                        }
                    }
                }
                // End of if (type == existingIngeScapeIopType)
            }
            // End of if (existingIopType <= 0)
        }
        else
        {
            //
            // Invalid name for Qt
            //

            QString warningMessage = QString("can not create input - '%1' is an invalid input name, it conflicts with Qt internal symbols").arg(name);

            qWarning() << "IngeScapeQuick warning:" << warningMessage;

            if (warning != NULL)
            {
                *warning = warningMessage;
            }
         }
    }
    else
    {
        //
        // Empty name
        //

         QString warningMessage = QString("can not create an input with an empty name");

         qWarning() << "IngeScapeQuick warning:" << warningMessage;

         if (warning != NULL)
         {
             *warning = warningMessage;
         }
    }

    return result;
}



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
 * @return true if an output is created, false otherwise
 */
bool IngeScapeQuick::_createOutput(QString name, IngeScapeIopType::Value type, QVariant qmlValue, void* cValue, size_t cSize, QString* warning)
{
    bool result = false;

    // Check if we have a name
    if (!name.isEmpty())
    {
        // Check if it is a valid IOP name
        if (checkIfIopNameIsValid(name))
        {
            std::string stdName = name.toStdString();
            const char* cName = stdName.c_str();

            // Check if we must create a IngeScape output
            iopType_t existingIopType = igs_getTypeForOutput(cName);
            if (existingIopType <= 0)
            {
                //
                // This ingescape output does not exists
                //

                // Try to create a new output
                if (igs_createOutput(cName, enumIngeScapeIopTypeToEnumIopType_t(type), cValue, cSize) == 1)
                {
                    // Observe this new output
                    if (igs_observeOutput(cName, IngeScapeQuick_callbackObserveOutput, this) != 1)
                    {
                        QString warningMessage = QString("failed to observe output '%1' with type %2").arg(name).arg(IngeScapeIopType::staticEnumToKey(type));

                        qWarning() << "IngeScapeQuick warning:" << warningMessage;

                        if (warning != NULL)
                        {
                            *warning = warningMessage;
                        }
                    }


                    // Add it to the list of QML dynamic properties
                    if (_outputs != NULL)
                    {
                        // Update QML
                        // NB: special case for impulsion properties because we don't want to trigger them at startup
                        if (type == IngeScapeIopType::IMPULSION)
                        {
                            _outputs->blockSignals(true);
                            _outputs->insert(name, qmlValue);
                            _outputs->blockSignals(false);
                        }
                        else
                        {
                            _outputs->insert(name, qmlValue);
                        }
                    }
                    // Else: should not happen. Otherwise, it means that our controller is destroyed


                    // Update our list of outputs
                    _outputsList.append(name);
                    Q_EMIT inputsListChanged(_outputsList);


                    // Everything is ok
                    result = true;
                }
                else
                {
                    QString warningMessage = QString("failed to create output '%1' with type %2").arg(name).arg(IngeScapeIopType::staticEnumToKey(type));

                    qWarning() << "IngeScapeQuick warning:" << warningMessage;

                    if (warning != NULL)
                    {
                        *warning = warningMessage;
                    }
                }
            }
            else
            {
                //
                // This ingescape input already exists
                //

                // Check if the existing property is compatible
                IngeScapeIopType::Value existingIngeScapeIopType = enumIopType_tToIngeScapeIopType(existingIopType);
                if (type == existingIngeScapeIopType)
                {
                    //
                    // Same type
                    //

                    // Check if we must add it to the list of QML dynamic properties
                    if (_outputs != NULL)
                    {
                        // Update QML if needed
                        if (!_outputs->contains(name))
                        {
                            // NB: special case for impulsion properties because we don't want to trigger them at startup
                            if (type == IngeScapeIopType::IMPULSION)
                            {
                                _outputs->blockSignals(true);
                                _outputs->insert(name, qmlValue);
                                _outputs->blockSignals(false);
                            }
                            else
                            {
                                _outputs->insert(name, qmlValue);
                            }
                        }
                        else
                        {
                            // Nothing to do

                            QString warningMessage = QString("output '%1' already exists with the same type (%2)").arg(name).arg(IngeScapeIopType::staticEnumToKey(type));

                            qWarning() << "IngeScapeQuick warning:" << warningMessage << "- Its initial value will not be changed to" << qmlValue;

                            if (warning != NULL)
                            {
                                *warning = warningMessage;
                            }
                        }
                    }
                    // Else: should not happen. Otherwise, it means that our controller is destroyed


                    // Update our list of outputs if needed
                    if (!_outputsList.contains(name))
                    {
                        _outputsList.append(name);
                        Q_EMIT inputsListChanged(_outputsList);
                    }


                    // Everything is ok
                    result = true;
                }
                else
                {
                    //
                    // Different types
                    //

                    // Check if types are compatible
                    if (IngeScapeQuickUtils::checkIfIopTypesAreCompatible(type, existingIngeScapeIopType))
                    {
                        //
                        // Compatible types
                        //

                        // Check if we must add it to the list of QML dynamic properties
                        if (_inputs != NULL)
                        {
                            // Update QML if needed
                            if (!_outputs->contains(name))
                            {
                                // NB: special case for impulsion properties because we don't want to trigger them at startup
                                if (type == IngeScapeIopType::IMPULSION)
                                {
                                    _outputs->blockSignals(true);
                                    _outputs->insert(name, qmlValue);
                                    _outputs->blockSignals(false);
                                }
                                else
                                {
                                    _outputs->insert(name, qmlValue);
                                }
                            }
                            else
                            {
                                // Nothing to do

                                QString warningMessage = QString("output '%1' already exists with a compatible type %2 - Its type will not be changed to %3")
                                        .arg(name)
                                        .arg(IngeScapeIopType::staticEnumToKey(existingIngeScapeIopType))
                                        .arg(IngeScapeIopType::staticEnumToKey(type));

                                qWarning() << "IngeScapeQuick warning:" << warningMessage <<  "and its initial value will not be changed to" << qmlValue;

                                if (warning != NULL)
                                {
                                    *warning = warningMessage;
                                }
                            }
                        }
                        // Else: should not happen. Otherwise, it means that our controller is destroyed


                        // Update our list of outputs if needed
                        if (!_outputsList.contains(name))
                        {
                            _outputsList.append(name);
                            Q_EMIT inputsListChanged(_outputsList);
                        }


                        // Everything is ok
                        result = true;
                    }
                    else
                    {
                        //
                        // Incompatible types
                        //

                        QString warningMessage = QString("output '%1' already exists with type %2 that is not compatible with the required type %3")
                                .arg(name)
                                .arg(IngeScapeIopType::staticEnumToKey(existingIngeScapeIopType))
                                .arg(IngeScapeIopType::staticEnumToKey(type));

                        qWarning() << "IngeScapeQuick warning:" << warningMessage;

                        if (warning != NULL)
                        {
                            *warning = warningMessage;
                        }
                    }
                }
                // End of if (type == existingIngeScapeIopType)
            }
            // End of if (existingIopType <= 0)
        }
        else
        {
            //
            // Invalid name for Qt
            //

            QString warningMessage = QString("can not create output - '%1' is an invalid output name, it conflicts with Qt internal symbols").arg(name);

            qWarning() << "IngeScapeQuick warning:" << warningMessage;

            if (warning != NULL)
            {
                *warning = warningMessage;
            }
         }
    }
    else
    {
        //
        // Empty name
        //

         QString warningMessage = QString("can not create an output with an empty name");

         qWarning() << "IngeScapeQuick warning:" << warningMessage;

         if (warning != NULL)
         {
             *warning = warningMessage;
         }
    }

    return result;
}



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
 * @return true if a parameter is created, false otherwise
 */
bool IngeScapeQuick::_createParameter(QString name, IngeScapeIopType::Value type, QVariant qmlValue, void* cValue, size_t cSize, QString* warning)
{
    bool result = false;

    // Check if we have a name
    if (!name.isEmpty())
    {
        // Check if it is a valid IOP name
        if (checkIfIopNameIsValid(name))
        {
            std::string stdName = name.toStdString();
            const char* cName = stdName.c_str();

            // Check if we must create a IngeScape parameter
            iopType_t existingIopType = igs_getTypeForParameter(cName);
            if (existingIopType <= 0)
            {
                //
                // This ingescape parameter does not exists
                //

                // Try to create a new parameter
                if (igs_createParameter(cName, enumIngeScapeIopTypeToEnumIopType_t(type), cValue, cSize) == 1)
                {
                    // Observe this new parameter
                    if (igs_observeParameter(cName, IngeScapeQuick_callbackObserveParameter, this) != 1)
                    {
                        QString warningMessage = QString("failed to observe parameter '%1' with type %2").arg(name).arg(IngeScapeIopType::staticEnumToKey(type));

                        qWarning() << "IngeScapeQuick warning:" << warningMessage;

                        if (warning != NULL)
                        {
                            *warning = warningMessage;
                        }
                    }


                    // Add it to the list of QML dynamic properties
                    if (_parameters != NULL)
                    {
                        // Update QML
                        // NB: special case for impulsion properties because we don't want to trigger them at startup
                        if (type == IngeScapeIopType::IMPULSION)
                        {
                            _parameters->blockSignals(true);
                            _parameters->insert(name, qmlValue);
                            _parameters->blockSignals(false);
                        }
                        else
                        {
                            _parameters->insert(name, qmlValue);
                        }
                    }
                    // Else: should not happen. Otherwise, it means that our controller is destroyed


                    // Update our list of parameters
                    _parametersList.append(name);
                    Q_EMIT inputsListChanged(_parametersList);


                    // Everything is ok
                    result = true;
                }
                else
                {
                    QString warningMessage = QString("failed to create parameter '%1' with type %2").arg(name).arg(IngeScapeIopType::staticEnumToKey(type));

                    qWarning() << "IngeScapeQuick warning:" << warningMessage;

                    if (warning != NULL)
                    {
                        *warning = warningMessage;
                    }
                }
            }
            else
            {
                //
                // This ingescape parameter already exists
                //

                // Check if the existing property is compatible
                IngeScapeIopType::Value existingIngeScapeIopType = enumIopType_tToIngeScapeIopType(existingIopType);
                if (type == existingIngeScapeIopType)
                {
                    //
                    // Same type
                    //

                    // Check if we must add it to the list of QML dynamic properties
                    if (_parameters != NULL)
                    {
                        // Update QML if needed
                        if (!_parameters->contains(name))
                        {
                            // NB: special case for impulsion properties because we don't want to trigger them at startup
                            if (type == IngeScapeIopType::IMPULSION)
                            {
                                _parameters->blockSignals(true);
                                _parameters->insert(name, qmlValue);
                                _parameters->blockSignals(false);
                            }
                            else
                            {
                                _parameters->insert(name, qmlValue);
                            }
                        }
                        else
                        {
                            // Nothing to do

                            QString warningMessage = QString("parameter '%1' already exists with the same type (%2)").arg(name).arg(IngeScapeIopType::staticEnumToKey(type));

                            qWarning() << "IngeScapeQuick warning:" << warningMessage << "- Its initial value will not be changed to" << qmlValue;

                            if (warning != NULL)
                            {
                                *warning = warningMessage;
                            }
                        }
                    }
                    // Else: should not happen. Otherwise, it means that our controller is destroyed


                    // Update our list of parameters if needed
                    if (!_parametersList.contains(name))
                    {
                        _parametersList.append(name);
                        Q_EMIT inputsListChanged(_parametersList);
                    }


                    // Everything is ok
                    result = true;
                }
                else
                {
                    //
                    // Different types
                    //

                    // Check if types are compatible
                    if (IngeScapeQuickUtils::checkIfIopTypesAreCompatible(type, existingIngeScapeIopType))
                    {
                        //
                        // Compatible types
                        //

                        // Check if we must add it to the list of QML dynamic properties
                        if (_parameters != NULL)
                        {
                            // Update QML if needed
                            if (!_parameters->contains(name))
                            {
                                // NB: special case for impulsion properties because we don't want to trigger them at startup
                                if (type == IngeScapeIopType::IMPULSION)
                                {
                                    _parameters->blockSignals(true);
                                    _parameters->insert(name, qmlValue);
                                    _parameters->blockSignals(false);
                                }
                                else
                                {
                                    _parameters->insert(name, qmlValue);
                                }
                            }
                            else
                            {
                                // Nothing to do

                                QString warningMessage = QString("parameter '%1' already exists with a compatible type %2 - Its type will not be changed to %3")
                                        .arg(name)
                                        .arg(IngeScapeIopType::staticEnumToKey(existingIngeScapeIopType))
                                        .arg(IngeScapeIopType::staticEnumToKey(type));

                                qWarning() << "IngeScapeQuick warning:" << warningMessage <<  "and its initial value will not be changed to" << qmlValue;

                                if (warning != NULL)
                                {
                                    *warning = warningMessage;
                                }
                            }
                        }
                        // Else: should not happen. Otherwise, it means that our controller is destroyed


                        // Update our list of parameters if needed
                        if (!_parametersList.contains(name))
                        {
                            _parametersList.append(name);
                            Q_EMIT inputsListChanged(_parametersList);
                        }


                        // Everything is ok
                        result = true;
                    }
                    else
                    {
                        //
                        // Incompatible types
                        //

                        QString warningMessage = QString("parameter '%1' already exists with type %2 that is not compatible with the required type %3")
                                .arg(name)
                                .arg(IngeScapeIopType::staticEnumToKey(existingIngeScapeIopType))
                                .arg(IngeScapeIopType::staticEnumToKey(type));

                        qWarning() << "IngeScapeQuick warning:" << warningMessage;

                        if (warning != NULL)
                        {
                            *warning = warningMessage;
                        }
                    }
                }
                // End of if (type == existingIngeScapeIopType)
            }
            // End of if (existingIopType <= 0)
        }
        else
        {
            //
            // Invalid name for Qt
            //

            QString warningMessage = QString("can not create parameter - '%1' is an invalid parameter name, it conflicts with Qt internal symbols").arg(name);

            qWarning() << "IngeScapeQuick warning:" << warningMessage;

            if (warning != NULL)
            {
                *warning = warningMessage;
            }
         }
    }
    else
    {
        //
        // Empty name
        //

         QString warningMessage = QString("can not create a parameter with an empty name");

         qWarning() << "IngeScapeQuick warning:" << warningMessage;

         if (warning != NULL)
         {
             *warning = warningMessage;
         }
    }

    return result;
}



/**
 * @brief Update our list of inputs
 */
void IngeScapeQuick::_updateInputsList()
{
    QStringList newInputsList;

    // Get our inputs
    long numberOfInputs;
    char **inputs = igs_getInputsList(&numberOfInputs);
    if (inputs != NULL)
    {
        for (long index = 0; index < numberOfInputs; index++)
        {
            newInputsList.append( inputs[index] );
            free( inputs[index] );
        }
        free(inputs);
    }

    // Update our list of inputs
    setinputsList(newInputsList);
}



/**
 * @brief Update our list of outputs
 */
void IngeScapeQuick::_updateOutputsList()
{
    QStringList newOutputsList;

    // Get our outputs
    long numberOfOutputs;
    char **outputs = igs_getOutputsList(&numberOfOutputs);
    if (outputs != NULL)
    {
        for (long index = 0; index < numberOfOutputs; index++)
        {
            newOutputsList.append( outputs[index] );
            free( outputs[index] );
        }
        free(outputs);
    }

    // Update our list of outputs
    setoutputsList(newOutputsList);
}



/**
 * @brief Update our list of parameters
 */
void IngeScapeQuick::_updateParametersList()
{
    QStringList newParametersList;

    // Get our parameters
    long numberOfParameters;
    char **parameters = igs_getParametersList(&numberOfParameters);
    if (parameters != NULL)
    {
        for (long index = 0; index < numberOfParameters; index++)
        {
            newParametersList.append( parameters[index] );
            free( parameters[index] );
        }
        free(parameters);
    }

    // Update our list of parameters
    setparametersList(newParametersList);
}



//-------------------------------------------------------------------
//
// Protected Q_SLOTS
//
//-------------------------------------------------------------------


/**
 * @brief Called when our agent is asked to stop on the network
 */
void IngeScapeQuick::_onForcedStop()
{
    // Stop our IngeScape agent
    stop();

    // Quit our application
    QCoreApplication::exit(0);
}



/**
 * @brief Called when an output is updated from QML
 * @param key
 * @param value
 */
void IngeScapeQuick::_onOutputUpdatedFromQML(const QString &key, const QVariant &value)
{
    // Ensure that we have a valid key
    if (!key.isEmpty())
    {
        std::string stdName = key.toStdString();
        const char* cName = stdName.c_str();

        // Get type of this output
        iopType_t type = igs_getTypeForOutput(cName);
        switch(type)
        {
            case IGS_INTEGER_T:
            {
                bool ok = false;
                int cValue = value.toInt(&ok);

                if (ok)
                {
                    _observeOutputNeedToUpdateQMLMutex.lock();
                    _observeOutputNeedToUpdateQML = false;

                    igs_writeOutputAsInt(cName, cValue);

                    _observeOutputNeedToUpdateQML = true;
                    _observeOutputNeedToUpdateQMLMutex.unlock();
                }
                else
                {
                    qWarning() << "IngeScapeQuick warning: invalid value" << value
                               << "for IngeScape output" << key << "with type INTEGER";
                }
            }
            break;


            case IGS_DOUBLE_T:
            {
                bool ok = false;
                double cValue = value.toDouble(&ok);

                if (ok)
                {
                    _observeOutputNeedToUpdateQMLMutex.lock();
                    _observeOutputNeedToUpdateQML = false;

                    igs_writeOutputAsDouble(cName, cValue);

                    _observeOutputNeedToUpdateQML = true;
                    _observeOutputNeedToUpdateQMLMutex.unlock();
                }
                else
                {
                    qWarning() << "IngeScapeQuick warning: invalid value" << value
                               << "for IngeScape output" << key << "with type DOUBLE";
                }
            }
            break;


            case IGS_STRING_T:
            {
                _observeOutputNeedToUpdateQMLMutex.lock();
                _observeOutputNeedToUpdateQML = false;

                igs_writeOutputAsString(cName, (char *)value.toString().toStdString().c_str());

                _observeOutputNeedToUpdateQML = true;
                _observeOutputNeedToUpdateQMLMutex.unlock();
            }
            break;


            case IGS_BOOL_T:
            {
                _observeOutputNeedToUpdateQMLMutex.lock();
                _observeOutputNeedToUpdateQML = false;

                igs_writeOutputAsBool(cName, value.toBool());

                _observeOutputNeedToUpdateQML = true;
                _observeOutputNeedToUpdateQMLMutex.unlock();
            }
            break;


            case IGS_IMPULSION_T:
            {
                _observeOutputNeedToUpdateQMLMutex.lock();
                _observeOutputNeedToUpdateQML = false;

                igs_writeOutputAsImpulsion(cName);

                _observeOutputNeedToUpdateQML = true;
                _observeOutputNeedToUpdateQMLMutex.unlock();
            }
            break;

            case IGS_DATA_T:
            {
                qWarning() << "IngeScapeQuick warning: can not update output" << key <<  "with type DATA (not yet implemented)";
            }
            break;

            default:
            {
                qWarning() << "IngeScapeQuick warning: unhandled output type. Can not update output" << key;
            }
            break;
        }
    }
}



/**
 * @brief Called when a parameter is updated from QML
 * @param key
 * @param value
 */
void IngeScapeQuick::_onParameterUpdatedFromQML(const QString &key, const QVariant &value)
{
    // Ensure that we have a valid key
    if (!key.isEmpty())
    {
        std::string stdName = key.toStdString();
        const char* cName = stdName.c_str();

        // Get type of this parameter
        iopType_t type = igs_getTypeForParameter(cName);
        switch(type)
        {
            case IGS_INTEGER_T:
            {
                bool ok = false;
                int cValue = value.toInt(&ok);

                if (ok)
                {
                    _observeParameterNeedToUpdateQMLMutex.lock();
                    _observeParameterNeedToUpdateQML = false;

                    igs_writeParameterAsInt(cName, cValue);

                    _observeParameterNeedToUpdateQML = true;
                    _observeParameterNeedToUpdateQMLMutex.unlock();
                }
                else
                {
                    qWarning() << "IngeScapeQuick warning: invalid value" << value
                               << "for IngeScape parameter" << key << "with type INTEGER";
                }
            }
            break;


            case IGS_DOUBLE_T:
            {
                bool ok = false;
                double cValue = value.toDouble(&ok);

                if (ok)
                {
                    _observeParameterNeedToUpdateQMLMutex.lock();
                    _observeParameterNeedToUpdateQML = false;

                    igs_writeParameterAsDouble(cName, cValue);

                    _observeParameterNeedToUpdateQML = true;
                    _observeParameterNeedToUpdateQMLMutex.unlock();
                }
                else
                {
                    qWarning() << "IngeScapeQuick warning: invalid value" << value
                               << "for IngeScape parameter" << key << "with type DOUBLE";
                }
            }
            break;


            case IGS_STRING_T:
            {
                _observeParameterNeedToUpdateQMLMutex.lock();
                _observeParameterNeedToUpdateQML = false;

                igs_writeParameterAsString(cName, (char *)value.toString().toStdString().c_str());

                _observeParameterNeedToUpdateQML = true;
                _observeParameterNeedToUpdateQMLMutex.unlock();
            }
            break;


            case IGS_BOOL_T:
            {
                _observeParameterNeedToUpdateQMLMutex.lock();
                _observeParameterNeedToUpdateQML = false;

                igs_writeParameterAsBool(cName, value.toBool());

                _observeParameterNeedToUpdateQML = true;
                _observeParameterNeedToUpdateQMLMutex.unlock();
            }
            break;


            case IGS_IMPULSION_T:
            {
                qWarning() << "IngeScapeQuick warning: invalid parameter type IMPULSION. Can not update parameter" << key;
            }
            break;


            default:
            {
                qWarning() << "IngeScapeQuick warning: unhandled parameter type. Can not update parameter" << key;
            }
            break;
        }
    }
}
