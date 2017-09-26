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

#ifndef AGENTSMAPPINGCONTROLLER_H
#define AGENTSMAPPINGCONTROLLER_H

#include <QObject>
#include <I2PropertyHelpers.h>

#include <controller/masticmodelmanager.h>

/**
 * @brief The AgentsMappingController class defines the controller for agents mapping
 */
class AgentsMappingController : public QObject
{
    Q_OBJECT

public:
    explicit AgentsMappingController(MasticModelManager* modelManager, QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~AgentsMappingController();

signals:

public slots:

private:
    // Usefull to save it
    MasticModelManager* _modelManager;
};

QML_DECLARE_TYPE(AgentsMappingController)

#endif // AGENTSMAPPINGCONTROLLER_H
