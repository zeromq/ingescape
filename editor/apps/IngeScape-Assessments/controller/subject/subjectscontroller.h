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

#ifndef SUBJECTSCONTROLLER_H
#define SUBJECTSCONTROLLER_H

#include <QObject>
#include <I2PropertyHelpers.h>

#include <model/assessmentsenums.h>
#include <model/experimentationm.h>


/**
 * @brief The SubjectsController class defines the controller to manage the subjects of the current experimentation
 */
class SubjectsController : public QObject
{
    Q_OBJECT

    // List of all types for characteristic value
    I2_ENUM_LISTMODEL(CharacteristicValueTypes, allCharacteristicValueTypes)

    // Model of the current experimentation
    I2_QML_PROPERTY_READONLY_CUSTOM_SETTER(ExperimentationM*, currentExperimentation)


public:

    /**
     * @brief Constructor
     * @param modelManager
     * @param parent
     */
    explicit SubjectsController(QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~SubjectsController();


    /**
     * @brief Return true if the user can create a characteristic with the name
     * Check if the name is not empty and if a characteristic with the same name does not already exist
     * @param characteristicName
     * @return
     */
    Q_INVOKABLE bool canCreateCharacteristicWithName(QString characteristicName);


    /**
     * @brief Create a new characteristic
     * @param characteristicName
     * @param nCharacteristicValueType
     */
    Q_INVOKABLE void createNewCharacteristic(const QString& characteristicName, int nCharacteristicValueType, const QStringList& enumValues = {});


    /**
     * @brief Create a new characteristic of type enum
     * @param characteristicName
     * @param enumValues
     */
    Q_INVOKABLE void createNewCharacteristicEnum(QString characteristicName, QStringList enumValues);


    /**
     * @brief Delete the given characteristic from the current experimentation and from the Cassandra DB
     * @param characteristic
     */
    Q_INVOKABLE void deleteCharacteristic(CharacteristicM* characteristic);


    /**
     * @brief Create a new subject
     */
    Q_INVOKABLE void createNewSubject();


    /**
     * @brief Delete the given subject from the current experimentation and from the Cassandra DB
     * @param subject
     */
    Q_INVOKABLE void deleteSubject(SubjectM* subject);


private Q_SLOTS:

    /**
     * @brief Slot called when the current experimentation changed
     * @param currentExperimentation
     */
    void _onCurrentExperimentationChanged(ExperimentationM* currentExperimentation);


    /**
     * @brief Creates a new characteristic with the given parameters and insert it into the Cassandra DB
     * A nullptr is returned if the operation fails
     * @param experimentationUuid
     * @param name
     * @param valueType
     * @param enumValues
     */
    CharacteristicM* _insertCharacteristicIntoDB(CassUuid experimentationUuid, const QString& name, CharacteristicValueTypes::Value valueType, const QStringList& enumValues = {});


    /**
     * @brief Creates a new subject with the given parameters an insert it into the Cassandra DB
     * a nullptr is returned if the operation fails
     * @param experimentationUuid
     * @param name
     * @return
     */
    SubjectM* _insertSubjectIntoDB(CassUuid experimentationUuid, const QString& name);


    /**
     * @brief Insert a new value for the given subject and characteristic into the DB
     * A default value is written in DB according to the characterystic's type
     * @param subject
     * @param characteristic
     */
    void _insertCharacteristicValueForSubjectIntoDB(SubjectM* subject, CharacteristicM* characteristic);


    /**
     * @brief Delete every characteristic value associated with the given subject
     * @param subject
     */
    void _deleteCharacteristicValuesForSubject(SubjectM* subject);


    /**
     * @brief Delete evert characteris value assciated with the given characteristic
     * @param characteristic
     */
    void _deleteCharacteristicValuesForCharacteristic(CharacteristicM* characteristic);


private:

};

QML_DECLARE_TYPE(SubjectsController)

#endif // SUBJECTSCONTROLLER_H
