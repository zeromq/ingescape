
/*
 *	ActionEditorController
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

#ifndef ACTIONEDITORCONTROLLER_H
#define ACTIONEDITORCONTROLLER_H

#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>

#include "I2PropertyHelpers.h"
#include "model/scenario/actionm.h"
#include "viewModel/agentinmappingvm.h"
#include "viewModel/actionconditionvm.h"
#include "viewModel/actioneffectvm.h"


/**
 * @brief The ActionEditorController class defines the main controller of our application
 */
class ActionEditorController: public QObject
{
    Q_OBJECT

    // Original action view model
    I2_QML_PROPERTY(ActionM*, originalAction)

    // Temporary action view model
    I2_QML_PROPERTY(ActionM*, editedAction)

    // Agents in mapping list
    I2_QML_PROPERTY(I2CustomItemListModel<AgentInMappingVM> *, listAgentsInMapping)

public:

    /**
     * @brief Default constructor
     * @param action name
     * @param original action
     * @param parent
     */
    explicit ActionEditorController(QString actionName, ActionM * originalAction, I2CustomItemListModel<AgentInMappingVM> * listAgentsInMapping, QObject *parent = 0);


    /**
      * @brief Destructor
      */
    ~ActionEditorController();

    /**
     * @brief Valide the edition/creation
     */
    void validateModification();

    /**
     * @brief Create a new condition
     */
    Q_INVOKABLE void createNewCondition();

    /**
     * @brief Remove the condition
     */
    Q_INVOKABLE void removeCondition(ActionConditionVM* conditionVM);

    /**
     * @brief Create a new effect
     */
    Q_INVOKABLE void createNewEffect();

    /**
     * @brief Remove the effect
     */
    Q_INVOKABLE void removeEffect(ActionEffectVM* effectVM);


Q_SIGNALS:


public Q_SLOTS:


protected:



};

QML_DECLARE_TYPE(ActionEditorController)

#endif // ACTIONEDITORCONTROLLER_H
