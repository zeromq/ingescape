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

#include "independentvariablevaluem.h"

/**
 * @brief Independent variable value table name
 */
const QString IndependentVariableValueM::table = "ingescape.independent_var_value";

/**
 * @brief Independent variable value table column names
 */
const QStringList IndependentVariableValueM::columnNames = {
    "id_experimentation",
    "id",
    "name",
    "platform_file",
};

/**
 * @brief Independent variable value table primary keys IN ORDER
 */
const QStringList IndependentVariableValueM::primaryKeys = {
    "id_experimentation",
    "id",
};
