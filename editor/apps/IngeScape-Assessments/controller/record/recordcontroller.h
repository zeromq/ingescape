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

#ifndef RECORDCONTROLLER_H
#define RECORDCONTROLLER_H

#include <QObject>
#include <I2PropertyHelpers.h>
#include <controller/abstracttimeactionslinescenarioviewcontroller.h>
#include <controller/scenariocontroller.h>
#include <controller/assessmentsmodelmanager.h>
#include <model/jsonhelper.h>
#include <model/experimentationrecordm.h>


/**
 * @brief The RecordController class defines the controller to manage a record of the current experimentation
 */
class RecordController : public QObject
{
    Q_OBJECT

    // Controller to manage the time-line
    I2_QML_PROPERTY_READONLY(AbstractTimeActionslineScenarioViewController*, timeLineC)

    // Controller for scenario management
    I2_QML_PROPERTY_READONLY(ScenarioController*, scenarioC)

    // The (experimentation) record currently selected
    I2_QML_PROPERTY_CUSTOM_SETTER(ExperimentationRecordM*, currentRecord)

    // List of actions
    I2_QOBJECT_LISTMODEL(ActionM, actionsList)

    // Selected action
    I2_QML_PROPERTY_DELETE_PROOF(ActionM*, selectedAction)


public:

    /**
     * @brief Constructor
     * @param modelManager
     * @param jsonHelper
     * @param parent
     */
    explicit RecordController(AssessmentsModelManager* modelManager,
                              JsonHelper* jsonHelper,
                              QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~RecordController();


Q_SIGNALS:


private Q_SLOTS:

    /**
     * @brief Slot called when the current record changed
     * @param currentRecord
     * @param previousRecord
     */
    void _onCurrentRecordChanged(ExperimentationRecordM* previousRecord, ExperimentationRecordM* currentRecord);


private:

    // Manager for the data model of our IngeScape Assessments application
    AssessmentsModelManager* _modelManager;

    // Helper to manage JSON files
    JsonHelper* _jsonHelper;

};

QML_DECLARE_TYPE(RecordController)

#endif // RECORDCONTROLLER_H
