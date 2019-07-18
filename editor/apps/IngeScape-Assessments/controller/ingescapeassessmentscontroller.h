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

#ifndef INGESCAPEASSESSMENTSCONTROLLER_H
#define INGESCAPEASSESSMENTSCONTROLLER_H

#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>

#include <I2PropertyHelpers.h>

#include <controller/networkcontroller.h>
#include <controller/experimentationslistcontroller.h>
#include <controller/experimentationcontroller.h>
#include <controller/subject/subjectscontroller.h>
#include <controller/task/taskscontroller.h>

#include <misc/terminationsignalwatcher.h>


/**
 * @brief The IngeScapeAssessmentsController class defines the main controller of the IngeScape Assessments application
 */
class IngeScapeAssessmentsController : public QObject
{
    Q_OBJECT

    // Network settings - network device
    I2_QML_PROPERTY_READONLY(QString, networkDevice)

    // Network settings - ip address
    I2_QML_PROPERTY_READONLY(QString, ipAddress)

    // Network settings - port
    I2_QML_PROPERTY_READONLY(uint, port)

    // Error message when a connection attempt fails
    I2_QML_PROPERTY_READONLY(QString, errorMessageWhenConnectionFailed)

    // Snapshot Directory
    I2_QML_PROPERTY_READONLY(QString, snapshotDirectory)

    // Controller for network communication
    I2_QML_PROPERTY_READONLY(NetworkController*, networkC)

    // Controller to manage the list of experimentations
    I2_QML_PROPERTY_READONLY(ExperimentationsListController*, experimentationsListC)

    // Controller to manage the current experimentation
    I2_QML_PROPERTY_READONLY(ExperimentationController*, experimentationC)

    // Controller to manage the subjects of the current experimentation
    I2_QML_PROPERTY_READONLY(SubjectsController*, subjectsC)

    // Controller to manage the tasks of the current experimentation
    I2_QML_PROPERTY_READONLY(TasksController*, tasksC)


public:
    /**
     * @brief Constructor
     * @param parent
     */
    explicit IngeScapeAssessmentsController(QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~IngeScapeAssessmentsController();


    /**
     * @brief Method used to provide a singleton to QML
     * @param engine
     * @param scriptEngine
     * @return
     */
    static QObject* qmlSingleton(QQmlEngine* engine, QJSEngine* scriptEngine);


    /**
      * @brief Actions to perform before the application closing
      */
    Q_INVOKABLE void processBeforeClosing();


public Q_SLOTS:

    /**
      * @brief Method used to force the creation of our singleton from QML
      */
    void forceCreation();


private Q_SLOTS:

    /**
     * @brief Slot called when the current experimentation changed
     * @param value
     */
    void _onCurrentExperimentationChanged(ExperimentationM* currentExperimentation);


private:

    // To subscribe to termination signals
    TerminationSignalWatcher *_terminationSignalWatcher;

    // Helper to manage JSON files
    JsonHelper* _jsonHelper;

    // Path to the directory containing JSON files about platforms
    //QString _platformDirectoryPath;

};

QML_DECLARE_TYPE(IngeScapeAssessmentsController)

#endif // INGESCAPEASSESSMENTSCONTROLLER_H
