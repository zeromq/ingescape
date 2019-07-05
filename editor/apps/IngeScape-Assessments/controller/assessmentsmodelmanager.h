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
#include <model/assessmentsenums.h>
#include <controller/ingescapemodelmanager.h>
//#include <model/experimentationm.h>
#include <viewModel/experimentationsgroupvm.h>
#include "cassandra.h"


/**
 * @brief The AssessmentsModelManager class defines the manager for the data model of our IngeScape Assessments application
 */
class AssessmentsModelManager : public IngeScapeModelManager
{
    Q_OBJECT

    // Cassandra server
    I2_QML_PROPERTY(QString, cassServer)

    // The experimentations group currently selected
    I2_QML_PROPERTY(ExperimentationsGroupVM*, currentExperimentationsGroup)

    // The experimentation currently selected
    I2_QML_PROPERTY(ExperimentationM*, currentExperimentation)


public:

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
    ~AssessmentsModelManager();
    //~AssessmentsModelManager() Q_DECL_OVERRIDE;


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


Q_SIGNALS:


public Q_SLOTS:


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


    // A UUID generator object
    CassUuidGen* _cassUuidGen = nullptr;

};

QML_DECLARE_TYPE(AssessmentsModelManager)

#endif // ASSESSMENTSMODELMANAGER_H
