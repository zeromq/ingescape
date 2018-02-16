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

#include "MasticQuick.h"

#include <QDebug>
#include <QCoreApplication>
#include <QMutex>


#include "masticquickinputspropertymap.h"
#include "masticquickoutputspropertymap.h"
#include "masticquickparameterspropertymap.h"

#include "masticquickbindingsingleton.h"
#include "masticquickinputbinding.h"
#include "masticquickoutputbinding.h"



extern "C" {
#include <mastic.h>
}





//-------------------------------------------------------------------
//
//
//  Static functions
//
//
//-------------------------------------------------------------------



/**
 * @brief Convert a MasticIopType::Value into an iopType_t value
 * @param value
 * @return
 */
static iopType_t enumMasticIopTypeToEnumIopType_t(MasticIopType::Value value)
{
    iopType_t result = INTEGER_T;

    switch(value)
    {
        case MasticIopType::INTEGER:
            result = INTEGER_T;
            break;

        case MasticIopType::DOUBLE:
            result = DOUBLE_T;
            break;

        case MasticIopType::STRING:
            result = STRING_T;
            break;

        case MasticIopType::BOOLEAN:
            result = BOOL_T;
            break;

        case MasticIopType::IMPULSION:
            result = IMPULSION_T;
            break;

        case MasticIopType::DATA:
            result = DATA_T;
            break;

        default:
            break;
    }

    return result;
}



/**
 * @brief Convert an iopType_t value into a MasticIopType::Value
 * @param value
 * @return
 */
static MasticIopType::Value enumIopType_tToMasticIopType(iopType_t value)
{
    MasticIopType::Value result = MasticIopType::INVALID;

    switch (value)
    {
        case INTEGER_T:
            result = MasticIopType::INTEGER;
            break;

        case DOUBLE_T:
            result = MasticIopType::DOUBLE;
            break;

        case STRING_T:
            result = MasticIopType::STRING;
            break;

        case BOOL_T:
            result = MasticIopType::BOOLEAN;
            break;

        case IMPULSION_T:
            result = MasticIopType::IMPULSION;
            break;

        case DATA_T:
            result = MasticIopType::DATA;
            break;

        default:
            break;
    }

    return result;
}



/**
 * @brief Convert a MasticLogLevel::Value into a mtic_logLevel_t value
 * @param value
 * @return
 */
static mtic_logLevel_t enumMasticLogLevelToEnumMticLogLevel_t(MasticLogLevel::Value value)
{
    mtic_logLevel_t result = MTIC_LOG_TRACE;

    switch (value)
    {
        case MasticLogLevel::LOG_TRACE:
            result = MTIC_LOG_TRACE;
            break;

        case MasticLogLevel::LOG_DEBUG:
            result = MTIC_LOG_DEBUG;
            break;

        case MasticLogLevel::LOG_INFO:
            result = MTIC_LOG_INFO;
            break;

        case MasticLogLevel::LOG_WARN:
            result = MTIC_LOG_WARN;
            break;

        case MasticLogLevel::LOG_ERROR:
            result = MTIC_LOG_ERROR;
            break;

        case MasticLogLevel::LOG_FATAL:
            result = MTIC_LOG_FATAL;
            break;

        default:
            break;
    }

    return result;
}



/**
 * @brief Convert a mtic_logLevel_t value into a MasticLogLevel::Value
 * @param value
 * @return
 */
static MasticLogLevel::Value enumMticLogLevel_tToMasticLogLevel(mtic_logLevel_t value)
{
    MasticLogLevel::Value result = MasticLogLevel::LOG_TRACE;

    switch(value)
    {
        case MTIC_LOG_TRACE:
            result = MasticLogLevel::LOG_TRACE;
            break;

        case MTIC_LOG_DEBUG:
            result = MasticLogLevel::LOG_DEBUG;
            break;

        case MTIC_LOG_INFO:
            result = MasticLogLevel::LOG_INFO;
            break;

        case MTIC_LOG_WARN:
            result = MasticLogLevel::LOG_WARN;
            break;

        case MTIC_LOG_ERROR:
            result = MasticLogLevel::LOG_ERROR;
            break;

        case MTIC_LOG_FATAL:
            result = MasticLogLevel::LOG_FATAL;
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
void MasticQuick_callbackObserveInput(iop_t iopType, const char *name, iopType_t valueType, void* value, long valueSize, void *customData)
{
    Q_UNUSED(valueSize)

    // Ensure that our callback is called by the required IOP category
    if (iopType == INPUT_T)
    {
        // Try to cast our custom data
        MasticQuick* controller = (MasticQuick *)customData;
        if ((controller != NULL) && (controller->inputs() != NULL))
        {
            QString qmlName(name);
            if (!qmlName.isEmpty())
            {
                switch(valueType)
                {
                    case INTEGER_T:
                        {
                            int newValue = *((int *)value);
                            QVariant qmlValue = QVariant(newValue);

                            controller->inputs()->insert(qmlName, qmlValue);
                            Q_EMIT controller->observeInput(qmlName, qmlValue);
                        }
                        break;

                    case DOUBLE_T:
                        {
                            double newValue = *((double *)value);
                            QVariant qmlValue = QVariant(newValue);

                            controller->inputs()->insert(qmlName, QVariant(newValue));
                            Q_EMIT controller->observeInput(qmlName, qmlValue);
                        }
                        break;

                    case STRING_T:
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

                    case BOOL_T:
                        {
                            bool newValue = *((bool *)value);
                            QVariant qmlValue = QVariant(newValue);

                            controller->inputs()->insert(qmlName, qmlValue);
                            Q_EMIT controller->observeInput(qmlName, qmlValue);
                        }
                        break;

                    case IMPULSION_T:
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

                    case DATA_T:
                        {
                            qWarning() << "MasticQuick warning: can not update input" << qmlName <<  "with type DATA (not yet implemented)";
                        }
                        break;

                    default:
                        break;
                }
            }
            else
            {
                qWarning() << "MasticQuick warning: can not update an input with an empty name";
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
void MasticQuick_callbackObserveOutput(iop_t iopType, const char *name, iopType_t valueType, void* value, long valueSize, void *customData)
{
    Q_UNUSED(valueSize)

    // Ensure that our callback is called by the required IOP category
    if (iopType == OUTPUT_T)
    {
        // Try to cast our custom data
        MasticQuick* controller = (MasticQuick *)customData;
        if ((controller != NULL) && (controller->outputs() != NULL))
        {
            QString qmlName(name);
            if (!qmlName.isEmpty())
            {
                switch(valueType)
                {
                    case INTEGER_T:
                        {
                            int newValue = *((int *)value);
                            QVariant qmlValue = QVariant(newValue);

                            controller->outputs()->insert(qmlName, qmlValue);
                            Q_EMIT controller->observeOutput(qmlName, qmlValue);
                        }
                        break;

                    case DOUBLE_T:
                        {
                            double newValue = *((double *)value);
                            QVariant qmlValue = QVariant(newValue);

                            controller->outputs()->insert(qmlName, QVariant(newValue));
                            Q_EMIT controller->observeOutput(qmlName, qmlValue);
                        }
                        break;

                    case STRING_T:
                        {
                            char* newCValue = (char *)value;
                            if (newCValue != NULL)
                            {
                                QString newValue(newCValue);
                                QVariant qmlValue = QVariant(newValue);

                                controller->outputs()->insert(qmlName, qmlValue);
                                Q_EMIT controller->observeOutput(qmlName, qmlValue);
                                // NB: we don't need to free newValue because we don't own it
                            }
                            else
                            {
                                controller->outputs()->insert(qmlName, QVariant(""));
                                Q_EMIT controller->observeOutput(qmlName, QVariant(""));
                            }
                        }
                        break;

                    case BOOL_T:
                        {
                            bool newValue = *((bool *)value);
                            QVariant qmlValue = QVariant(newValue);

                            controller->outputs()->insert(qmlName, qmlValue);
                            Q_EMIT controller->observeOutput(qmlName, qmlValue);
                        }
                        break;

                    case IMPULSION_T:
                        {
                            // Hack to force the update of our property
                            // We disable signals then we clear its value to detect a valud change when we set an empty value
                            controller->outputs()->blockSignals(true);
                            controller->outputs()->clear(qmlName);
                            controller->outputs()->blockSignals(false);

                            // Set an empty value to trigger an update
                            controller->outputs()->insert(qmlName, QVariant(""));
                            Q_EMIT controller->observeOutput(qmlName, QVariant(""));
                        }
                        break;

                    case DATA_T:
                        {
                            qWarning() << "MasticQuick warning: can not update output" << qmlName << "with type DATA (not yet implemented)";
                        }
                        break;

                    default:
                        break;
                }
            }
            else
            {
                qWarning() << "MasticQuick warning: can not update an output with an empty name";
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
void MasticQuick_callbackObserveParameter(iop_t iopType, const char *name, iopType_t valueType, void* value, long valueSize, void *customData)
{
    Q_UNUSED(valueSize)

    // Ensure that our callback is called by the required IOP category
    if (iopType == PARAMETER_T)
    {
        // Try to cast our custom data
        MasticQuick* controller = (MasticQuick *)customData;
        if ((controller != NULL) && (controller->parameters() != NULL))
        {
            QString qmlName(name);
            if (!qmlName.isEmpty())
            {
                switch(valueType)
                {
                    case INTEGER_T:
                        {
                            int newValue = *((int *)value);
                            QVariant qmlValue = QVariant(newValue);

                            controller->parameters()->insert(qmlName, qmlValue);
                            Q_EMIT controller->observeParameter(qmlName, qmlValue);
                        }
                        break;

                    case DOUBLE_T:
                        {
                            double newValue = *((double *)value);
                            QVariant qmlValue = QVariant(newValue);

                            controller->parameters()->insert(qmlName, QVariant(newValue));
                            Q_EMIT controller->observeParameter(qmlName, qmlValue);
                        }
                        break;

                    case STRING_T:
                        {
                            char* newCValue = (char *)value;
                            if (newCValue != NULL)
                            {
                                QString newValue(newCValue);
                                QVariant qmlValue = QVariant(newValue);

                                controller->parameters()->insert(qmlName, qmlValue);
                                Q_EMIT controller->observeParameter(qmlName, qmlValue);
                                // NB: we don't need to free newValue because we don't own it
                            }
                            else
                            {
                                controller->parameters()->insert(qmlName, QVariant(""));
                                Q_EMIT controller->observeParameter(qmlName, QVariant(""));
                            }
                        }
                        break;

                    case BOOL_T:
                        {
                            bool newValue = *((bool *)value);
                            QVariant qmlValue = QVariant(newValue);

                            controller->parameters()->insert(qmlName, qmlValue);
                            Q_EMIT controller->observeParameter(qmlName, qmlValue);
                        }
                        break;

                    case IMPULSION_T:
                        {
                            // Should not happen because a paramater can not be an impulsion
                            qWarning() << "MasticQuick warning: can not update parameter" << qmlName << "with type IMPULSION";
                        }
                        break;

                    case DATA_T:
                        {
                            qWarning() << "MasticQuick warning: can not update parameter" << qmlName << "with type DATA (not yet implemented)";
                        }
                        break;

                    default:
                        break;
                }
            }
            else
            {
                qWarning() << "MasticQuick warning: can not update a parameter with an empty name";
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
void MasticQuick_callbackObserveFreeze(bool isFrozen, void* customData)
{
    // Try to cast our custom data
    MasticQuick* controller = (MasticQuick *)customData;
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
void MasticQuick_callbackObserveMute(bool isMuted, void* customData)
{
    // Try to cast our custom data
    MasticQuick* controller = (MasticQuick *)customData;
    if (controller != NULL)
    {
        controller->_internal_setIsMuted(isMuted);
    }
}



/**
 * @brief Callback used to observe force stop
 * @param customData
 */
void MasticQuick_callbackForcedStop(void *customData)
{
    // Try to cast our custom data
    MasticQuick* controller = (MasticQuick *)customData;
    if (controller != NULL)
    {
        Q_EMIT controller->forcedStop();
    }
}




//-------------------------------------------------------------------
//
//
//  MasticQuick
//
//
//-------------------------------------------------------------------


//
// Define our singleton instance
//
// NB: we can not use Q_GLOBAL_STATIC because Mastic MUST be stopped in our mainloop.
//     Otherwise, Mastic can not terminate ZMQ safely and the application will crash
//
static MasticQuick* _MASTICQUICK_SINGLETON = NULL;


//
// Flag indicating if we have already instantiate a singleton
//
static bool _MASTICQUICK_SINGLETON_INITIALIZED = false;


//
// Define a mutex to protect our singleton instance
//
static QMutex _MASTICQUICK_SINGLETON_MUTEX;



//
// MasticQuick version
//
#define MASTICQUICK_VERSION_MAJOR 1
#define MASTICQUICK_VERSION_MINOR 0




/**
 * @brief Default constructor
 * @param parent
 */
MasticQuick::MasticQuick(QObject *parent) : QObject(parent),
    _canBeFrozen(false),
    _isStarted(false),
    _inputs(NULL),
    _outputs(NULL),
    _parameters(NULL)
{    
    // Set our default agent name
    setagentName(QCoreApplication::applicationName());

    // Set our default definition
    setdefinitionName(QCoreApplication::applicationName());
    setdefinitionVersion("0.0");
    setdefinitionDescription(tr("Definition of %1").arg(QCoreApplication::applicationName()));


    //
    // Get initial values of Mastic internal properties
    //

    // - version of Mastic
    _version = mtic_version();

    // - agent state
    char* cAgentState = mtic_getAgentState();
    if (cAgentState != NULL)
    {
        _agentState = QString(cAgentState);
        free(cAgentState);
        cAgentState = NULL;
    }

    // - isFrozen flag
    _isFrozen = mtic_isFrozen();

    // - isMuted flag
    _isMuted = mtic_isMuted();

    // - isVerbose
    _isVerbose = mtic_isVerbose();

    // - log level
    _logLevel = enumMticLogLevel_tToMasticLogLevel( mtic_getLogLevel() );


    //
    // Add mastic observers
    //

    // Observe mute/unmute
    mtic_observeMute(&MasticQuick_callbackObserveMute, this);

    // Observe freeze/unfreeze
    mtic_observeFreeze(&MasticQuick_callbackObserveFreeze, this);


    // Observe forced stop
    connect(this, &MasticQuick::forcedStop, this, &MasticQuick::_onForcedStop);
    mtic_observeForcedStop(&MasticQuick_callbackForcedStop, this);



    //
    // Init dynamic properties
    //

    // - inputs
    _inputs = new MasticQuickInputsPropertyMap(this);

    // - outputs
    _outputs = new MasticQuickOutputsPropertyMap(this);
    if (_outputs != NULL)
    {
        connect(_outputs, &QQmlPropertyMap::valueChanged, this, &MasticQuick::_onOutputUpdatedFromFromQML);
    }

    // - parameters
    _parameters = new MasticQuickParametersPropertyMap(this);
    if (_parameters != NULL)
    {
        connect(_parameters, &QQmlPropertyMap::valueChanged, this, &MasticQuick::_onParameterUpdatedFromFromQML);
    }
}


/**
 * @brief Destructor
 */
MasticQuick::~MasticQuick()
{
    // Unsubscribe to Mastic signals
    disconnect(this, &MasticQuick::forcedStop, this, &MasticQuick::_onForcedStop);

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
        disconnect(_outputs, &QQmlPropertyMap::valueChanged, this, &MasticQuick::_onOutputUpdatedFromFromQML);

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
        disconnect(_parameters, &QQmlPropertyMap::valueChanged, this, &MasticQuick::_onParameterUpdatedFromFromQML);

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
    _MASTICQUICK_SINGLETON_MUTEX.lock();
    if (_MASTICQUICK_SINGLETON == this)
    {
        _MASTICQUICK_SINGLETON = NULL;
    }
    _MASTICQUICK_SINGLETON_MUTEX.unlock();
}



/**
 * @brief Get our singleton instance
 * @return
 *
 * @remark Our singleton is owned by the QML engine. Thus, it is unsafe to keep a reference that points to it
 */
MasticQuick* MasticQuick::instance()
{
    // Thread-safe init
    _MASTICQUICK_SINGLETON_MUTEX.lock();

    if (_MASTICQUICK_SINGLETON == NULL)
    {
        // Check if it is our first singleton
        if (_MASTICQUICK_SINGLETON_INITIALIZED)
        {
            qWarning() << "MasticQuick warning: a singleton has already been created and destroyed."
                       << "Please do not try to access MasticQuick after its destruction by QML";
        }
        else
        {
            // First init, we can create a singleton
            _MASTICQUICK_SINGLETON = new MasticQuick();
            _MASTICQUICK_SINGLETON_INITIALIZED = true;
        }
    }

    _MASTICQUICK_SINGLETON_MUTEX.unlock();

    return _MASTICQUICK_SINGLETON;
}



/**
 * @brief Method used to provide a singleton to QML
 * @param engine
 * @param scriptEngine
 * @return
 */
QObject* MasticQuick::qmlSingleton(QQmlEngine* engine, QJSEngine* scriptEngine)
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
void MasticQuick::registerTypes(const char* uri)
{
    // Check our URI
    Q_ASSERT(uri == QLatin1String(DEFAULT_MASTICQUICK_URI));


    //
    // Register enums
    //
    // - IOP types
    MasticIopType::qmlRegister(uri, MASTICQUICK_VERSION_MAJOR, MASTICQUICK_VERSION_MINOR);
    // - Log levels
    MasticLogLevel::qmlRegister(uri, MASTICQUICK_VERSION_MAJOR, MASTICQUICK_VERSION_MINOR);


    //
    // Register uncreatable types
    //
    qmlRegisterUncreatableType<MasticQuickInputsPropertyMap>(uri, MASTICQUICK_VERSION_MAJOR, MASTICQUICK_VERSION_MINOR, "MasticQuickInputsPropertyMap", "Internal class");
    qmlRegisterUncreatableType<MasticQuickOutputsPropertyMap>(uri, MASTICQUICK_VERSION_MAJOR, MASTICQUICK_VERSION_MINOR, "MasticQuickOutputsPropertyMap", "Internal class");
    qmlRegisterUncreatableType<MasticQuickParametersPropertyMap>(uri, MASTICQUICK_VERSION_MAJOR, MASTICQUICK_VERSION_MINOR, "MasticQuickParametersPropertyMap", "Internal class");


    //
    // Register creatable types
    //
    qmlRegisterType<MasticQuickInputBinding>(uri, MASTICQUICK_VERSION_MAJOR, MASTICQUICK_VERSION_MINOR, "MasticInputBinding");
    qmlRegisterType<MasticQuickOutputBinding>(uri, MASTICQUICK_VERSION_MAJOR, MASTICQUICK_VERSION_MINOR, "MasticOutputBinding");


    //
    // Register singletons
    //
    qmlRegisterSingletonType<MasticQuickBindingSingleton>(uri, MASTICQUICK_VERSION_MAJOR, MASTICQUICK_VERSION_MINOR, "MasticBinding", &MasticQuickBindingSingleton::qmlSingleton);
    qmlRegisterSingletonType<MasticQuick>(uri, MASTICQUICK_VERSION_MAJOR, MASTICQUICK_VERSION_MINOR, "Mastic", &MasticQuick::qmlSingleton);
}





//-------------------------------------------------------------------
//
// Custom getters
//
//-------------------------------------------------------------------


/**
 * @brief Get our inputs property
 * @return
 */
QQmlPropertyMap* MasticQuick::inputs() const
{
    return _inputs;
}


/**
 * @brief Get our outputs property
 * @return
 */
QQmlPropertyMap* MasticQuick::outputs() const
{
    return _outputs;
}


/**
 * @brief Get our parameters property
 * @return
 */
QQmlPropertyMap* MasticQuick::parameters() const
{
    return _parameters;
}





//-------------------------------------------------------------------
//
// Custom setters
//
//-------------------------------------------------------------------



/**
 * @brief Set our agent name
 * @param value
 */
void MasticQuick::setagentName(QString value)
{
    if (_agentName != value)
    {
        // Save value
        _agentName = value;

        // Set our agent name
        mtic_setAgentName(value.toStdString().c_str());

        // Notify change
        Q_EMIT agentNameChanged(value);
    }
}



/**
 * @brief Set our agent state
 * @param value
 */
void MasticQuick::setagentState(QString value)
{
    if (_agentState != value)
    {
        // Save our new state
        _agentState = value;

        // Set our new state
        mtic_setAgentState(_agentState.toStdString().c_str());

        // Notify change
        Q_EMIT agentStateChanged(value);
    }
}



/**
 * @brief Set the name of our definition
 * @param value
 */
void MasticQuick::setdefinitionName(QString value)
{
    if (_definitionName != value)
    {
        // Save value
        _definitionName = value;

        // Set our definition name
        mtic_setDefinitionName(value.toStdString().c_str());

        // Notify change
        Q_EMIT definitionNameChanged(value);
    }
}



/**
 * @brief Set the version of our definition
 * @param value
 */
void MasticQuick::setdefinitionVersion(QString value)
{
    if (_definitionVersion != value)
    {
        // Save value
        _definitionVersion = value;

        // Set our definition version
        mtic_setDefinitionVersion(value.toStdString().c_str());

        // Notify change
        Q_EMIT definitionVersionChanged(value);
    }
}



/**
 * @brief Set the description of our definition
 * @param value
 */
void MasticQuick::setdefinitionDescription(QString value)
{
    if (_definitionDescription != value)
    {
        // Save value
        _definitionDescription = value;

        // Set our definition description
        mtic_setDefinitionDescription(value.toStdString().c_str());

        // Notify change
        Q_EMIT definitionDescriptionChanged(value);
    }
}



/**
 * @brief Set our isMuted flag
 * @param value
 */
void MasticQuick::setisMuted(bool value)
{
    if (_isMuted != value)
    {
        // Set our isMuted flag
        if (value)
        {
            mtic_mute();
        }
        else
        {
            mtic_unmute();
        }

        // Get our new value
        _isMuted = mtic_isMuted();

        // Notify change
        Q_EMIT isMutedChanged(_isMuted);
    }
}



/**
 * @brief Set our isFrozen flag
 * @param value
 */
void MasticQuick::setisFrozen(bool value)
{
    if (_isFrozen != value)
    {
        // Set our isFrozen flag
        if (value)
        {
            mtic_freeze();
        }
        else
        {
            mtic_unfreeze();
        }

        // Get our new value
        _isFrozen = mtic_isFrozen();

        // Notify change
        Q_EMIT isFrozenChanged(_isFrozen);
    }
}



/**
 * @brief Set if our agent can be frozen or not
 * @param value
 */
void MasticQuick::setcanBeFrozen(bool value)
{
    if (_canBeFrozen != value)
    {
        // Save our new value
        _canBeFrozen = value;

        // Set our canBeFrozen property
        mtic_setCanBeFrozen(value);

        // Notify change
        Q_EMIT canBeFrozenChanged(value);
    }
}



/**
 * @brief Set our isVerbose flag
 * @param value
 */
void MasticQuick::setisVerbose(bool value)
{
    if (_isVerbose != value)
    {
        // Save value
        _isVerbose = value;

        // Set our verbose flag
        mtic_setVerbose(value);

        // Notify change
        Q_EMIT isVerboseChanged(value);
    }
}



/**
 * @brief Set our log lebvel
 * @param value
 */
void MasticQuick::setlogLevel(MasticLogLevel::Value value)
{
    if (_logLevel != value)
    {
        // Save value
        _logLevel = value;

        // Set out log level
        mtic_setLogLevel( enumMasticLogLevelToEnumMticLogLevel_t(value) );

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
bool MasticQuick::startWithDevice(QString networkDevice, int port)
{
    bool result = false;

    // Check if we must stop our agent
    if (_isStarted)
    {
        stop();
    }

    // Try to start Mastic
    if (mtic_startWithDevice(networkDevice.toStdString().c_str(), port) == 1)
    {
        setisStarted(true);
        result = true;
    }
    else
    {
        qWarning() << "MasticQuick warning: fail to start our agent via startWithDevice("
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
bool MasticQuick::startWithIP(QString ipAddress, int port)
{
    bool result = false;

    // Check if we must stop our agent
    if (_isStarted)
    {
        stop();
    }

    // Try to start Mastic
    if (mtic_startWithIP(ipAddress.toStdString().c_str(), port) == 1)
    {
        setisStarted(true);
        result = true;
    }
    else
    {
        qWarning() << "MasticQuick warning: fail to start our agent via startWithIP("
                   << ipAddress << "," << port << ")";
    }

    return result;
}



/**
 * @brief Stop our agent
 */
bool MasticQuick::stop()
{
    bool result = false;

    // Check if our agent is started
    if (_isStarted)
    {
        if (mtic_stop() == 1)
        {
            setisStarted(false);
            result = true;
        }
        else
        {
            qWarning() << "MasticQuick warning: fail to stop our agent";
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
bool MasticQuick::writeOutputAsInt(QString name, int value)
{
    bool result = false;

    // Ensure that we have a valid name
    if (!name.isEmpty())
    {
        // Try to write value
        if (mtic_writeOutputAsInt(name.toStdString().c_str(), value) == 1)
        {
            result = true;
        }
        else
        {
            qWarning() << "MasticQuick warning: fail to write Mastic output"
                       << name << "via writeOutputAsInt";
        }
    }
    else
    {
        qWarning() << "MasticQuick warning: writeOutputAsInt() can not write an output without a name";
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
bool MasticQuick::writeOutputAsDouble(QString name, double value)
{
    bool result = false;

    // Ensure that we have a valid name
    if (!name.isEmpty())
    {
        // Try to write value
        if (mtic_writeOutputAsDouble(name.toStdString().c_str(), value) == 1)
        {
            result = true;
        }
        else
        {
            qWarning() << "MasticQuick warning: fail to write output"
                       << name << "via writeOutputAsDouble";
        }
    }
    else
    {
        qWarning() << "MasticQuick warning: writeOutputAsDouble() can not write an output without a name";
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
bool MasticQuick::writeOutputAsString(QString name, QString value)
{
    bool result = false;

    // Ensure that we have a valid name
    if (!name.isEmpty())
    {
        // Try to write value
        if (mtic_writeOutputAsString(name.toStdString().c_str(), (char *)value.toStdString().c_str()) == 1)
        {
            result = true;
        }
        else
        {
            qWarning() << "MasticQuick warning: fail to write output"
                       << name << "via writeOutputAsString";
        }
    }
    else
    {
        qWarning() << "MasticQuick warning: writeOutputAsString() can not write an output without a name";
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
bool MasticQuick::writeOutputAsBool(QString name, bool value)
{
    bool result = false;

    // Ensure that we have a valid name
    if (!name.isEmpty())
    {
        // Try to write value
        if (mtic_writeOutputAsBool(name.toStdString().c_str(), value) == 1)
        {
            result = true;
        }
        else
        {
            qWarning() << "MasticQuick warning: fail to write output"
                       << name << "via writeOutputAsBool";
        }
    }
    else
    {
        qWarning() << "MasticQuick warning: writeOutputAsBool() can not write an output without a name";
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
bool MasticQuick::writeOutputAsImpulsion(QString name)
{
    bool result = false;

    // Ensure that we have a valid name
    if (!name.isEmpty())
    {
        // Try to write value
        if (mtic_writeOutputAsImpulsion(name.toStdString().c_str()) == 1)
        {
            result = true;
        }
        else
        {
            qWarning() << "MasticQuick warning: fail to write output"
                       << name << "via writeOutputAsImpulsion";
        }
    }
    else
    {
        qWarning() << "MasticQuick warning: writeOutputAsImpulsion() can not write an output without a name";
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
bool MasticQuick::writeOutputAsData(QString name, void* value)
{
    Q_UNUSED(name)
    Q_UNUSED(value)

    bool result = false;

    qWarning() << "MasticQuick warning: NOT YET IMPLEMENTED. Can not write output"
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
bool MasticQuick::writeParameterAsInt(QString name, int value)
{
    bool result = false;

    // Ensure that we have a valid name
    if (!name.isEmpty())
    {
        // Try to write value
        if (mtic_writeParameterAsInt(name.toStdString().c_str(), value) == 1)
        {
            result = true;
        }
        else
        {
            qWarning() << "MasticQuick warning: fail to write parameter"
                       << name << "via writeParameterAsInt";
        }
    }
    else
    {
        qWarning() << "MasticQuick warning: writeParameterAsInt() can not write a parameter without a name";
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
bool MasticQuick::writeParameterAsDouble(QString name, double value)
{
    bool result = false;

    // Ensure that we have a valid name
    if (!name.isEmpty())
    {
        // Try to write value
        if (mtic_writeParameterAsDouble(name.toStdString().c_str(), value) == 1)
        {
            result = true;
        }
        else
        {
            qWarning() << "MasticQuick warning: fail to write parameter"
                       << name << "via writeParameterAsDouble";
        }
    }
    else
    {
        qWarning() << "MasticQuick warning: writeParameterAsDouble() can not write a parameter without a name";
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
bool MasticQuick::writeParameterAsString(QString name, QString value)
{
    bool result = false;

    // Ensure that we have a valid name
    if (!name.isEmpty())
    {
        // Try to write value
        if (mtic_writeParameterAsString(name.toStdString().c_str(), (char *)value.toStdString().c_str()) == 1)
        {
            result = true;
        }
        else
        {
            qWarning() << "MasticQuick warning: fail to write parameter"
                       << name << "via writeParameterAsString";
        }
    }
    else
    {
        qWarning() << "MasticQuick warning: writeParameterAsString() can not write a parametrer without a name";
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
bool MasticQuick::writeParameterAsBool(QString name, bool value)
{
    bool result = false;

    // Ensure that we have a valid name
    if (!name.isEmpty())
    {
        // Try to write value
        if (mtic_writeParameterAsBool(name.toStdString().c_str(), value) == 1)
        {
            result = true;
        }
        else
        {
            qWarning() << "MasticQuick warning: fail to write parameter"
                       << name << "via writeParameterAsBool";
        }
    }
    else
    {
        qWarning() << "MasticQuick warning: writeParameterAsBool() can not write a parameter without a name";
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
bool MasticQuick::writeParameterAsData(QString name, void* value)
{
    Q_UNUSED(name)
    Q_UNUSED(value)

    bool result = false;

    qWarning() << "MasticQuick warning: NOT YET IMPLEMENTED. Can not write parameter"
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
MasticIopType::Value MasticQuick::getTypeForInput(QString name)
{
    MasticIopType::Value result = MasticIopType::INVALID;

    if (!name.isEmpty())
    {
        std::string stdName = name.toStdString();
        const char* cName = stdName.c_str();
        if (mtic_checkInputExistence(cName))
        {
            result = enumIopType_tToMasticIopType( mtic_getTypeForInput(cName) );
        }
    }
    else
    {
        qWarning() << "MasticQuick warning: getTypeForInput() - name can not be empty";
    }

    return result;
}



/**
 * @brief Get type of a given output
 * @param name
 * @return
 */
MasticIopType::Value MasticQuick::getTypeForOutput(QString name)
{
    MasticIopType::Value result = MasticIopType::INVALID;

    if (!name.isEmpty())
    {
        std::string stdName = name.toStdString();
        const char* cName = stdName.c_str();
        if (mtic_checkOutputExistence(cName))
        {
            result = enumIopType_tToMasticIopType( mtic_getTypeForOutput(cName) );
        }
    }
    else
    {
        qWarning() << "MasticQuick warning: getTypeForOutput() - name can not be empty";
    }

    return result;
}



/**
 * @brief Get type of a given parameter
 * @param name
 * @return
 */
MasticIopType::Value MasticQuick::getTypeForParameter(QString name)
{
    MasticIopType::Value result = MasticIopType::INVALID;

    if (!name.isEmpty())
    {
        std::string stdName = name.toStdString();
        const char* cName = stdName.c_str();
        if (mtic_checkParameterExistence(cName))
        {
            result = enumIopType_tToMasticIopType( mtic_getTypeForParameter(cName) );
        }
    }
    else
    {
        qWarning() << "MasticQuick warning: getTypeForParameter() - name can not be empty";
    }

    return result;
}



/**
 * @brief Check if our agent has an input with this name
 * @param name
 * @return
 */
bool MasticQuick::checkInputExistence(QString name)
{
    return mtic_checkInputExistence(name.toStdString().c_str());
}



/**
 * @brief Check if our agent has an output with this name
 * @param name
 * @return
 */
bool MasticQuick::checkOutputExistence(QString name)
{
    return mtic_checkOutputExistence(name.toStdString().c_str());
}



/**
 * @brief Check if our agent has a parameter with this name
 * @param name
 * @return
 */
bool MasticQuick::checkParameterExistence(QString name)
{
    return mtic_checkParameterExistence(name.toStdString().c_str());
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
bool MasticQuick::muteOuput(QString name)
{
    bool result = false;

    if (!name.isEmpty())
    {
        if (mtic_muteOutput(name.toStdString().c_str()) == 1)
        {
            result = true;
        }
    }
    else
    {
        qWarning() << "MasticQuick warning: muteOuput() - name can not be empty";
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
bool MasticQuick::unmuteOuput(QString name)
{
    bool result = false;

    if (!name.isEmpty())
    {
        if (mtic_unmuteOutput(name.toStdString().c_str()) == 1)
        {
            result = true;
        }
    }
    else
    {
        qWarning() << "MasticQuick warning: unmuteOuput() - name can not be empty";
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
bool MasticQuick::isOutputMuted(QString name, QVariant qmlUpdateExtraParameter)
{
    Q_UNUSED(qmlUpdateExtraParameter)
    bool result = false;

    if (!name.isEmpty())
    {
        result = mtic_isOutputMuted(name.toStdString().c_str());
    }
    else
    {
        qWarning() << "MasticQuick warning: isOutputMuted() - name can not be empty";
    }

    return result;
}




//-------------------------------------------------------------------
//
// Load / set / get definition
//
//-------------------------------------------------------------------





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
bool MasticQuick::createInputInt(QString name, int value)
{
    return _createInput(name, MasticIopType::INTEGER, QVariant(value), &value, sizeof(int));
}



/**
 * @brief Create a new double input
 *
 * @param name
 * @param value
 *
 * @return true if an input is created, false otherwise (i.e. we already have an input with this name)
 */
bool MasticQuick::createInputDouble(QString name, double value)
{
    return _createInput(name, MasticIopType::DOUBLE, QVariant(value), &value, sizeof(double));
}



/**
 * @brief Create a new string input
 *
 * @param name
 * @param value
 *
 * @return true if an input is created, false otherwise (i.e. we already have an input with this name)
 */
bool MasticQuick::createInputString(QString name, QString value)
{
    std::string stdString = value.toStdString();
    const char* cValue = stdString.c_str();
    int cValueLength = ((cValue != NULL) ? strlen(cValue) : 0);

    return _createInput(name, MasticIopType::STRING, QVariant(value), (void *)cValue, (cValueLength + 1) * sizeof(char));
}



/**
 * @brief Create a new boolean input
 *
 * @param name
 * @param value
 *
 * @return true if an input is created, false otherwise (i.e. we already have an input with this name)
 */
bool MasticQuick::createInputBool(QString name, bool value)
{
    return _createInput(name, MasticIopType::BOOLEAN, QVariant(value), &value, sizeof(bool));
}



/**
 * @brief Create a new impulsion input
 *
 * @param name
 *
 * @return true if an input is created, false otherwise (i.e. we already have an input with this name)
 */
bool MasticQuick::createInputImpulsion(QString name)
{
    return _createInput(name, MasticIopType::IMPULSION, QVariant(""), 0, 0);
}



/**
 * @brief Create a new data input
 *
 * @param name
 * @param value
 *
 * @return true if an input is created, false otherwise (i.e. we already have an input with this name)
 */
bool MasticQuick::createInputData(QString name, void* value)
{
    Q_UNUSED(name)
    Q_UNUSED(value)

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
bool MasticQuick::createOutputInt(QString name, int value)
{
    return _createOutput(name, MasticIopType::INTEGER, QVariant(value), &value, sizeof(int));
}



/**
 * @brief Create a new double output
 *
 * @param name
 * @param value
 *
 * @return true if an output is created, false otherwise (i.e. we already have an output with this name)
 */
bool MasticQuick::createOutputDouble(QString name, double value)
{
    return _createOutput(name, MasticIopType::DOUBLE, QVariant(value), &value, sizeof(double));
}



/**
 * @brief Create a new string output
 *
 * @param name
 * @param value
 *
 * @return true if an output is created, false otherwise (i.e. we already have an output with this name)
 */
bool MasticQuick::createOutputString(QString name, QString value)
{
    std::string stdString = value.toStdString();
    const char* cValue = stdString.c_str();
    int cValueLength = ((cValue != NULL) ? strlen(cValue) : 0);

    return _createOutput(name, MasticIopType::STRING, QVariant(value), (void *)cValue, (cValueLength + 1) * sizeof(char));
}



/**
 * @brief Create a new boolean output
 *
 * @param name
 * @param value
 *
 * @return true if an output is created, false otherwise (i.e. we already have an output with this name)
 */
bool MasticQuick::createOutputBool(QString name, bool value)
{
    return _createOutput(name, MasticIopType::BOOLEAN, QVariant(value), &value, sizeof(bool));
}



/**
 * @brief Create a new impulsion output
 *
 * @param name
 *
 * @return true if an output is created, false otherwise (i.e. we already have an output with this name)
 */
bool MasticQuick::createOutputImpulsion(QString name)
{
    return _createOutput(name, MasticIopType::IMPULSION, QVariant(""), 0, 0);
}


/**
 * @brief Create a new data output
 *
 * @param name
 * @param value
 *
 * @return true if an output is created, false otherwise (i.e. we already have an output with this name)
 */
bool MasticQuick::createOutputData(QString name, void* value)
{
    Q_UNUSED(name)
    Q_UNUSED(value)

    bool result = false;

    qWarning() << "MasticQuick warning: createOutputData() NOT YET IMPLEMENTED. Can not create output" << name << "with type DATA";

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
bool MasticQuick::createParameterInt(QString name, int value)
{
    return _createParameter(name, MasticIopType::INTEGER, QVariant(value), &value, sizeof(int));
}



/**
 * @brief Create a new double parameter
 *
 * @param name
 * @param value
 *
 * @return true if a parameter is created, false otherwise (i.e. we already have a parameter with this name)
 */
bool MasticQuick::createParameterDouble(QString name, double value)
{
    return _createParameter(name, MasticIopType::DOUBLE, QVariant(value), &value, sizeof(double));
}



/**
 * @brief Create a new string parameter
 *
 * @param name
 * @param value
 *
 * @return true if a parameter is created, false otherwise (i.e. we already have a parameter with this name)
 */
bool MasticQuick::createParameterString(QString name, QString value)
{
    std::string stdString = value.toStdString();
    const char* cValue = stdString.c_str();
    int cValueLength = ((cValue != NULL) ? strlen(cValue) : 0);

    return _createParameter(name, MasticIopType::STRING, QVariant(value), (void *)cValue, (cValueLength + 1) * sizeof(char));
 }



/**
 * @brief Create a new boolean parameter
 *
 * @param name
 * @param value
 *
 * @return true if a parameter is created, false otherwise (i.e. we already have a parameter with this name)
 */
bool MasticQuick::createParameterBool(QString name, bool value)
{
    return _createParameter(name, MasticIopType::BOOLEAN, QVariant(value), &value, sizeof(bool));
}



/**
 * @brief Create a new data parameter
 *
 * @param name
 * @param value
 *
 * @return true if a parameter is created, false otherwise (i.e. we already have a parameter with this name)
 */
bool MasticQuick::createParameterData(QString name, void* value)
{
    Q_UNUSED(name)
    Q_UNUSED(value)

    bool result = false;

    qDebug() << "MasticQuick warning: createParameterData() NOT YET IMPLEMENTED. Can not create parameter" << name << "with type DATA";

    return result;
}



/**
 * @brief Remove an input
 *
 * @param name
 *
 * @return true if this input is removed, false otherwise
 */
bool MasticQuick::removeInput(QString name)
{
    bool result = false;

    // Ensure that we have a valid name
    if (!name.isEmpty())
    {
        // Try to remove input
        if (mtic_removeInput(name.toStdString().c_str()) == 1)
        {
            result = true;
        }
        else
        {
            qWarning() << "MasticQuick warning: failed to remove input"
                       << name << "via removeInput";
        }
    }
    else
    {
        qWarning() << "MasticQuick warning: removeInput() can not remove an input without a name";
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
bool MasticQuick::removeOutput(QString name)
{
    bool result = false;

    // Ensure that we have a valid name
    if (!name.isEmpty())
    {
        // Try to remove ouput
        if (mtic_removeOutput(name.toStdString().c_str()) == 1)
        {
            result = true;
        }
        else
        {
            qWarning() << "MasticQuick warning: failed to remove output"
                       << name << "via removeOutput";
        }
    }
    else
    {
        qWarning() << "MasticQuick warning: removeOutput() can not remove an output without a name";
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
bool MasticQuick::removeParameter(QString name)
{
    bool result = false;

    // Ensure that we have a valid name
    if (!name.isEmpty())
    {
        // Try to remove parameter
        if (mtic_removeParameter(name.toStdString().c_str()) == 1)
        {
            result = true;
        }
        else
        {
            qWarning() << "MasticQuick warning: failed to remove parameter"
                       << name << "via removeParameter";
        }
    }
    else
    {
        qWarning() << "MasticQuick warning: removeParameter() can not remove a parameter without a name";
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
QStringList MasticQuick::getNetdevicesList(QVariant qmlUpdateExtraParameter)
{
    Q_UNUSED(qmlUpdateExtraParameter)

    QStringList result;

    // Get all devices
    char **devices = NULL;
    int numberOfDevices = 0;

    mtic_getNetdevicesList(&devices, &numberOfDevices);
    for (int index = 0; index < numberOfDevices; index++)
    {
        result.append( QString(devices[index]) );
    }

    // Clean-up
    mtic_freeNetdevicesList(devices, numberOfDevices);

    return result;
}



/**
 * @brief Print (or save) debugging information
 * @param logLevel
 * @param text
 */
void MasticQuick::log(MasticLogLevel::Value logLevel, QString text)
{
    mtic_log( enumMasticLogLevelToEnumMticLogLevel_t(logLevel), text.toStdString().c_str());
}



/**
 * @brief Print (or save) debugging information (loglevel = trace)
 * @param text
 */
void MasticQuick::trace(QString text)
{
    log(MasticLogLevel::LOG_TRACE, text);
}



/**
 * @brief Print (or save) debugging information (loglevel = debug)
 * @param text
 */
void MasticQuick::debug(QString text)
{
    log(MasticLogLevel::LOG_DEBUG, text);
}



/**
 * @brief Print (or save) debugging information (loglevel = info)
 * @param text
 */
void MasticQuick::info(QString text)
{
    log(MasticLogLevel::LOG_INFO, text);
}



/**
 * @brief Print (or save) debugging information (loglevel = warn)
 * @param text
 */
void MasticQuick::warn(QString text)
{
    log(MasticLogLevel::LOG_WARN, text);
}



/**
 * @brief Print (or save) debugging information (loglevel = error)
 * @param text
 */
void MasticQuick::error(QString text)
{
    log(MasticLogLevel::LOG_ERROR, text);
}



/**
 * @brief Print (or save) debugging information (loglevel = fatal)
 * @param text
 */
void MasticQuick::fatal(QString text)
{
    log(MasticLogLevel::LOG_FATAL, text);
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
bool MasticQuick::checkIfIopNameIsValid(const QString& name)
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
// Internal API - MUST not be used outside of MasticQuick
//
//--------------------------------------------------------


/**
 * @brief Set our isMuted flag based on observeMute
 * @param value
 */
void MasticQuick::_internal_setIsMuted(bool value)
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
void MasticQuick::_internal_setIsFrozen(bool value)
{
    if (_isFrozen != value)
    {
        // Save value
        _isFrozen = value;

        // Notify change
        Q_EMIT isFrozenChanged(value);
    }
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
bool MasticQuick::_createInput(QString name, MasticIopType::Value type, QVariant qmlValue, void* cValue, long cSize)
{
    bool result = false;

    // Check if we have a name
    if (!name.isEmpty())
    {
        // Check if it is a valid IOP name
        if (checkIfIopNameIsValid(name))
        {
            // Check if we must create a Mastic input
            std::string stdName = name.toStdString();
            const char* cName = stdName.c_str();
            if (!mtic_checkInputExistence(cName))
            {
                if (mtic_createInput(cName, enumMasticIopTypeToEnumIopType_t(type), cValue, cSize) == 1)
                {
                    // Observe this new input
                    if (mtic_observeInput(cName, &MasticQuick_callbackObserveInput, this) != 1)
                    {
                        qWarning() << Q_FUNC_INFO << "warning: failed to observe input" << name;
                    }


                    // Add it to the list of QML dynamic properties
                    if (_inputs != NULL)
                    {
                        // Update QML
                        // NB: special case for impulsion properties because we don't want to trigger them at startup
                        if (type == MasticIopType::IMPULSION)
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
                    qWarning() << Q_FUNC_INFO << "warning: failed to create input" << name;
                }
            }
            else
            {
                qWarning() << Q_FUNC_INFO << "warning: input" << name << "already exists";
            }
        }
        else
        {
            qWarning() << Q_FUNC_INFO << "warning: '" << name << "' is an invalid input name, it conflicts with Qt internal symbols";
        }
    }
    else
    {
        qWarning() << Q_FUNC_INFO << "warning: can not create an input with an empty name";
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
 *
 * @return true if an output is created, false otherwise
 */
bool MasticQuick::_createOutput(QString name, MasticIopType::Value type, QVariant qmlValue, void* cValue, long cSize)
{
    bool result = false;

    // Check if we have a name
    if (!name.isEmpty())
    {
        // Check if it is a valid IOP name
        if (checkIfIopNameIsValid(name))
        {
            // Check if we must create a Mastic input
            std::string stdName = name.toStdString();
            const char* cName = stdName.c_str();
            if (!mtic_checkOutputExistence(cName))
            {
                if (mtic_createOutput(cName, enumMasticIopTypeToEnumIopType_t(type), cValue, cSize) == 1)
                {
                    // Observe this new output
                    if (mtic_observeOutput(cName, &MasticQuick_callbackObserveOutput, this) != 1)
                    {
                        qWarning() << Q_FUNC_INFO << "warning: failed to observe output" << name;
                    }


                    // Add it to the list of QML dynamic properties
                    if (_outputs != NULL)
                    {
                        // Update QML
                        // NB: special case for impulsion properties because we don't want to trigger them at startup
                        if (type == MasticIopType::IMPULSION)
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
                    Q_EMIT outputsListChanged(_outputsList);


                    // Everything is ok
                    result = true;
                }
                else
                {
                    qWarning() << Q_FUNC_INFO << "warning: failed to create output" << name;
                }
            }
            else
            {
                qWarning() << Q_FUNC_INFO << "warning: output" << name << "already exists";
            }
        }
        else
        {
            qWarning() << Q_FUNC_INFO << "warning: '" << name << "' is an invalid output name, it conflicts with Qt internal symbols";
        }
    }
    else
    {
        qWarning() << Q_FUNC_INFO << "warning: can not create an output with an empty name";
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
 *
 * @return true if a parameter is created, false otherwise
 */
bool MasticQuick::_createParameter(QString name, MasticIopType::Value type, QVariant qmlValue, void* cValue, long cSize)
{
    bool result = false;

    // Check if we have a name
    if (!name.isEmpty())
    {
        // Check if it is a valid IOP name
        if (checkIfIopNameIsValid(name))
        {
            // Check if we must create a Mastic input
            const char* cName = name.toStdString().c_str();
            if (!mtic_checkParameterExistence(cName))
            {
                if (mtic_createParameter(cName, enumMasticIopTypeToEnumIopType_t(type), cValue, cSize) == 1)
                {
                    // Observe this new parameter
                    if (mtic_observeParameter(cName, &MasticQuick_callbackObserveParameter, this) != 1)
                    {
                        qWarning() << Q_FUNC_INFO << "warning: failed to observe parameter" << name;
                    }

                     // Add it to the list of QML dynamic properties
                    if (_parameters != NULL)
                    {
                        // Update QML
                        // NB: special case for impulsion properties because we don't want to trigger them at startup
                        if (type == MasticIopType::IMPULSION)
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
                    Q_EMIT parametersListChanged(_parametersList);


                    // Everything is ok
                    result = true;
                }
                else
                {
                    qWarning() << Q_FUNC_INFO << "warning: failed to create parameter" << name;
                }
            }
            else
            {
                qWarning() << Q_FUNC_INFO << "warning: parameter" << name << "already exists";
            }
        }
        else
        {
            qWarning() << Q_FUNC_INFO << "warning: '" << name << "' is an invalid parameter name, it conflicts with Qt internal symbols";
         }
    }
    else
    {
        qWarning() << Q_FUNC_INFO << "warning: can not create a parameter with an empty name";
    }

    return result;
}



/**
 * @brief Update our list of inputs
 */
void MasticQuick::_updateInputsList()
{
    QStringList newInputsList;

    // Get our inputs
    long numberOfInputs;
    char **inputs = mtic_getInputsList(&numberOfInputs);
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
void MasticQuick::_updateOutputsList()
{
    QStringList newOutputsList;

    // Get our outputs
    long numberOfOutputs;
    char **outputs = mtic_getOutputsList(&numberOfOutputs);
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
void MasticQuick::_updateParametersList()
{
    QStringList newParametersList;

    // Get our parameters
    long numberOfParameters;
    char **parameters = mtic_getParametersList(&numberOfParameters);
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
void MasticQuick::_onForcedStop()
{
    // Stop our Mastic agent
    stop();

    // Quit our application
    QCoreApplication::exit(0);
}



/**
 * @brief Called when an output is updated from QML
 * @param key
 * @param value
 */
void MasticQuick::_onOutputUpdatedFromFromQML(const QString &key, const QVariant &value)
{
    // Ensure that we have a valid key
    if (!key.isEmpty())
    {
        std::string stdName = key.toStdString();
        const char* cName = stdName.c_str();

        // Ensure that this output exists
        if (mtic_checkOutputExistence(cName))
        {
            iopType_t type = mtic_getTypeForOutput(cName);

            switch(type)
            {
                case INTEGER_T:
                    {
                        bool ok = false;
                        int cValue = value.toInt(&ok);

                        if (ok)
                        {
                            mtic_writeOutputAsInt(cName, cValue);
                        }
                        else
                        {
                            qWarning() << "MasticQuick warning: invalid value" << value
                                       << "for Mastic output" << key << "with type INTEGER";
                        }
                    }
                    break;


                case DOUBLE_T:
                    {
                        bool ok = false;
                        double cValue = value.toDouble(&ok);

                        if (ok)
                        {
                            mtic_writeOutputAsDouble(cName, cValue);
                        }
                        else
                        {
                            qWarning() << "MasticQuick warning: invalid value" << value
                                       << "for Mastic output" << key << "with type DOUBLE";
                        }
                    }
                    break;


                case STRING_T:
                    {
                        QString qmlValue = value.toString();
                        mtic_writeOutputAsString(cName, (char *)qmlValue.toStdString().c_str());
                    }
                    break;


                case BOOL_T:
                    {
                        mtic_writeOutputAsBool(cName, value.toBool());
                    }
                    break;


                case IMPULSION_T:
                    {
                        mtic_writeOutputAsImpulsion(cName);
                    }
                    break;

                case DATA_T:
                    {
                        qWarning() << "MasticQuick warning: can not update output" << key <<  "with type DATA (not yet implemented)";
                    }
                    break;

                default:
                    {
                        qWarning() << "MasticQuick warning: unhandled output type. Can not update output" << key;
                    }
                    break;
            }
        }
    }
}



/**
 * @brief Called when a parameter is updated from QML
 * @param key
 * @param value
 */
void MasticQuick::_onParameterUpdatedFromFromQML(const QString &key, const QVariant &value)
{
    // Ensure that we have a valid key
    if (!key.isEmpty())
    {
        std::string stdName = key.toStdString();
        const char* cName = stdName.c_str();

        // Ensure that this parameter exists
        if (mtic_checkParameterExistence(cName))
        {
            iopType_t type = mtic_getTypeForParameter(cName);

            switch(type)
            {
                case INTEGER_T:
                    {
                        bool ok = false;
                        int cValue = value.toInt(&ok);

                        if (ok)
                        {
                            mtic_writeParameterAsInt(cName, cValue);
                        }
                        else
                        {
                            qWarning() << "MasticQuick warning: invalid value" << value
                                       << "for Mastic parameter" << key << "with type INTEGER";
                        }
                    }
                    break;


                case DOUBLE_T:
                    {
                        bool ok = false;
                        double cValue = value.toDouble(&ok);

                        if (ok)
                        {
                            mtic_writeParameterAsDouble(cName, cValue);
                        }
                        else
                        {
                            qWarning() << "MasticQuick warning: invalid value" << value
                                       << "for Mastic parameter" << key << "with type DOUBLE";
                        }
                    }
                    break;


                case STRING_T:
                    {
                        QString qmlValue = value.toString();
                        mtic_writeParameterAsString(cName, (char *)qmlValue.toStdString().c_str());
                    }
                    break;


                case BOOL_T:
                    {
                        mtic_writeParameterAsBool(cName, value.toBool());
                    }
                    break;


                case IMPULSION_T:
                    {
                        qWarning() << "MasticQuick warning: invalid parameter type IMPULSION. Can not update parameter" << key;
                    }
                    break;


                default:
                    {
                        qWarning() << "MasticQuick warning: unhandled parameter type. Can not update parameter" << key;
                    }
                    break;
            }
        }
    }
}
