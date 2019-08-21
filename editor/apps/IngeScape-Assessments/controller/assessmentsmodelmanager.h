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

#ifndef ASSESSMENTSMODELMANAGER_H
#define ASSESSMENTSMODELMANAGER_H

#include <QObject>
#include <I2PropertyHelpers.h>
#include <controller/ingescapemodelmanager.h>

#include "cassandra.h"


/**
 * @brief Equality operator to be able to compare CassUuid struct directly.
 * Required to use CassUuid type as QHash key.
 * @param left
 * @param right
 * @return
 */
bool operator==(const CassUuid& left, const CassUuid& right);

/**
 * @brief qHash function to compute a hash key for the given CassUuid.
 * Required to use CassUuid type as QHash key.
 * @param cassUuid
 * @return
 */
uint qHash(const CassUuid& cassUuid);


/**
 * @brief The AssessmentsModelManager class defines the manager for the data model of our IngeScape Assessments application
 */
class AssessmentsModelManager : public IngeScapeModelManager
{
    Q_OBJECT


public:

    // ------------------------
    //
    //  Singleton logic
    //
    // ------------------------

    /**
     * @brief Initialize the singleton instance, destroying any previous instance if any
     * @param jsonHelper
     * @param rootDirectoryPath
     * @param parent
     */
    static void initInstance(JsonHelper* jsonHelper,
                             QString rootDirectoryPath,
                             QObject *parent = nullptr);

    /**
     * @brief Destroy the current singleton instance
     */
    static void destroyInstance();

    /**
     * @brief Accessor to the singleton instance
     * @return
     */
    static AssessmentsModelManager* Instance();

    /**
     * @brief Retrieve a 'text' value of given column inside the given row
     * and convert it to QString before returning it
     * @param row
     * @param columnName
     * @return
     */
    static QString getStringValueFromColumnName(const CassRow* row, const char* columnName);

    /**
     * @brief Retrive a full collection of 'text' for the given value inside the given row
     * @param row
     * @param columnName
     * @return
     */
    static QStringList getStringListFromColumnName(const CassRow* row, const char* columnName);

    /**
     * @brief Retrive a date and a time value from the given columns inside the given row
     * and convert it to a QDateTime before returning it
     * @param row
     * @param dateColumnName
     * @param timeColumnName
     * @return
     */
    static QDateTime getDateTimeFromColumnNames(const CassRow* row, const char* dateColumnName, const char* timeColumnName);


    // ------------------------
    //
    //  Model manager logic
    //
    // ------------------------

    /**
     * @brief Get the Cassandra Cluster
     * @return
     */
    CassCluster* getCassCluster();


    /**
     * @brief Get the Cassandra Session
     * @return
     */
    CassSession* getCassSession();


    /**
     * @brief Get the Cassandra UUID generator
     * @return
     */
    CassUuidGen* getCassUuidGen();


    /**
     * @brief Converts the given CassUuid into a readable QString
     * This is a static utility function and does not interact with the Cassandra server
     * @param cassUuid
     * @return
     */
    static QString cassUuidToQString(CassUuid cassUuid);


private:
    /**
     * @brief The singleton instance.
     * NOTE: We hold a pointer here and not a plain value to be able to initialize it from the main controller without a "default value".
     */
    static AssessmentsModelManager* _instance;

    /**
     * @brief Constructor
     * @param jsonHelper
     * @param rootDirectoryPath
     * @param parent
     */
    explicit AssessmentsModelManager(JsonHelper* jsonHelper,
                                     QString rootDirectoryPath,
                                     QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~AssessmentsModelManager() Q_DECL_OVERRIDE;


private:
    /**
     * @brief A cluster object describes the configuration of the Cassandra cluster and is used
     * to construct a session instance. Unlike other DataStax drivers the cluster object
     * does not maintain the control connection.
     */
    CassCluster* _cassCluster = nullptr;

    /**
     * @brief A session object is used to execute queries and maintains cluster state through
     * the control connection. The control connection is used to auto-discover nodes and
     * monitor cluster changes (topology and schema). Each session also maintains multiple
     * pools of connections to cluster nodes which are used to query the cluster.
     */
    CassSession* _cassSession = nullptr;

    /**
     * @brief The Cassandra server URL
     */
    QString _cassServer;

    /**
     * @brief A UUID generator privoded by Cassandra
     */
    CassUuidGen* _cassUuidGen = nullptr;

};

QML_DECLARE_TYPE(AssessmentsModelManager)

#endif // ASSESSMENTSMODELMANAGER_H
