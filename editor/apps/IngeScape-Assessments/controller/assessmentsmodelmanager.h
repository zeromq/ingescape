/*
 *	IngeScape Assessments
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

#ifndef ASSESSMENTSMODELMANAGER_H
#define ASSESSMENTSMODELMANAGER_H

#include <QObject>
//#include <QtQml>
#include <I2PropertyHelpers.h>
#include <model/assessmentsenums.h>
#include <model/jsonhelper.h>
#include <model/experimentationm.h>
#include <viewModel/experimentationsgroupvm.h>


/**
 * @brief The AssessmentsModelManager class defines the manager for the data model of our IngeScape Assessments application
 */
class AssessmentsModelManager : public QObject
{
    Q_OBJECT

    // The experimentations group currently selected
    I2_QML_PROPERTY(ExperimentationsGroupVM*, currentExperimentationsGroup)

    // The experimentation currently selected
    I2_QML_PROPERTY(ExperimentationM*, currentExperimentation)


public:

    /**
     * @brief Constructor
     * @param jsonHelper
     * @param parent
     */
    explicit AssessmentsModelManager(JsonHelper* jsonHelper,
                                   QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~AssessmentsModelManager();


Q_SIGNALS:


public Q_SLOTS:


private:

    // Helper to manage JSON files
    JsonHelper* _jsonHelper;

};

QML_DECLARE_TYPE(AssessmentsModelManager)

#endif // ASSESSMENTSMODELMANAGER_H
