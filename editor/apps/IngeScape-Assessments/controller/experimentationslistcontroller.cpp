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

#include "experimentationslistcontroller.h"

/**
 * @brief Constructor
 * @param parent
 */
ExperimentationsListController::ExperimentationsListController(AssessmentsModelManager* modelManager,
                                                               QObject *parent) : QObject(parent),
    _defaultGroupOther(nullptr),
    _newGroup(nullptr),
    _modelManager(modelManager)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Experimentations List Controller";

    // Groups are sorted on their name (alphabetical order)
    _allExperimentationsGroups.setSortProperty("name");


    //
    // Create the default group "Other"
    //
    _defaultGroupOther = new ExperimentationsGroupVM("Others", nullptr);

    _allExperimentationsGroups.append(_defaultGroupOther);

    _hashFromNameToExperimentationsGroup.insert(_defaultGroupOther->name(), _defaultGroupOther);


    // Create the (fake) group "New"
    _newGroup = new ExperimentationsGroupVM(tr("New Group"), nullptr);


    //
    // FIXME for tests
    //
    /*int nbExpes = 1;

    for (int i = 0; i < 3; i++)
    {
        QString groupName = QString("Group for test %1").arg(i + 1);

        ExperimentationsGroupVM* group = new ExperimentationsGroupVM(groupName, nullptr);

        _allExperimentationsGroups.append(group);

        for (int j = 0; j < 2; j++)
        {
            QString expeName = QString("Expe for test %1").arg(nbExpes);

            ExperimentationM* experimentation = new ExperimentationM(expeName, QDateTime::currentDateTime(), nullptr);

            group->experimentations()->append(experimentation);

            nbExpes++;
        }
    }*/


    // Create the query
    QString query = QString("SELECT * FROM ingescape.experimentation;");

    // Creates the new query statement
    CassStatement* cassStatement = cass_statement_new(query.toStdString().c_str(), 0);

    // Execute the query or bound statement
    CassFuture* cassFuture = cass_session_execute(_modelManager->getCassSession(), cassStatement);

    CassError cassError = cass_future_error_code(cassFuture);
    if (cassError == CASS_OK)
    {
        qDebug() << "Get all experimentations succeeded";

        // Retrieve result set and iterate over the rows
        const CassResult* cassResult = cass_future_get_result(cassFuture);

        if (cassResult != nullptr)
        {
            CassIterator* cassIterator = cass_iterator_from_result(cassResult);

            while(cass_iterator_next(cassIterator))
            {
                const CassRow* row = cass_iterator_get_row(cassIterator);

                CassUuid experimentationUid;
                cass_value_get_uuid(cass_row_get_column_by_name(row, "id"), &experimentationUid);
                char chrExperimentationUid[CASS_UUID_STRING_LENGTH];
                cass_uuid_string(experimentationUid, chrExperimentationUid);

                const char *chrExperimentationName = "";
                size_t nameLength;
                cass_value_get_string(cass_row_get_column_by_name(row, "name"), &chrExperimentationName, &nameLength);
                QString experimentationName = QString::fromUtf8(chrExperimentationName, static_cast<int>(nameLength));

                cass_uint32_t creationDate;
                cass_value_get_uint32(cass_row_get_column_by_name(row, "creation_date"), &creationDate);

                cass_int64_t creationTime;
                cass_value_get_int64(cass_row_get_column_by_name(row, "creation_time"), &creationTime);

                time_t secCreationDateTime = cass_date_time_to_epoch(creationDate, creationTime);
                QDateTime creationDateTime;
                creationDateTime.setSecsSinceEpoch(secCreationDateTime);

                const char *chrExperimentationsGroupName = "";
                size_t experimentationsGroupNameLength;
                cass_value_get_string(cass_row_get_column_by_name(row, "group_name"), &chrExperimentationsGroupName, &experimentationsGroupNameLength);
                QString experimentationsGroupName = QString::fromUtf8(chrExperimentationsGroupName, static_cast<int>(experimentationsGroupNameLength));

                // Create the new experimentation
                ExperimentationM* experimentation = new ExperimentationM(experimentationUid, experimentationName, creationDateTime, nullptr);

                ExperimentationsGroupVM* experimentationsGroup = _getExperimentationsGroupFromName(experimentationsGroupName);
                if (experimentationsGroup == nullptr)
                {
                    // FIXME TODO: create the group but not create the expe, just add
                    //createNewExperimentationInNewGroup(experimentationName, experimentationsGroupName);
                }

                if (experimentationsGroup != nullptr)
                {
                    // Add to the group
                    experimentationsGroup->experimentations()->append(experimentation);
                }
            }

            cass_iterator_free(cassIterator);
        }
    }
    else {
        qCritical() << "Could not get all experimentations from the DataBase:" << cass_error_desc(cassError);
    }

    cass_statement_free(cassStatement);
    cass_future_free(cassFuture);
}


/**
 * @brief Destructor
 */
ExperimentationsListController::~ExperimentationsListController()
{
    qInfo() << "Delete Experimentations List Controller";

    _allExperimentationsGroups.clear();
    //_allExperimentationsGroups.deleteAllItems();

    if (_defaultGroupOther != nullptr)
    {
        ExperimentationsGroupVM* temp = _defaultGroupOther;
        setdefaultGroupOther(nullptr);
        delete temp;
    }

    if (_newGroup != nullptr)
    {
        ExperimentationsGroupVM* temp = _newGroup;
        setnewGroup(nullptr);
        delete temp;
    }

    if (_modelManager != nullptr)
    {
        _modelManager = nullptr;
    }
}


/**
 * @brief Create a new experimentation in new group (to create)
 * @param experimentationName
 * @param newExperimentationsGroupName
 */
void ExperimentationsListController::createNewExperimentationInNewGroup(QString experimentationName, QString newExperimentationsGroupName)
{
    if (!experimentationName.isEmpty() && !newExperimentationsGroupName.isEmpty())
    {
        ExperimentationsGroupVM* experimentationsGroup = _getExperimentationsGroupFromName(newExperimentationsGroupName);
        if (experimentationsGroup == nullptr)
        {
            // Create a new experimentations group
            experimentationsGroup = new ExperimentationsGroupVM(newExperimentationsGroupName, nullptr);

            _allExperimentationsGroups.append(experimentationsGroup);

            _hashFromNameToExperimentationsGroup.insert(newExperimentationsGroupName, experimentationsGroup);

            // Create a new experimentation in this new group
            createNewExperimentationInGroup(experimentationName, experimentationsGroup);
        }
    }
}


/**
 * @brief Create a new experimentation in a group
 * @param experimentationName
 * @param experimentationsGroup
 */
void ExperimentationsListController::createNewExperimentationInGroup(QString experimentationName, ExperimentationsGroupVM* experimentationsGroup)
{
    if (!experimentationName.isEmpty() && (experimentationsGroup != nullptr)
            && (_modelManager != nullptr))
    {
        CassUuid experimentationUid;
        cass_uuid_gen_time(_modelManager->getCassUuidGen(), &experimentationUid);

        // Returns the number of seconds since 1970-01-01T00:00:00 Universal Coordinated Time.
        time_t now = QDateTime::currentSecsSinceEpoch();

        // Converts the time since the Epoch in seconds to the 'date' type
        cass_uint32_t creationDate = cass_date_from_epoch(now);

        // Converts the time since the Epoch in seconds to the 'time' type
        cass_int64_t creationTime = cass_time_from_epoch(now);

        // Create the query
        QString query = QString("INSERT INTO ingescape.experimentation (id, name, creation_date, creation_time, group_name) VALUES (?, ?, ?, ?, ?);");

        // Creates the new query statement
        CassStatement* cassStatement = cass_statement_new(query.toStdString().c_str(), 5);
        cass_statement_bind_uuid(cassStatement, 0, experimentationUid);
        cass_statement_bind_string(cassStatement, 1, experimentationName.toStdString().c_str());
        cass_statement_bind_uint32(cassStatement, 2, creationDate);
        cass_statement_bind_int64(cassStatement, 3, creationTime);
        cass_statement_bind_string(cassStatement, 4, experimentationsGroup->name().toStdString().c_str());

        // Execute the query or bound statement
        CassFuture* cassFuture = cass_session_execute(_modelManager->getCassSession(), cassStatement);

        CassError cassError = cass_future_error_code(cassFuture);
        if (cassError == CASS_OK)
        {
            qInfo() << "Experimentation" << experimentationName << "inserted into the DataBase";

            // Create the new experimentation
            ExperimentationM* experimentation = new ExperimentationM(experimentationUid, experimentationName, QDateTime::currentDateTime(), nullptr);

            // Add to the group
            experimentationsGroup->experimentations()->append(experimentation);
        }
        else {
            qCritical() << "Could not insert the experimentation" << experimentationName << "into the DataBase:" << cass_error_desc(cassError);
        }

        cass_statement_free(cassStatement);
        cass_future_free(cassFuture);
    }
    else {
        qWarning() << "Cannot create new experimentation because name is empty (" << experimentationName << ") or group is null !";
    }
}


/**
 * @brief Return true if the user can create an experimentations group with the name
 * Check if the name is not empty and if a group with the same name does not already exist
 * @param experimentationsGroupName
 * @return
 */
bool ExperimentationsListController::canCreateExperimentationsGroupWithName(QString experimentationsGroupName)
{
    if (!experimentationsGroupName.isEmpty())
    {
        if (_getExperimentationsGroupFromName(experimentationsGroupName) != nullptr) {
            return false;
        }
        else {
            return true;
        }
    }
    else {
        return false;
    }
}


/**
 * @brief Open an experimentation of a group
 * @param experimentation
 */
void ExperimentationsListController::openExperimentationOfGroup(ExperimentationM* experimentation, ExperimentationsGroupVM* experimentationsGroup)
{
    if ((experimentation != nullptr) && (experimentationsGroup != nullptr))
    {
        qInfo() << "Open the experimentation" << experimentation->name() << "of the group" << experimentationsGroup->name();

        // Update the model manager
        if (_modelManager != nullptr)
        {
            _modelManager->setcurrentExperimentation(experimentation);
            _modelManager->setcurrentExperimentationsGroup(experimentationsGroup);
        }
    }
}


/**
 * @brief Delete an experimentation of a group
 * @param experimentation
 * @param experimentationsGroup
 */
void ExperimentationsListController::deleteExperimentationOfGroup(ExperimentationM* experimentation, ExperimentationsGroupVM* experimentationsGroup)
{
    if ((experimentation != nullptr) && (experimentationsGroup != nullptr))
    {
        qInfo() << "Delete the experimentation" << experimentation->name() << "of the group" << experimentationsGroup->name();

        // Remove from the group
        experimentationsGroup->experimentations()->remove(experimentation);

        // Free memory
        delete experimentation;
    }
}


/**
 * @brief Get the group of experimentations from a name
 * @param experimentationsGroupName
 * @return
 */
ExperimentationsGroupVM* ExperimentationsListController::_getExperimentationsGroupFromName(QString experimentationsGroupName)
{
    if (_hashFromNameToExperimentationsGroup.contains(experimentationsGroupName)) {
        return _hashFromNameToExperimentationsGroup.value(experimentationsGroupName);
    }
    else {
        return nullptr;
    }
}
