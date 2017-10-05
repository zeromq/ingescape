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

#ifndef AGENTMAPPINGM_H
#define AGENTMAPPINGM_H

#include <QObject>
#include <QtQml>

#include <I2PropertyHelpers.h>

/**
 * @brief The AgentMappingM class defines a model of agent mapping
 */
class AgentMappingM : public QObject
{
    Q_OBJECT

public:
    explicit AgentMappingM(QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~AgentMappingM();


Q_SIGNALS:

public Q_SLOTS:
};

QML_DECLARE_TYPE(AgentMappingM)

#endif // AGENTMAPPINGM_H
