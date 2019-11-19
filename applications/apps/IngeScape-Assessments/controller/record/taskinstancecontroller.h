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
#include <controller/abstractscenariocontroller.h>
#include <model/jsonhelper.h>
#include <model/taskinstancem.h>

#include <controller/assessmentsmodelmanager.h>


/**
 * @brief The TaskInstanceController class defines the controller to manage a record of the current experimentation
 */
class TaskInstanceController : public QObject
{
    Q_OBJECT

    // Controller to manage the time-line
    I2_QML_PROPERTY_READONLY(AbstractTimeActionslineScenarioViewController*, timeLineC)

    // Controller for scenario management
    I2_QML_PROPERTY_READONLY(AbstractScenarioController*, scenarioC)

    // The (experimentation) task instance currently selected
    I2_QML_PROPERTY_CUSTOM_SETTER(TaskInstanceM*, currentTaskInstance)

    // List off all agents present in the current platform
    I2_QOBJECT_SORTFILTERPROXY(AgentsGroupedByNameVM, allAgentsGroupedByNameInCurrentPlatform)

    //FIXME: remove temp property
    I2_QML_PROPERTY(AssessmentsModelManager*, temp)


public:

    /**
     * @brief Constructor
     * @param modelManager
     * @param jsonHelper
     * @param parent
     */
    explicit TaskInstanceController(JsonHelper* jsonHelper,
                              QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~TaskInstanceController();

    /**
     * @brief Adds the given URLs as attachements for this record
     * @param urlList
     */
    Q_INVOKABLE void addNewAttachements(const QList<QUrl>& urlList);


private Q_SLOTS:

    /**
     * @brief Slot called when the current task instance changed
     * @param previousTaskInstance
     * @param currentTaskInstance
     */
    void _oncurrentTaskInstanceChanged(TaskInstanceM* previousTaskInstance, TaskInstanceM* currentTaskInstance);


private:
    // Helper to manage JSON files
    JsonHelper* _jsonHelper;

};

QML_DECLARE_TYPE(TaskInstanceController)

#endif // RECORDCONTROLLER_H
