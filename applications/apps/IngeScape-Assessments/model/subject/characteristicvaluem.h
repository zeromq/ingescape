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

#ifndef CHARACTERISTICVALUEM_H
#define CHARACTERISTICVALUEM_H

#include <QStringList>
#include "cassandra.h"

/**
 * @brief Mostly empty class representing a characteristic value for a particular subject.
 * This class is a "fancy-struct" with all its attributes publicly accessible.
 * Its purpose is to handle more easily the DB entries and should no be kept in collections by controllers.
 * See SubjectM for more details on how characteristic values are stored for QML to use them.
 */
class CharacteristicValueM {

    public:
        /**
         * @brief Constructor setting all parameters
         * @param experimentationUuid
         * @param subjectUuid
         * @param characteristicUuid
 * @param valueString
         */
        CharacteristicValueM(const CassUuid& experimentationUuid,
                             const CassUuid& subjectUuid,
                             const CassUuid& characteristicUuid,
                             const QString& valueString);

        /**
         * @brief Experimentation's UUID
         */
        CassUuid experimentationUuid;

        /**
         * @brief Subject's UUID
         */
        CassUuid subjectUuid;

        /**
         * @brief Characteristic's UUID
         */
        CassUuid characteristicUuid;

        /**
         * @brief The characteristic value as a QString
         */
        QString valueString;

        /**
         * @brief Characteristic value table name
         */
        static const QString table;

        /**
         * @brief Characteristic value table column names
         */
        static const QStringList columnNames;

        /**
         * @brief Characteristic value table primary keys IN ORDER
         */
        static const QStringList primaryKeys;

        /**
         * @brief Static factory method to create a characteristic value from a CassandraDB record
         * @param row
         * @return
         */
        static CharacteristicValueM* createFromCassandraRow(const CassRow* row);

        /**
         * @brief Create a CassStatement to insert a CharacteristicValueM into the DB.
         * The statement contains the values from the given characteristicValue.
         * Passed characteristicValue must have a valid and unique UUID.
         * @param characteristicValue
         * @return
         */
        static CassStatement* createBoundInsertStatement(const CharacteristicValueM& characteristicValue);

        /**
         * @brief Create a CassStatement to update a CharacteristicValueM into the DB.
         * The statement contains the values from the given characteristicValue.
         * Passed characteristicValue must have a valid and unique UUID.
         * @param characteristicValue
         * @return
         */
        static CassStatement* createBoundUpdateStatement(const CharacteristicValueM& characteristicValue);
};

#endif // CHARACTERISTICVALUEM_H
