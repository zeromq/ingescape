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
