/*
 *	MASTIC Editor
 *
 *  Copyright © 2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

#ifndef AGENTSSUPERVISIONCONTROLLER_H
#define AGENTSSUPERVISIONCONTROLLER_H

#include <QObject>
#include <I2PropertyHelpers.h>

#include <controller/masticmodelmanager.h>


/**
 * @brief The AgentsSupervisionController class defines the controller for agents supervision
 */
class AgentsSupervisionController : public QObject
{
    Q_OBJECT

    //I2_QOBJECT_LISTMODEL_WITH_SORTFILTERPROXY(AgentVM, agentsList)
    //I2_QOBJECT_SORTFILTERPROXY(AgentVM, agentsList)

public:
    explicit AgentsSupervisionController(MasticModelManager* modelManager, QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~AgentsSupervisionController();

    /**
     * @brief Delete an agent VM from the list
     * @param agent VM to delete
     */
    Q_INVOKABLE void deleteAgentFromList(AgentVM * agentToDelete);

signals:

public slots:

private:
    // Usefull to save it
    MasticModelManager* _modelManager;
};

QML_DECLARE_TYPE(AgentsSupervisionController)

#endif // AGENTSSUPERVISIONCONTROLLER_H
