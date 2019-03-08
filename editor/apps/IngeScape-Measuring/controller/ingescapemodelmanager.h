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

#ifndef INGESCAPEMODELMANAGER_H
#define INGESCAPEMODELMANAGER_H

#include <QObject>
#include <I2PropertyHelpers.h>

#include <model/measuringenums.h>
#include <model/experimentationm.h>
#include <model/subject/subjectm.h>
#include <model/subject/characteristicm.h>
#include <model/subject/characteristicenumm.h>
#include <model/task/taskm.h>
#include <model/task/independentvariablem.h>
#include <model/task/independentvariableenumm.h>
#include <model/task/dependentvariablem.h>
#include <model/recordm.h>

#include <viewModel/experimentationsgroupvm.h>


/**
 * @brief The IngeScapeModelManager class defines the manager for the data model of our IngeScape measuring application
 */
class IngeScapeModelManager : public QObject
{
    Q_OBJECT

    // The experimentations group currently selected
    I2_QML_PROPERTY(ExperimentationsGroupVM*, selectedExperimentationsGroup)

    // The experimentation currently selected
    I2_QML_PROPERTY(ExperimentationM*, selectedExperimentation)




public:
    /**
     * @brief Constructor
     * @param parent
     */
    explicit IngeScapeModelManager(QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~IngeScapeModelManager();


Q_SIGNALS:


public Q_SLOTS:


private:


};

QML_DECLARE_TYPE(IngeScapeModelManager)

#endif // INGESCAPEMODELMANAGER_H
