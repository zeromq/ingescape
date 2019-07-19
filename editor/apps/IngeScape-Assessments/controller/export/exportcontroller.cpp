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


extern "C" {
    #include <../../dependencies/cqlExporter/cqlexporter.h>
}


/**
 * @brief Constructor
 * @param parent
 */
ExportController::ExportController(QObject *parent) : QObject(parent),
    _currentExperimentation(nullptr)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Export Controller";
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
        char chrExperimentationUid[CASS_UUID_STRING_LENGTH];
        cass_uuid_string(experimentationUid, chrExperimentationUid);

        qInfo() << "Export the experimentation" << _currentExperimentation->name() << "(" << chrExperimentationUid << ")";

        QString ipAddress = "127.0.0.1";
        QString exportFileName = QString("export_%1.csv").arg(_currentExperimentation->name());

        // Connect to the BDD
        connectToBDD((char*)ipAddress.toStdString().c_str());

        // Open the file to save the export
        openFile((char*)exportFileName.toStdString().c_str());

        // Export a full dump of the current experimentation
        exportAllRecordsFromIdExpAndTableRecordSetup(experimentationUid);

        // Disconnect from the BDD
        disconnectToBDD();

        // Close the opening file
        closeFileOpened();

    }
}
