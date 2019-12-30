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
    * @brief Constructor
    * @param experimentationUuid
    * @param protocolUuid
    * @param cassUuid
    * @param name
    * @param description
    * @param agentName
    * @param outputName
    * @param parent
    */
    explicit DependentVariableM(CassUuid experimentationUuid
                                , CassUuid protocolUuid
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
    static DependentVariableM* createFromCassandraRow(const CassRow* row);


    /**
     * @brief Create a CassStatement to insert a DependentVariableM into the DB.
     * The statement contains the values from the given dependentVariable.
     * Passed dependentVariable must have a valid and unique UUID.
     * @param dependentVariable
     * @return
     */
    static CassStatement* createBoundInsertStatement(const DependentVariableM& dependentVariable);


    /**
     * @brief Create a CassStatement to update a DependentVariableM into the DB.
     * The statement contains the values from the given dependentVariable.
     * Passed dependentVariable must have a valid and unique UUID.
     * @param dependentVariable
     * @return
     */
    static CassStatement* createBoundUpdateStatement(const DependentVariableM& dependentVariable);


    CassUuid getExperimentationCassUuid() const { return _experimentationCassUuid; }
    CassUuid getProtocolCassUuid() const { return _protocolCassUuid; }
    CassUuid getCassUuid() const { return _cassUuid; }

protected: // Attributes
    // Protocol's experimentation's UUID from Cassandra DB
    CassUuid _experimentationCassUuid;

    // Protocol's UUID from Cassandra DB
    CassUuid _protocolCassUuid;

    // Unique identifier in Cassandra Data Base
    CassUuid _cassUuid;


};

QML_DECLARE_TYPE(DependentVariableM)

#endif // DEPENDENTVARIABLEM_H
