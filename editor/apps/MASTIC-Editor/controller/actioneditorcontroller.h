
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
#include "viewModel/actionvm.h"



/**
 * @brief The ActionEditorController class defines the main controller of our application
 */
class ActionEditorController: public QObject
{
    Q_OBJECT

    // Original action view model
    I2_QML_PROPERTY(ActionVM*, originalAction)

    // Temporary action view model
    I2_QML_PROPERTY(ActionVM*, editedAction)

public:

    /**
     * @brief Default constructor
     * @param original action
     * @param parent
     */
    explicit ActionEditorController(ActionVM * originalAction, QObject *parent = 0);


    /**
      * @brief Destructor
      */
    ~ActionEditorController();

    /**
     * @brief Valide the edition/creation
     */
    void validateModification();


Q_SIGNALS:


public Q_SLOTS:


protected:



};

QML_DECLARE_TYPE(ActionEditorController)

#endif // ACTIONEDITORCONTROLLER_H
