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
 *      Mathieu Soum     <soum@ingenuity.io>
 *
 */

#ifndef INDEPENDENTVARIABLEVALUEM_H
#define INDEPENDENTVARIABLEVALUEM_H

#include <QStringList>
#include "cassandra.h"

/**
 * @brief Mostly empty class representing an independent variable value for a particular session.
 * This class is a "fancy-struct" with all its attributes publicly accessible.
 * Its purpose is to handle more easily the DB entries and should no be kept in collections by controllers.
 * See SessionM for more details on how independent variable values values are stored for QML to use them.
 */
class IndependentVariableValueM
{
public:
    /**
     * @brief Constructor setting all parameters
     * @param experimentationUuid
     * @param sessionUuid
     * @param independentVariableUuid
     * @param valueString
     */
    IndependentVariableValueM(const CassUuid& experimentationUuid,
                              const CassUuid& sessionUuid,
                              const CassUuid& independentVariableUuid,
                              const QString& valueString);

    /**
     * @brief Experimentation's UUID
     */
    CassUuid experimentationUuid;

    /**
     * @brief Session's UUID
     */
    CassUuid sessionUuid;

    /**
     * @brief Independent variable's UUID
     */
    CassUuid independentVariableUuid;

    /**
     * @brief The independent variable value as a QString
     */
    QString valueString;

    /**
     * @brief Independent variable value table name
     */
    static const QString table;

    /**
     * @brief Independent variable value table column names
     */
    static const QStringList columnNames;

    /**
     * @brief Independent variable value table primary keys IN ORDER
     */
    static const QStringList primaryKeys;

    /**
     * @brief Static factory method to create an independent variable value from a CassandraDB record
     * @param row
     * @return
     */
    static IndependentVariableValueM* createFromCassandraRow(const CassRow* row);

    /**
     * @brief Create a CassStatement to insert a IndependentVariableValueM into the DB.
     * The statement contains the values from the given independent variable value.
     * Passed independent variable value must have a valid and unique UUID.
     * @param independentVariableValue
     * @return
     */
    static CassStatement* createBoundInsertStatement(const IndependentVariableValueM& independentVariableValue);

    /**
     * @brief Create a CassStatement to update a IndependentVariableValueM into the DB.
     * The statement contains the values from the given independent variable value.
     * Passed independent variable value must have a valid and unique UUID.
     * @param independentVariableValue
     * @return
     */
    static CassStatement* createBoundUpdateStatement(const IndependentVariableValueM& independentVariableValue);
};

#endif // INDEPENDENTVARIABLEVALUEM_H
