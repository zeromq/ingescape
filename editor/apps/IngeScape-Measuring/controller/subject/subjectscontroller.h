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

#ifndef SUBJECTSCONTROLLER_H
#define SUBJECTSCONTROLLER_H

#include <QObject>
#include <I2PropertyHelpers.h>


/**
 * @brief The SubjectsController class defines the controller to manage the subjects of the current experimentation
 */
class SubjectsController : public QObject
{
    Q_OBJECT


public:

    /**
     * @brief Constructor
     * @param parent
     */
    explicit SubjectsController(QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~SubjectsController();


Q_SIGNALS:


public Q_SLOTS:


private:

};

QML_DECLARE_TYPE(SubjectsController)

#endif // SUBJECTSCONTROLLER_H
