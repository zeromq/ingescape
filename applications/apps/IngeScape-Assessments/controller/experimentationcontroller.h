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
 *      Chloé Roumieu      <roumieu@ingenuity.io>
 */

#ifndef EXPERIMENTATIONCONTROLLER_H
#define EXPERIMENTATIONCONTROLLER_H

#include <QObject>
#include <I2PropertyHelpers.h>
#include <controller/session/sessioncontroller.h>
#include <model/experimentationm.h>
#include <sortFilter/aresubjectandprotocolinsessionfilter.h>


/**
 * @brief The ExperimentationController class defines the controller to manage the current experimentation
 */
class ExperimentationController : public QObject
{
    Q_OBJECT

    I2_QML_PROPERTY_READONLY(SessionController*, sessionC)
    I2_QML_PROPERTY_CUSTOM_SETTER(ExperimentationM*, currentExperimentation)

    I2_CPP_NOSIGNAL_PROPERTY(QString, peerIdOfRecorder)
    I2_CPP_NOSIGNAL_PROPERTY(QString, peerNameOfRecorder)
    I2_QML_PROPERTY_READONLY(bool, isRecorderON)
    I2_QML_PROPERTY_READONLY(bool, isRecording)

    I2_QML_PROPERTY(bool, isSelectingSessions) // Mode "export sessions" in qml
    I2_QOBJECT_LISTMODEL(SessionM, selectedSessions)

    Q_PROPERTY(AreSubjectAndProtocolInSessionFilter* sessionFilteredList READ sessionFilteredList CONSTANT) // List of all agents present in the current platform
    I2_QML_PROPERTY_READONLY(QStringList, selectedSubjectIdListToFilter)
    I2_QML_PROPERTY_READONLY(QStringList, selectedProtocolNameListToFilter)

    I2_QOBJECT_LISTMODEL_WITH_SORTFILTERPROXY(RecordAssessmentM, listFuturRecordsToHandle) // at each timeout of our timer
    I2_QML_PROPERTY(RecordAssessmentM*, nextRecordToHandle)
    I2_QML_PROPERTY(bool, removeOtherRecordsWhileRecording) // If false, it means that recording have to stop at first other record encounter


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
    Q_INVOKABLE void createNewSessionForSubjectAndProtocol(SubjectM* subject, ProtocolM* protocol);


    /**
     * @brief Open a session
     * @param session
     */
    Q_INVOKABLE void openSession(SessionM* session);


    /**
     * @brief Delete a session
     * @param session
     */
    Q_INVOKABLE void deleteSession(SessionM* session);


    /**
     * @brief Export the list of selected sessions
     */
    Q_INVOKABLE void exportSelectedSessions();


    /**
     * @brief Return true if start time in our timeline is before one (or more) record(s)
     * of our current session, false if not
     * @return
     */
    Q_INVOKABLE bool isThereOneRecordAfterStartTime();


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
     * @brief Filter sessions list with one more subject
     * @param subjectId
     */
    Q_INVOKABLE void addOneSubjectToFilterSessions(QString subjectId);


    /**
     * @brief Filter sessions list without one more subject
     * @param subjectId
     */
    Q_INVOKABLE void removeOneSubjectToFilterSessions(QString subjectId);

    
    /**
     * @brief Filter sessions list with all subjects
     */
    Q_INVOKABLE void addAllSubjectsToFilterSessions();

    
    /**
     * @brief Filter sessions list without any subject
     */
    Q_INVOKABLE void removeAllSubjectsToFilterSessions();

    
    /**
     * @brief Return true if the subject id is used to filter sessions list
     * @param subjectId
     * @return
     */
    Q_INVOKABLE bool isSubjectFilterSessions(QString subjectId);

    
    /**
     * @brief Filter sessions list with one more protocol
     * @param protocolName
     */
    Q_INVOKABLE void addOneProtocolToFilterSessions(QString protocolName);

    
    /**
     * @brief Filter sessions list without one more protocol
     * @param protocolName
     */
    Q_INVOKABLE void removeOneProtocolToFilterSessions(QString protocolName);


    /**
     * @brief Filter sessions list with all protocols
     */
    Q_INVOKABLE void addAllProtocolsToFilterSessions();


    /**
     * @brief Filter sessions list without any protocols
     */
    Q_INVOKABLE void removeAllProtocolsToFilterSessions();


    /**
     * @brief Return true if the session is show for subject name
     * @param agentName
     * @return
     */
    Q_INVOKABLE bool isProtocolFilterSessions(QString protocolName);


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


    /**
     * @brief Slot called when a record is deleted
     * @param message (id of the record)
     */
    void onRecordDeletedReceived(QString message);


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


    /**
     * @brief Called when our timer time out to handle existing records
     */
    void _onTimeout_EncounterExistingRecords();



protected: // Methods
    /**
     * @brief Create and insert a new session into the DB.
     * A nullptr is returned if the session could not be created
     * @param sessionName
     * @param subject
     * @param protocol
     * @return
     */
    SessionM* _insertSessionIntoDB(const QString& sessionName, SubjectM* subject, ProtocolM* protocol);

    /**
     * @brief Retrieve all independent variables from the Cassandra DB for the given protocol.
     * The protocol will be updated by this method.
     * @param protocol
     */
    void _retrieveIndependentVariableForProtocol(ProtocolM* protocol);

    /**
     * @brief Retrieve all dependent variables from the Cassandra DB for the given protocol.
     * The protocol will be updated by this method.
     * @param protocol
     */
    void _retrieveDependentVariableForProtocol(ProtocolM* protocol);

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
     * @brief Retrieve all protocols from the Cassandra DB for the given experimentaion.
     * The experimentation will be updated by this method
     * @param experimentation
     */
    void _retrieveProtocolsForExperimentation(ExperimentationM* experimentation);

    /**
     * @brief Retrieve all sessions from the Cassandra DB for the given experimentaion.
     * The experimentation will be updated by this method
     * @param experimentation
     */
    void _retrieveSessionsForExperimentation(ExperimentationM* experimentation);

    /**
     * @brief Retrieve all characteristic values from the Cassandra DB for each subjects in the given experimentation.
     * The subjects will be updated by this method
     * @param experimentation
     */
    void _retrieveCharacteristicValuesForSubjectsInExperimentation(ExperimentationM* experimentation);

    /**
     * @brief Retrieve all independent variable values Cassandra DB for each session in the given experimentation.
     * The experimentation will be updated by this method
     * @param experimentation
     */
    void _retrieveIndependentVariableValuesForSessionsInExperimentation(ExperimentationM* experimentation);


protected:

    // Timer to handle the scenario and execute actions
    QTimer _timerToHandleExistingRecords;


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
