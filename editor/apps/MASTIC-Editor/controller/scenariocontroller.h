
/*
 *	ScenarioController
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

#ifndef SCENARIOCONTROLLER_H
#define SCENARIOCONTROLLER_H

#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>

#include "I2PropertyHelpers.h"
#include "viewModel/actionvm.h"
#include "controller/actioneditorcontroller.h"


/**
 * @brief The ScenarioController class defines the main controller of our application
 */
class ScenarioController: public QObject
{
    Q_OBJECT

    // Sorted list of ations by start time
    I2_QOBJECT_LISTMODEL_WITH_SORTFILTERPROXY(ActionVM, actionsList)

    // Sorted list of ations by start time
    I2_QOBJECT_LISTMODEL(ActionEditorController, openedActionsEditorsControllers)

public:

    /**
     * @brief Default constructor
     * @param parent
     */
    explicit ScenarioController(QObject *parent = 0);


    /**
      * @brief Destructor
      */
    ~ScenarioController();

    /**
      * @brief Open the action editor
      * @param action view model
      */
    Q_INVOKABLE void openActionEditor(ActionVM* actionVM);

    /**
      * @brief Delete an action from the list
      * @param action view model
      */
    Q_INVOKABLE void deleteAction(ActionVM * actionVM);

    /**
      * @brief Valide action edition
      * @param action editor controller
      */
    Q_INVOKABLE void valideActionEditor(ActionEditorController* actionEditorC);

    /**
      * @brief Close action edition
      * @param action editor controller
      */
    Q_INVOKABLE void closeActionEditor(ActionEditorController* actionEditorC);

    /**
      * @brief Delete action edition
      * @param action editor controller
      */
    Q_INVOKABLE void deleteActionEditor(ActionEditorController* actionEditorC);

Q_SIGNALS:


public Q_SLOTS:


protected:

    QHash<ActionVM*, ActionEditorController*> _mapActionsEditorControllersFromActionVM;

};

QML_DECLARE_TYPE(ScenarioController)

#endif // SCENARIOCONTROLLER_H
