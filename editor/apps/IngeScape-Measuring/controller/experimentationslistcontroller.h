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

#ifndef EXPERIMENTATIONSLISTCONTROLLER_H
#define EXPERIMENTATIONSLISTCONTROLLER_H

#include <QObject>
#include <I2PropertyHelpers.h>

/**
 * @brief The ExperimentationsListController class defines the controller to manage the list of experimentations
 */
class ExperimentationsListController : public QObject
{
    Q_OBJECT


public:
    /**
     * @brief Constructor
     * @param parent
     */
    explicit ExperimentationsListController(QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~ExperimentationsListController();


Q_SIGNALS:


public Q_SLOTS:


private:

};

QML_DECLARE_TYPE(ExperimentationsListController)

#endif // EXPERIMENTATIONSLISTCONTROLLER_H
