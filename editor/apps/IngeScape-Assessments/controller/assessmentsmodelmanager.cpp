/*
 *	IngeScape Assessments
 *
 *  Copyright © 2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

#include "assessmentsmodelmanager.h"



/**
 * @brief Equality operator to be able to compare CassUuid struct directly.
 * Required to use CassUuid type as QHash key.
 * @param left
 * @param right
 * @return
 */
bool operator==(const CassUuid& left, const CassUuid& right)
{
    return (left.time_and_version == right.time_and_version) && (left.clock_seq_and_node == right.clock_seq_and_node);
}

/**
 * @brief qHash function to compute a hash key for the given CassUuid.
 * Required to use CassUuid type as QHash key.
 * @param cassUuid
 * @return
 */
uint qHash(const CassUuid& cassUuid)
{
    return static_cast<uint>(cassUuid.time_and_version * cassUuid.clock_seq_and_node);
}


/**
 * @brief The singleton instance.
 * NOTE: We hold a pointer here and not a plain value to be able to initialize it from the main controller without a "default value".
 */
AssessmentsModelManager* AssessmentsModelManager::_instance = nullptr;

/**
 * @brief Initialize the singleton instance, destroying any previous instance if any
 * @param jsonHelper
 * @param rootDirectoryPath
 * @param parent
 */
void AssessmentsModelManager::initInstance(JsonHelper* jsonHelper, QString rootDirectoryPath, QObject* parent)
{
    if (AssessmentsModelManager::_instance != nullptr)
    {
        AssessmentsModelManager::destroyInstance();
    }

    AssessmentsModelManager::_instance = new AssessmentsModelManager(jsonHelper, rootDirectoryPath, parent);
}

/**
 * @brief Destroy the current singleton instance
 */
void AssessmentsModelManager::destroyInstance()
{
    if (AssessmentsModelManager::_instance != nullptr)
    {
        delete AssessmentsModelManager::_instance;
        AssessmentsModelManager::_instance = nullptr;
    }
}

/**
 * @brief Accessor to the singleton instance
 * @return
 */
AssessmentsModelManager* AssessmentsModelManager::Instance()
{
    return AssessmentsModelManager::_instance;
}

/**
 * @brief Constructor
 * @param jsonHelper
 * @param rootDirectoryPath
 * @param parent
 */
AssessmentsModelManager::AssessmentsModelManager(JsonHelper* jsonHelper,
                                                 QString rootDirectoryPath,
                                                 QObject *parent) : IngeScapeModelManager(jsonHelper,
                                                                                          rootDirectoryPath,
                                                                                          parent),
    _cassCluster(nullptr),
    _cassSession(nullptr),
    _cassServer("localhost"),
    _cassUuidGen(nullptr)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New IngeScape Assessments Model Manager";


    //
    // Init about Cassandra DataBase
    //
    _cassCluster = cass_cluster_new();
    _cassSession = cass_session_new();
    _cassUuidGen = cass_uuid_gen_new();

    cass_cluster_set_contact_points(_cassCluster, _cassServer.toStdString().c_str());

    // Connects a session
    CassFuture* cassFuture = cass_session_connect(_cassSession, _cassCluster);
    CassError cassError = cass_future_error_code(cassFuture);
    if (cassError == CASS_OK) {
        qInfo() << "Connected to the Cassandra DataBase on" << _cassServer;
    }
    else {
        qCritical() << "Could not connect to the Cassandra DataBase on" << _cassServer << "(" << cass_error_desc(cassError) << ")";
    }

    cass_future_free(cassFuture);
}


/**
 * @brief Destructor
 */
AssessmentsModelManager::~AssessmentsModelManager()
{
    qInfo() << "Delete IngeScape Assessments Model Manager";

    //
    // Cleaning about Cassandra DataBase
    //
    if (_cassUuidGen != nullptr) {
        cass_uuid_gen_free(_cassUuidGen);
    }

    if (_cassSession != nullptr)
    {
        // Closes the session instance, outputs a close future which can be used to determine when the session has been terminated.
        // This allows in-flight requests to finish.
        //cass_session_close(_cassSession);

        cass_session_free(_cassSession);
    }

    if (_cassCluster != nullptr) {
        cass_cluster_free(_cassCluster);
    }

    // Mother class is automatically called
    //IngeScapeModelManager::~IngeScapeModelManager();
}


/**
 * @brief Get the Cassandra Cluster
 * @return
 */
CassCluster* AssessmentsModelManager::getCassCluster()
{
    return _cassCluster;
}


/**
 * @brief Get the Cassandra Session
 * @return
 */
CassSession* AssessmentsModelManager::getCassSession()
{
    return _cassSession;
}


/**
 * @brief Get the Cassandra UUID generator
 * @return
 */
CassUuidGen* AssessmentsModelManager::getCassUuidGen()
{
    return _cassUuidGen;
}


/**
 * @brief Converts the given CassUuid into a readable QString
 * This is a static utility function and does not interact with the Cassandra server
 * @param cassUuid
 * @return
 */
QString AssessmentsModelManager::cassUuidToQString(CassUuid cassUuid)
{
    char chrTaskUuid[CASS_UUID_STRING_LENGTH];
    cass_uuid_string(cassUuid, chrTaskUuid);
    return QString(chrTaskUuid);
}
