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

#include "exportcontroller.h"
#include <controller/assessmentsmodelmanager.h>
#include <misc/ingescapeutils.h>


extern "C" {
    #include <cqlexporter.h>
}


/**
 * @brief Constructor
 * @param parent
 */
ExportController::ExportController(QObject *parent) : QObject(parent),
    _currentExperimentation(nullptr),
    _exportsDirectoryPath("")
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Export Controller";

    // Init the path to the directory containing CSV files about exports
    _exportsDirectoryPath = IngeScapeUtils::getExportsPath();

}


/**
 * @brief Destructor
 */
ExportController::~ExportController()
{
    qInfo() << "Delete Export Controller";

    // Reset the model of the current experimentation
    if (_currentExperimentation != nullptr) {
        setcurrentExperimentation(nullptr);
    }
}


/**
 * @brief Export the current experimentation
 */
void ExportController::exportExperimentation()
{
    if (_currentExperimentation != nullptr)
    {
        CassUuid experimentationUid = _currentExperimentation->getCassUuid();
        QString expeUuidString = AssessmentsModelManager::cassUuidToQString(experimentationUid);

        qInfo() << "Export the experimentation" << _currentExperimentation->name() << "(" << expeUuidString << ")";

        QString exportFilePath = QString("%1export_%2.csv").arg(_exportsDirectoryPath, _currentExperimentation->name());

        // Open the file to save the export
        openFile(const_cast<char*>(exportFilePath.toStdString().c_str()));

        // Set the cassandra session for the exporting layer
        setCassSession(AssessmentsModelManager::Instance()->getCassSession());

        // Export a full dump of the current experimentation
        exportAllRecordsFromIdExpAndTableRecordSetup(experimentationUid);

        // Close the opening file
        closeFileOpened();

    }
}

/**
 * @brief Export the current experimentation function test
 */
void ExportController::exportExperimentationTest(QString uuidIdExp)
{
    CassUuid experimentationUid;
    cass_uuid_from_string(uuidIdExp.toLatin1().constData(), &experimentationUid);
    QString expeUuidString = AssessmentsModelManager::cassUuidToQString(experimentationUid);

    qInfo() << "Export the experimentation from UUID : "<< uuidIdExp;

    QString ipAddress = "127.0.0.1";
    QString exportFileName = QString("export_test.csv");

    // Connect to the BDD
    connectToBDD(const_cast<char*>(ipAddress.toStdString().c_str()));

    // Open the file to save the export
    openFile(const_cast<char*>(exportFileName.toStdString().c_str()));

    // Export a full dump of the current experimentation
    exportAllRecordsFromIdExpAndTableRecordSetup(experimentationUid);

    // Disconnect from the BDD
    disconnectToBDD();

    // Close the opening file
    closeFileOpened();
}
