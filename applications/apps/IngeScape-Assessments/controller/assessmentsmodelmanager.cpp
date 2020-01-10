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
 *      Chloé Roumieu      <roumieu@ingenuity.io>
 */

#include "assessmentsmodelmanager.h"

#include "settings/ingescapesettings.h"



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


//--------------------------------------------------------------
//
//
// Assessments Model Manager
//
//
//--------------------------------------------------------------

// Define our singleton instance
// Creates a global and static object of type QGlobalStatic, of name _singletonInstance and that behaves as a pointer to AssessmentsModelManager.
// The object created by Q_GLOBAL_STATIC initializes itself on the first use, which means that it will not increase the application or the library's load time.
// Additionally, the object is initialized in a thread-safe manner on all platforms.
Q_GLOBAL_STATIC(AssessmentsModelManager, _singletonInstance)


/**
 * @brief Get our singleton instance
 * @return
 */
AssessmentsModelManager* AssessmentsModelManager::instance()
{
    return _singletonInstance;
}


/**
 * @brief Method used to provide a singleton to QML
 * @param engine
 * @param scriptEngine
 * @return
 */
QObject* AssessmentsModelManager::qmlSingleton(QQmlEngine* engine, QJSEngine* scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    return _singletonInstance;
}


/**
 * @brief Constructor
 * @param parent
 */
AssessmentsModelManager::AssessmentsModelManager(QObject *parent) : QObject(parent),
    _databaseAddress("localhost"),
    _isConnectedToDatabase(false),
    _errorMessageWhenDatabaseConnectionFailed(""),
    _cassCluster(nullptr),
    _cassSession(nullptr),
    _cassUuidGen(nullptr)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New IngeScape Assessments Model Manager";


    // Retrieve the IP Address of the (Cassandra) database
    QVariant tempDatabaseAddress = IngeScapeSettings::Instance().value("network/databaseAddress");
    if (tempDatabaseAddress.isValid())
    {
        setdatabaseAddress(tempDatabaseAddress.toString());
        qInfo() << "Database address" << _databaseAddress;
    }

    // Connect to the Cassandra database
    connectToDatabase();
}


/**
 * @brief Destructor
 */
AssessmentsModelManager::~AssessmentsModelManager()
{
    qInfo() << "Delete IngeScape Assessments Model Manager";

    // DIS-connect from the Cassandra Database
    disconnectFromDatabase();
}


/**
 * @brief Setter for property "Database Address"
 * @param value
 */
void AssessmentsModelManager::setdatabaseAddress(QString value)
{
    if (_databaseAddress != value)
    {
        _databaseAddress = value;

        // Update settings file
        IngeScapeSettings &settings = IngeScapeSettings::Instance();

        settings.setValue("network/databaseAddress", _databaseAddress);

        // Save new values
        settings.sync();

        Q_EMIT databaseAddressChanged(value);
    }
}


/**
 * @brief Retrieve a 'text' value of given column inside the given row
 * and convert it to QString before returning it
 * @param row
 * @param columnName
 * @return
 */
QString AssessmentsModelManager::getStringValueFromColumnName(const CassRow* row, const char* columnName)
{
    const char *chrValueString = "";
    size_t valueStringLength = 0;
    cass_value_get_string(cass_row_get_column_by_name(row, columnName), &chrValueString, &valueStringLength);
    return QString::fromUtf8(chrValueString, static_cast<int>(valueStringLength));
}


/**
 * @brief Retrive a full collection of 'text' for the given value inside the given row
 * @param row
 * @param columnName
 * @return
 */
QStringList AssessmentsModelManager::getStringListFromColumnName(const CassRow* row, const char* columnName)
{
    QStringList collection;
    CassIterator* enumValuesIterator = cass_iterator_from_collection(cass_row_get_column_by_name(row, columnName));
    if (enumValuesIterator != nullptr) {
        while(cass_iterator_next(enumValuesIterator)) {
            const char *chrEnumValue = "";
            size_t enumValueLength = 0;
            cass_value_get_string(cass_iterator_get_value(enumValuesIterator), &chrEnumValue, &enumValueLength);
            collection.append(QString::fromUtf8(chrEnumValue, static_cast<int>(enumValueLength)));
        }

        cass_iterator_free(enumValuesIterator);
        enumValuesIterator = nullptr;
    }
    return collection;
}


/**
 * @brief Retrieve an 'int64' value of given column inside the given row
 * and convert it to int before returning it
 * @param row
 * @param columnName
 * @return
 */
int AssessmentsModelManager::getIntValueFromColumnName(const CassRow* row, const char* columnName)
{
    cass_int64_t valueInt = 0;
    cass_value_get_int64(cass_row_get_column_by_name(row, columnName), &valueInt);
    return static_cast<int>(valueInt);
}


/**
 * @brief Retrieve an 'int32' value of given column inside the given row
 * and convert it to int before returning it
 * @param row
 * @param columnName
 * @return
 */
int AssessmentsModelManager::getInt32ValueFromColumnName(const CassRow* row, const char* columnName)
{
    cass_int32_t valueInt = 0;
    cass_value_get_int32(cass_row_get_column_by_name(row, columnName), &valueInt);
    return static_cast<int>(valueInt);
}


/**
 * @brief Retrieve an 'int8' value of given column inside the given row
 * and convert it to int before returning it
 * @param row
 * @param columnName
 * @return
 */
int AssessmentsModelManager::getTinyIntValueFromColumnName(const CassRow* row, const char* columnName)
{
    cass_int8_t valueInt = 0;
    cass_value_get_int8(cass_row_get_column_by_name(row, columnName), &valueInt);
    return static_cast<int>(valueInt);
}


/**
 * @brief Retrive a date and a time value from the given columns inside the given row
 * and convert it to a QDateTime before returning it
 * @param row
 * @param dateColumnName
 * @param timeColumnName
 * @return
 */
QDateTime AssessmentsModelManager::getDateTimeFromColumnNames(const CassRow* row, const char* dateColumnName, const char* timeColumnName)
{
    cass_uint32_t yearMonthDay;
    cass_value_get_uint32(cass_row_get_column_by_name(row, dateColumnName), &yearMonthDay);

    cass_int64_t timeOfDay;
    cass_value_get_int64(cass_row_get_column_by_name(row, timeColumnName), &timeOfDay);

    // Convert in time to epoch
    time_t timeToEpoch = cass_date_time_to_epoch(yearMonthDay, timeOfDay);
    cass_int64_t millisecondsSinceStartOfDay = (timeOfDay/1000000)%1000;

    return QDateTime::fromMSecsSinceEpoch(timeToEpoch*1000 + millisecondsSinceStartOfDay);
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
 * @brief Connect to the Cassandra Database
 */
void AssessmentsModelManager::connectToDatabase()
{
    // If we are already connected to a database...
    if (_isConnectedToDatabase)
    {
        // ...first, disconnect from this database
        disconnectFromDatabase();
    }

    if (!_isConnectedToDatabase)
    {
        if ((_cassCluster == nullptr) && (_cassSession == nullptr) && (_cassUuidGen == nullptr))
        {
            // Init
            _cassCluster = cass_cluster_new();
            _cassSession = cass_session_new();
            _cassUuidGen = cass_uuid_gen_new();

            // Establishing the connection
            cass_cluster_set_contact_points(_cassCluster, _databaseAddress.toStdString().c_str());

            // Connects a session
            CassFuture* cassFuture = cass_session_connect(_cassSession, _cassCluster);
            //CassFuture* cassFuture = cass_session_connect_keyspace(_cassSession, _cassCluster, "ingescape");

            CassError cassError = cass_future_error_code(cassFuture);

            if (cassError == CASS_OK)
            {
                qInfo() << "Connected to the Cassandra database on" << _databaseAddress;

                // Reset the error message and set the flag
                //seterrorMessageWhenDatabaseConnectionFailed("");
                seterrorMessageWhenDatabaseConnectionFailed(tr("Connected to the Cassandra database on '%1'").arg(_databaseAddress));
                setisConnectedToDatabase(true);
            }
            else
            {
                qCritical() << "Could not connect to the Cassandra database on" << _databaseAddress << "(" << cass_error_desc(cassError) << ")";

                // Set the error message
                seterrorMessageWhenDatabaseConnectionFailed(tr("Failed to connect to the Cassandra database on '%1'\nMust be 'localhost' or an IP address").arg(_databaseAddress));
                setisConnectedToDatabase(false);
            }

            cass_future_free(cassFuture);

            if (!_isConnectedToDatabase)
            {
                cass_uuid_gen_free(_cassUuidGen);
                _cassUuidGen = nullptr;

                cass_session_free(_cassSession);
                _cassSession = nullptr;

                cass_cluster_free(_cassCluster);
                _cassCluster = nullptr;
            }
        }
    }
}


/**
 * @brief DIS-connect from the Cassandra Database
 */
void AssessmentsModelManager::disconnectFromDatabase()
{
    if (_isConnectedToDatabase)
    {
        if ((_cassCluster != nullptr) && (_cassSession != nullptr) && (_cassUuidGen != nullptr))
        {
            // Closes the session instance, outputs a close future which can be used to determine when the session has been terminated.
            // This allows in-flight requests to finish.
            CassFuture* cassFuture = cass_session_close(_cassSession);

            CassError cassError = cass_future_error_code(cassFuture);

            if (cassError == CASS_OK) {
                qInfo() << "DIS-connected from the Cassandra database";

                // Update the flag
                setisConnectedToDatabase(false);
            }
            else {
                qCritical() << "Could not DIS-connect from the Cassandra database (" << cass_error_desc(cassError) << ")";
            }

            cass_future_free(cassFuture);

            if (!_isConnectedToDatabase)
            {
                cass_uuid_gen_free(_cassUuidGen);
                _cassUuidGen = nullptr;

                cass_session_free(_cassSession);
                _cassSession = nullptr;

                cass_cluster_free(_cassCluster);
                _cassCluster = nullptr;
            }
        }
    }
}


/**
 * @brief Converts the given CassUuid into a readable QString
 * This is a static utility function and does not interact with the Cassandra server
 * @param cassUuid
 * @return
 */
QString AssessmentsModelManager::cassUuidToQString(CassUuid cassUuid)
{
    char chrCassUuid[CASS_UUID_STRING_LENGTH];
    cass_uuid_string(cassUuid, chrCassUuid);
    return QString(chrCassUuid);
}


/**
 * @brief Converts the given QString into a CassUuid
 * This is a static utility function and does not interact with the Cassandra server
 * @param stringCassUuid
 * @return
 */
CassUuid AssessmentsModelManager::qStringToCassUuid(QString stringCassUuid)
{
    const char * convertStringCassUuid = stringCassUuid.toStdString().c_str();
    CassUuid cassUuid;
    cass_uuid_from_string(convertStringCassUuid, &cassUuid);
    return cassUuid;
}


/**
 * @brief Helper function generating a Cassandra UUID
 * @return
 */
CassUuid AssessmentsModelManager::genCassUuid()
{
    CassUuid uuid;
    cass_uuid_gen_time(AssessmentsModelManager::instance()->getCassUuidGen(), &uuid);
    return uuid;
}
