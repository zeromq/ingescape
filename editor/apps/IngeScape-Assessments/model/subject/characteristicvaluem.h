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

/**
 * @brief Mostly empty class representing a characteristic value for a particular subject.
 * This class is never instantiated and is there only to match the other model classes.
 */
class CharacteristicValueM {
    public:
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

    private:
        CharacteristicValueM() = default;
};

#endif // CHARACTERISTICVALUEM_H
