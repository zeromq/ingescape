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

#include <controller/ingescapemodelmanager.h>
#include <controller/ingescapenetworkcontroller.h>
#include <controller/abstracttimeactionslinescenarioviewcontroller.h>
#include <controller/abstractscenariocontroller.h>
#include <model/sessionm.h>
#include <sortFilter/areagentsinplatformfilter.h>


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

    // The session currently selected
    I2_QML_PROPERTY_CUSTOM_SETTER(SessionM*, currentSession)

    // List off all agents present in the current platform
    Q_PROPERTY(AreAgentsInPlatformFilter* agentsGroupedByNameInCurrentPlatform READ agentsGroupedByNameInCurrentPlatform CONSTANT)


public:

    /**
     * @brief Constructor
     * @param parent
     */
    explicit TaskInstanceController(QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~TaskInstanceController();


    /**
     * @brief Adds the given URLs as attachements for this record
     * @param urlList
     */
    Q_INVOKABLE void addNewAttachements(const QList<QUrl>& urlList);


    AreAgentsInPlatformFilter* agentsGroupedByNameInCurrentPlatform()
    {
        return &_agentsGroupedByNameInCurrentPlatform;
    }


private Q_SLOTS:

    /**
     * @brief Slot called when the current session changed
     * @param previousSession
     * @param currentSession
     */
    void _oncurrentSessionChanged(SessionM* previousSession, SessionM* currentSession);


    /**
     * @brief Slot called when a model of agent "ON" has been added to an agent(s grouped by name)
     * @param model
     */
    void _onAgentModelONhasBeenAdded(AgentM* model);


private:

    /**
     * @brief Import the global mapping (of agents) from JSON
     * @param jsonArrayOfAgentsInMapping
     */
    void _importMappingFromJson(QJsonArray jsonArrayOfAgentsInMapping);


private:

    // Filtered list with all present agents
    AreAgentsInPlatformFilter _agentsGroupedByNameInCurrentPlatform;

};

QML_DECLARE_TYPE(TaskInstanceController)

#endif // RECORDCONTROLLER_H
