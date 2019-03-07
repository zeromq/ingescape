/*
 *	IngeScape Measuring
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

#ifndef TASKSCONTROLLER_H
#define TASKSCONTROLLER_H

#include <QObject>
#include <I2PropertyHelpers.h>


/**
 * @brief The TasksController class defines the controller to manage the tasks of the current experimentation
 */
class TasksController : public QObject
{
    Q_OBJECT


public:

    /**
     * @brief Constructor
     * @param parent
     */
    explicit TasksController(QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~TasksController();


Q_SIGNALS:


public Q_SLOTS:


private:

};

QML_DECLARE_TYPE(TasksController)

#endif // TASKSCONTROLLER_H
