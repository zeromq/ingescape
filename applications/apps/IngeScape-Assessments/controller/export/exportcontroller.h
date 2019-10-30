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

#ifndef EXPORTCONTROLLER_H
#define EXPORTCONTROLLER_H

#include <QObject>
#include <I2PropertyHelpers.h>

#include <model/experimentationm.h>

/**
 * @brief The ExportController class defines the controller to export data from the data base
 */
class ExportController : public QObject
{
    Q_OBJECT

    // Model of the current experimentation
    I2_QML_PROPERTY_READONLY(ExperimentationM*, currentExperimentation)


public:
    /**
     * @brief Constructor
     * @param parent
     */
    explicit ExportController(QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~ExportController();


    /**
     * @brief Export the current experimentation
     */
    Q_INVOKABLE void exportExperimentation();

    /**
     * @brief Export the current experimentation function test
     */
    Q_INVOKABLE void exportExperimentationTest(QString uuidIdExp);


private:

    // Path to the directory containing CSV files about exports
    QString _exportsDirectoryPath;

};

QML_DECLARE_TYPE(ExportController)

#endif // EXPORTCONTROLLER_H
