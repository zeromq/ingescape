/*
 *	IngeScape Measuring
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

#ifndef EXPERIMENTATIONCONTROLLER_H
#define EXPERIMENTATIONCONTROLLER_H

#include <QObject>
#include <I2PropertyHelpers.h>

#include <controller/ingescapemodelmanager.h>


/**
 * @brief The ExperimentationController class defines the controller to manage the current experimentation
 */
class ExperimentationController : public QObject
{
    Q_OBJECT


public:

    /**
     * @brief Constructor
     * @param modelManager
     * @param parent
     */
    explicit ExperimentationController(IngeScapeModelManager* modelManager,
                                       QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~ExperimentationController();


    /**
     * @brief Create a new record for a subject and a task
     * @param recordName
     * @param subject
     * @param task
     */
    Q_INVOKABLE void createNewRecordForSubjectAndTask(QString recordName, SubjectM* subject, TaskM* task);


    /**
     * @brief Open a record
     * @param record
     */
    Q_INVOKABLE void openRecord(RecordM* record);


    /**
     * @brief Delete a record
     * @param record
     */
    Q_INVOKABLE void deleteRecord(RecordM* record);


Q_SIGNALS:


public Q_SLOTS:

    /**
     * @brief Slot called when the current experimentation changed
     * @param currentExperimentation
     */
    void onCurrentExperimentationChanged(ExperimentationM* currentExperimentation);


private:

    // Manager for the data model of our IngeScape measuring application
    IngeScapeModelManager* _modelManager;

};

QML_DECLARE_TYPE(ExperimentationController)

#endif // EXPERIMENTATIONCONTROLLER_H
