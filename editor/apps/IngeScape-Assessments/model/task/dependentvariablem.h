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

#ifndef DEPENDENTVARIABLEM_H
#define DEPENDENTVARIABLEM_H

#include <QObject>
#include <I2PropertyHelpers.h>

#include "cassandra.h"


/**
 * @brief The DependentVariableM class defines a model of dependent variable
 */
class DependentVariableM : public QObject
{
    Q_OBJECT

    // Name of our dependent variable
    I2_QML_PROPERTY(QString, name)

    // Description of our dependent variable
    I2_QML_PROPERTY(QString, description)

    // Name of the agent in the platform associated to the task
    I2_QML_PROPERTY(QString, agentName)

    // Name of the (agent) output in the platform associated to the task
    I2_QML_PROPERTY(QString, outputName)


public:
   /**
    * @brief DependentVariableM
    * @param experimentationUuid
    * @param taskUuid
    * @param cassUuid
    * @param name
    * @param description
    * @param agentName
    * @param outputName
    * @param parent
    */
    explicit DependentVariableM(CassUuid experimentationUuid
                                , CassUuid taskUuid
                                , CassUuid cassUuid
                                , const QString& name
                                , const QString& description
                                , const QString& agentName
                                , const QString& outputName
                                , QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    virtual ~DependentVariableM();

    /**
     * @brief Dependent variable table name
     */
    static const QString table;

    /**
     * @brief Dependent variable table column names
     */
    static const QStringList columnNames;

    /**
     * @brief Dependent variable table primary keys IN ORDER
     */
    static const QStringList primaryKeys;

    /**
     * @brief Static factory method to create an dependent variable from a CassandraDB record
     * @param row
     * @return
     */
    static DependentVariableM* createDependentVariableFromCassandraRow(const CassRow* row);

    /**
     * @brief Delete the given dependent variable from the Cassandra DB
     * @param row
     * @return
     */
    static void deleteDependentVariableFromCassandraDB(const DependentVariableM& entry);

    /**
     * @brief Update the given DependentVariableM into the Cassandra DB
     * @param entry
     * @return
     */
    static bool updateDependentVariableIntoCassandraDB(const DependentVariableM& entry);


    CassUuid getExperimentationCassUuid() const { return _experimentationCassUuid; }
    CassUuid getTaskCassUuid() const { return _taskCassUuid; }
    CassUuid getCassUuid() const { return _cassUuid; }

    /**
     * @brief Create a clone of the current object.
     * Return nullptr in case of failure.
     * Clone object does not have valid CassUuids.
     * The caller is in charge of freeing the returned instance (if any).
     * @return
     */
    DependentVariableM* clone() const;



protected: // Methods

protected: // Attributes
    // Task's experimentation's UUID from Cassandra DB
    CassUuid _experimentationCassUuid;

    // Task's UUID from Cassandra DB
    CassUuid _taskCassUuid;

    // Unique identifier in Cassandra Data Base
    CassUuid _cassUuid;


};

QML_DECLARE_TYPE(DependentVariableM)

#endif // DEPENDENTVARIABLEM_H
