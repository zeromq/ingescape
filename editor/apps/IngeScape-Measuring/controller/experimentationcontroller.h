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

#ifndef EXPERIMENTATIONCONTROLLER_H
#define EXPERIMENTATIONCONTROLLER_H

#include <QObject>
#include <I2PropertyHelpers.h>

/**
 * @brief The ExperimentationController class defines the controller to manage the current experimentation
 */
class ExperimentationController : public QObject
{
    Q_OBJECT


public:
    /**
     * @brief Constructor
     * @param parent
     */
    explicit ExperimentationController(QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~ExperimentationController();


Q_SIGNALS:


public Q_SLOTS:


private:

};

QML_DECLARE_TYPE(ExperimentationController)

#endif // EXPERIMENTATIONCONTROLLER_H
