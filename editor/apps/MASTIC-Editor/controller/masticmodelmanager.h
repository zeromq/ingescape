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

#ifndef MASTICMODELMANAGER_H
#define MASTICMODELMANAGER_H

#include <QObject>
#include <I2PropertyHelpers.h>

#include <model/jsonhelper.h>
#include <viewModel/agentvm.h>


/**
 * @brief The MasticModelManager class defines a manager for the data model of MASTIC
 */
class MasticModelManager : public QObject
{
    Q_OBJECT

    // List of all models of agents
    //I2_QOBJECT_LISTMODEL(AgentM, allAgents)

    // List of all view model of agents
    I2_QOBJECT_LISTMODEL(AgentVM, allAgentsVM)


public:
    /**
     * @brief Default constructor
     * @param parent
     */
    explicit MasticModelManager(QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~MasticModelManager();

Q_SIGNALS:

public Q_SLOTS:

private:
    // Helper to manage JSON definitions of agents
    JsonHelper* _jsonHelper;

    // List of all models of agents
    QList<AgentM*> _allAgentsModel;

};

QML_DECLARE_TYPE(MasticModelManager)

#endif // MASTICMODELMANAGER_H
