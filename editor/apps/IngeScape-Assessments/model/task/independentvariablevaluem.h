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

/**
 * @brief Mostly empty class representing an independent variable value for a particular task instance.
 * This class is never instantiated and is there only to match the other model classes.
 */
class IndependentVariableValueM
{
public:
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

private:
    IndependentVariableValueM() = default;
};

#endif // INDEPENDENTVARIABLEVALUEM_H
