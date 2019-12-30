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

#ifndef INDEPENDENTVARIABLEM_H
#define INDEPENDENTVARIABLEM_H

#include <QObject>
#include <I2PropertyHelpers.h>
#include <model/assessmentsenums.h>

#include "cassandra.h"

/**
 * @brief The IndependentVariableM class defines a model of independent variable
 */
class IndependentVariableM : public QObject
{
    Q_OBJECT

    // Name of our independent variable
    I2_QML_PROPERTY(QString, name)

    // Description of our independent variable
    I2_QML_PROPERTY(QString, description)

    // Unit of our independent variable
    //I2_QML_PROPERTY(QString, unit)

    // Type of our independent variable value
    I2_QML_PROPERTY(IndependentVariableValueTypes::Value, valueType)

    // List of possible values if the value type is "INDEPENDENT_VARIABLE_ENUM"
    I2_QML_PROPERTY(QStringList, enumValues)


public:
    // TODO Comment
    explicit IndependentVariableM(CassUuid experimentationUuid,
                                  CassUuid protocolUuid,
                                  CassUuid uuid,
                                  QString name,
                                  QString description,
                                  IndependentVariableValueTypes::Value valueType,
                                  QStringList enumValues = {},
                                  QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    virtual ~IndependentVariableM();


    /**
     * @brief Accessor for this independent variable's task's experiment UUID in the Cassandra DB
     * @return
     */
    CassUuid getExperimentationCassUuid() const { return _experimentationCassUuid; }


    /**
     * @brief Accessor for this independent variable's protocol UUID in the Cassandra DB
     * @return
     */
    CassUuid getProtocolCassUuid() const { return _protocolCassUuid; }


    /**
     * @brief Accessor for this independent variable UUID in the Cassandra DB
     * @return
     */
    CassUuid getCassUuid() const { return _cassUuid; }


    /**
     * @brief Independent variable table name
     */
    static const QString table;


    /**
     * @brief Independent variable table column names
     */
    static const QStringList columnNames;


    /**
     * @brief Independent variable table primary keys IN ORDER
     */
    static const QStringList primaryKeys;


    /**
     * @brief Static factory method to create an independent variable from a CassandraDB record
     * @param row
     * @return
     */
    static IndependentVariableM* createFromCassandraRow(const CassRow* row);


    /**
     * @brief Delete the given independent variable from Cassandra DB
     * @param independentVariable
     * @return
     */
    static void deleteIndependentVariableFromCassandra(const IndependentVariableM& independentVariable);


    /**
     * @brief Create a CassStatement to insert an IndependentVariableM into the DB.
     * The statement contains the values from the given independentVariable.
     * Passed independentVariable must have a valid and unique UUID.
     * @param independentVariable
     * @return
     */
    static CassStatement* createBoundInsertStatement(const IndependentVariableM& independentVariable);


    /**
     * @brief Create a CassStatement to update an IndependentVariableM into the DB.
     * The statement contains the values from the given independentVariable.
     * Passed independentVariable must have a valid and unique UUID.
     * @param independentVariable
     * @return
     */
    static CassStatement* createBoundUpdateStatement(const IndependentVariableM& independentVariable);


private:
    // Protocol's experimentation's UUID from Cassandra DB
    CassUuid _experimentationCassUuid;

    // Protocol's UUID from Cassandra DB
    CassUuid _protocolCassUuid;

    // Independent variable's UUID from Cassandra DB
    CassUuid _cassUuid;
};

QML_DECLARE_TYPE(IndependentVariableM)

#endif // INDEPENDENTVARIABLEM_H
