/*
 *	ActionEffectM
 *
 *  Copyright (c) 2016-2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *
 */

#ifndef ACTIONEFFECTM_H
#define ACTIONEFFECTM_H

#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>

#include "I2PropertyHelpers.h"
#include <viewModel/agentsgroupedbynamevm.h>


/**
 * @brief The ActionEffectM class defines an action effect model
 */
class ActionEffectM : public QObject
{
    Q_OBJECT

    // View model of agents grouped by name
    I2_QML_PROPERTY_CUSTOM_SETTER(AgentsGroupedByNameVM*, agent)


public:

    /**
     * @brief Default constructor
     * @param parent
     */
    explicit ActionEffectM(QObject *parent = 0);


    /**
      * @brief Destructor
      */
    virtual ~ActionEffectM();


    /**
    * @brief Copy from another effect model
    * @param effct to copy
    */
    virtual void copyFrom(ActionEffectM* effect);


    /**
     * @brief Get a pair with the agent and the command (with parameters) of our effect
     * @return
     */
    virtual QPair<AgentsGroupedByNameVM*, QStringList> getAgentAndCommandWithParameters() = 0;


    /**
     * @brief Get a pair with the agent name and the reverse command (with parameters) of our effect
     * @return
     */
    virtual QPair<QString, QStringList> getAgentNameAndReverseCommandWithParameters() = 0;


Q_SIGNALS:

    /**
     * @brief Signal the parent that the action effect can be destroyed and removed from the list
     */
    void askForDestruction();


public Q_SLOTS:

    /**
     * @brief FIXME custom event instead: Called when our agent is destroyed
     * @param sender
     */
    void _onAgentDestroyed(QObject* sender);

};

QML_DECLARE_TYPE(ActionEffectM)

#endif // ACTIONEFFECTM_H
