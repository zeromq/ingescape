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

#include <model/measuringenums.h>
#include <controller/ingescapemodelmanager.h>


/**
 * @brief The SubjectsController class defines the controller to manage the subjects of the current experimentation
 */
class SubjectsController : public QObject
{
    Q_OBJECT

    // List of all types for characteristic value
    I2_ENUM_LISTMODEL(CharacteristicValueTypes, allCharacteristicValueTypes)


public:

    /**
     * @brief Constructor
     * @param modelManager
     * @param parent
     */
    explicit SubjectsController(IngeScapeModelManager* modelManager,
                                QObject *parent = nullptr);


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
    Q_INVOKABLE void createNewCharacteristic(QString characteristicName, int nCharacteristicValueType);


    /**
     * @brief Create a new characteristic of type enum
     * @param characteristicName
     * @param enumValues
     */
    Q_INVOKABLE void createNewCharacteristicEnum(QString characteristicName, QStringList enumValues);


    /**
     * @brief Delete a characteristic
     * @param characteristic
     */
    Q_INVOKABLE void deleteCharacteristic(CharacteristicM* characteristic);


    /**
     * @brief Create a new subject
     */
    Q_INVOKABLE void createNewSubject();


    /**
     * @brief Delete a subject
     * @param subject
     */
    Q_INVOKABLE void deleteSubject(SubjectM* subject);


Q_SIGNALS:


public Q_SLOTS:


private:

    // Manager for the data model of our IngeScape Assessments application
    IngeScapeModelManager* _modelManager;

};

QML_DECLARE_TYPE(SubjectsController)

#endif // SUBJECTSCONTROLLER_H
