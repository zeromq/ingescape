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

#include "characteristicvaluem.h"

/**
 * @brief Characteristic value table name
 */
const QString CharacteristicValueM::table = "ingescape.characteristic_value";

/**
 * @brief Characteristic value table column names
 */
const QStringList CharacteristicValueM::columnNames = {
    "id_experimentation",
    "id_subject",
    "id_characteristic",
    "characteristic_value",
};

/**
 * @brief Characteristic value table primary keys IN ORDER
 */
const QStringList CharacteristicValueM::primaryKeys = {
    "id_experimentation",
    "id",
};
