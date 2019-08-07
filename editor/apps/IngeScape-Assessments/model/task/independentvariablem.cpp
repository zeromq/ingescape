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

#include "independentvariablem.h"

/**
 * @brief Constructor
 * @param name
 * @param description
 * @param valueType
 * @param parent
 */
IndependentVariableM::IndependentVariableM(CassUuid experimentationUuid,
                                           CassUuid taskUuid,
                                           CassUuid uuid,
                                           QString name,
                                           QString description,
                                           IndependentVariableValueTypes::Value valueType,
                                           QStringList enumValues,
                                           QObject *parent) : QObject(parent),
    _name(name),
    _description(description),
    _valueType(valueType),
    _enumValues(enumValues),
    _experimentationCassUuid(experimentationUuid),
    _taskCassUuid(taskUuid),
    _cassUuid(uuid)

{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Model of Independent Variable" << _name << "of type" << IndependentVariableValueTypes::staticEnumToString(_valueType);
}


/**
 * @brief Destructor
 */
IndependentVariableM::~IndependentVariableM()
{
    qInfo() << "Delete Model of Independent Variable" << _name << "of type" << IndependentVariableValueTypes::staticEnumToString(_valueType);

}


/**
 * @brief Static factory method to create an independent variable from a CassandraDB record
 * @param row
 * @return
 */
IndependentVariableM* IndependentVariableM::createIndependentVariableFromCassandraRow(const CassRow* row)
{
    IndependentVariableM* independentVariable = nullptr;

    if (row != nullptr)
    {
        CassUuid experimentationUuid, taskUuid, independentVarUuid;
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id_experimentation"), &experimentationUuid);
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id_task"), &taskUuid);
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id"), &independentVarUuid);

        const char *chrVariableName = "";
        size_t varNameLength = 0;
        cass_value_get_string(cass_row_get_column_by_name(row, "name"), &chrVariableName, &varNameLength);
        QString variableName = QString::fromUtf8(chrVariableName, static_cast<int>(varNameLength));

        const char *chrVariableDescription = "";
        size_t varDescriptionLength = 0;
        cass_value_get_string(cass_row_get_column_by_name(row, "description"), &chrVariableDescription, &varDescriptionLength);
        QString variableDescription(QString::fromUtf8(chrVariableDescription, static_cast<int>(varDescriptionLength)));

        int8_t i8ValueType = 0;
        cass_value_get_int8(cass_row_get_column_by_name(row, "value_type"), &i8ValueType);
        IndependentVariableValueTypes::Value valueType = static_cast<IndependentVariableValueTypes::Value>(i8ValueType);

        QStringList enumValues;
        CassIterator* enumValuesIterator = cass_iterator_from_collection(cass_row_get_column_by_name(row, "enum_values"));
        if (enumValuesIterator != nullptr) {
            while(cass_iterator_next(enumValuesIterator)) {
                const char *chrEnumValue = "";
                size_t enumValueLength = 0;
                cass_value_get_string(cass_iterator_get_value(enumValuesIterator), &chrEnumValue, &enumValueLength);
                enumValues.append(QString::fromUtf8(chrEnumValue, static_cast<int>(enumValueLength)));
            }

            cass_iterator_free(enumValuesIterator);
            enumValuesIterator = nullptr;
        }

        independentVariable = new IndependentVariableM(experimentationUuid, taskUuid, independentVarUuid, variableName, variableDescription, valueType, enumValues);
    }

    return independentVariable;
}
