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

#ifndef EXPERIMENTATIONCONTROLLER_H
#define EXPERIMENTATIONCONTROLLER_H

#include <QObject>
#include <I2PropertyHelpers.h>
#include <controller/record/taskinstancecontroller.h>
#include <model/experimentationm.h>
#include <sortFilter/aresubjectandprotocolinsessionfilter.h>


/**
 * @brief The ExperimentationController class defines the controller to manage the current experimentation
 */
class ExperimentationController : public QObject
{
    Q_OBJECT

    // Controller to manage a task instance of the current experimentation
    I2_QML_PROPERTY_READONLY(TaskInstanceController*, taskInstanceC)

    // Model of the current experimentation
    I2_QML_PROPERTY_CUSTOM_SETTER(ExperimentationM*, currentExperimentation)

    // Peer id of the recorder
    I2_CPP_NOSIGNAL_PROPERTY(QString, peerIdOfRecorder)

    // Peer name of the recorder
    I2_CPP_NOSIGNAL_PROPERTY(QString, peerNameOfRecorder)

    // Flag indicating is there is a recorder with state ON
    I2_QML_PROPERTY_READONLY(bool, isRecorderON)

    // Flag indicating if the recorder is currently recording
    I2_QML_PROPERTY_READONLY(bool, isRecording)

    // List of selected sessions
    I2_QOBJECT_LISTMODEL(TaskInstanceM, selectedSessions)

    // List off all agents present in the current platform
    Q_PROPERTY(AreSubjectAndProtocolInSessionFilter* sessionFilteredList READ sessionFilteredList CONSTANT)

    // List with ids of selected subjects
    I2_QML_PROPERTY_READONLY(QStringList, selectedSubjectIdList)

    // List with names of selected Protocol
    I2_QML_PROPERTY_READONLY(QStringList, selectedProtocolNameList)

    // List with ids of subjects for current experimentation
    I2_QML_PROPERTY_READONLY(QStringList, subjectIdList)

    // List with names of Protocol for current experimentation
    I2_QML_PROPERTY_READONLY(QStringList, protocolNameList)


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


    /**
     * @brief Create a new session for a subject and a protocol
     * @param subject
     * @param protocol
     */
    Q_INVOKABLE void createNewSessionForSubjectAndProtocol(SubjectM* subject, TaskM* protocol);


    /**
     * @brief Open a session
     * @param session
     */
    Q_INVOKABLE void openSession(TaskInstanceM* session);


    /**
     * @brief Delete a task instance
     * @param record
     */
    Q_INVOKABLE void deleteTaskInstance(TaskInstanceM* taskInstance);


    /**
     * @brief Export the list of selected sessions
     */
    Q_INVOKABLE void exportSelectedSessions();


    /**
     * @brief Method called when the user wants to start to record
     */
    Q_INVOKABLE void startToRecord();


    /**
     * @brief Method called when the user wants to stop to record
     */
    Q_INVOKABLE void stopToRecord();

    /**
     * @brief Get our filtered list of values
     * @return
     */
    AreSubjectAndProtocolInSessionFilter* sessionFilteredList()
    {
        return &_sessionFilteredList;
    }

    /**
     * @brief Show the session for one subject
     * @param agentName
     */
    Q_INVOKABLE void showSessionForSubject(QString subjectId);

    /**
     * @brief hide the session for one subject
     * @param agentName
     */
    Q_INVOKABLE void hideSessionForSubject(QString subjectId);

    /**
     * @brief Show sessions for every subject
     * @param agentName
     */
    Q_INVOKABLE void showAllSessionsForSubject();

    /**
     * @brief hide sessions for every subject
     * @param agentName
     */
    Q_INVOKABLE void hideAllSessionsForSubject();

    /**
     * @brief Return true if the session is show for subject name
     * @param agentName
     * @return
     */
    Q_INVOKABLE bool areShownSessionsForSubject(QString subjectId);

    /**
     * @brief Show the session for one protocol
     * @param agentName
     */
    Q_INVOKABLE void showSessionForProtocol(QString protocolName);

    /**
     * @brief hide the session for one subject
     * @param agentName
     */
    Q_INVOKABLE void hideSessionForProtocol(QString protocolName);

    /**
     * @brief Show sessions for every subject
     * @param agentName
     */
    Q_INVOKABLE void showAllSessionsForProtocol();

    /**
     * @brief hide sessions for every subject
     * @param agentName
     */
    Q_INVOKABLE void hideAllSessionsForProtocol();

    /**
     * @brief Return true if the session is show for subject name
     * @param agentName
     * @return
     */
    Q_INVOKABLE bool areShownSessionsForProtocol(QString protocolName);

public Q_SLOTS:

    /**
     * @brief Slot called when a recorder enter the network
     * @param peerId
     * @param peerName
     * @param ipAddress
     * @param hostname
     */
    void onRecorderEntered(QString peerId, QString peerName, QString ipAddress, QString hostname);


    /**
     * @brief Slot called when a recorder quit the network
     * @param peerId
     * @param peerName
     */
    void onRecorderExited(QString peerId, QString peerName);


    /**
     * @brief Slot called when the recorder has started to record
     * @param state
     */
    void onRecordStartedReceived();

    /**
     * @brief Slot called when the recorder has stopped the record
     */
    void onRecordStoppedReceived();

    /**
     * @brief Slot called when a record is added
     * @param model
     */
    void onRecordAddedReceived(QString message);

private Q_SLOTS:

    /**
     * @brief Slot called when the current experimentation changed
     * @param currentExperimentation
     */
    void _onCurrentExperimentationChanged(ExperimentationM* currentExperimentation);


    /**
     * @brief Slot called just before an action is performed
     * (the message "EXECUTED ACTION" must be sent on the network to the recorder)
     * @param message
     */
    void _onActionWillBeExecuted(QString message);


    /**
     * @brief Slot called when the state of the TimeLine updated
     * @param state
     */
    void _onTimeLineStateUpdated(QString state);


protected: // Methods
    /**
     * @brief Create and insert a new task instance into the DB.
     * A nullptr is returned if the task instance could not be created
     * @param recordName
     * @param subject
     * @param task
     * @return
     */
    TaskInstanceM* _insertTaskInstanceIntoDB(const QString& taskInstanceName, SubjectM* subject, TaskM* task);

    /**
     * @brief Retrieve all independent variables from the Cassandra DB for the given task.
     * The task will be updated by this method.
     * @param task
     */
    void _retrieveIndependentVariableForTask(TaskM* task);

    /**
     * @brief Retrieve all dependent variables from the Cassandra DB for the given task.
     * The task will be updated by this method.
     * @param task
     */
    void _retrieveDependentVariableForTask(TaskM* task);

    /**
     * @brief Retrieve all subjects from the Cassandra DB for the given experimentaion.
     * The experimentation will be updated by this method
     * @param experimentation
     */
    void _retrieveSubjectsForExperimentation(ExperimentationM* experimentation);

    /**
     * @brief Retrieve all characteristics from the Cassandra DB for the given experimentaion.
     * The experimentation will be updated by this method
     * @param experimentation
     */
    void _retrieveCharacteristicsForExperimentation(ExperimentationM* experimentation);

    /**
     * @brief Retrieve all tasks from the Cassandra DB for the given experimentaion.
     * The experimentation will be updated by this method
     * @param experimentation
     */
    void _retrieveTasksForExperimentation(ExperimentationM* experimentation);

    /**
     * @brief Retrieve all task instances from the Cassandra DB for the given experimentaion.
     * The experimentation will be updated by this method
     * @param experimentation
     */
    void _retrieveTaskInstancesForExperimentation(ExperimentationM* experimentation);

    /**
     * @brief Retrieve all characteristic values from the Cassandra DB for each subjects in the given experimentation.
     * The subjects will be updated by this method
     * @param experimentation
     */
    void _retrieveCharacteristicValuesForSubjectsInExperimentation(ExperimentationM* experimentation);

    /**
     * @brief Retrieve all independent variable values Cassandra DB for each task instance in the given experimentation.
     * The experimentation will be updated by this method
     * @param experimentation
     */
    void _retrieveIndependentVariableValuesForTaskInstancesInExperimentation(ExperimentationM* experimentation);


protected:

private:

    /**
     * @brief Update the filters on the list of sessions
     */
    void _updateFilters();

private :
    //List of sessions filtered by subject id and protocol name
    AreSubjectAndProtocolInSessionFilter _sessionFilteredList;

};

QML_DECLARE_TYPE(ExperimentationController)

#endif // EXPERIMENTATIONCONTROLLER_H
