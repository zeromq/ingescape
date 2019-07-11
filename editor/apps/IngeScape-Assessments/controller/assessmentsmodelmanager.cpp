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

#include "assessmentsmodelmanager.h"

/**
 * @brief Constructor
 * @param jsonHelper
 * @param rootDirectoryPath
 * @param parent
 */
AssessmentsModelManager::AssessmentsModelManager(JsonHelper* jsonHelper,
                                                 QString rootDirectoryPath,
                                                 QObject *parent) : IngeScapeModelManager(jsonHelper,
                                                                                          rootDirectoryPath,
                                                                                          parent),
    _cassServer("localhost"),
    _currentExperimentationsGroup(nullptr),
    _currentExperimentation(nullptr),
    _cassCluster(nullptr),
    _cassSession(nullptr),
    _cassUuidGen(nullptr)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New IngeScape Assessments Model Manager";


    //
    // Init about Cassandra DataBase
    //
    _cassCluster = cass_cluster_new();
    _cassSession = cass_session_new();
    _cassUuidGen = cass_uuid_gen_new();

    cass_cluster_set_contact_points(_cassCluster, _cassServer.toStdString().c_str());

    // Connects a session
    CassFuture* cassFuture = cass_session_connect(_cassSession, _cassCluster);

    CassError cassError = cass_future_error_code(cassFuture);
    if (cassError == CASS_OK) {
        qInfo() << "Connected to the Cassandra DataBase on" << _cassServer;
    }
    else {
        qCritical() << "Could not connect to the Cassandra DataBase on" << _cassServer << "(" << cass_error_desc(cassError) << ")";
    }

    cass_future_free(cassFuture);


    //
    // FIXME for tests
    //
    /*
    QStringList enumBool = { tr("FALSE"), tr("TRUE") };
    //QStringList enumBool_French = { "FAUX", "VRAI" };

    QStringList enumYesNo = { tr("NO"), tr("YES") };
    //QStringList enumYesNo_French = { "NON", "OUI" };

    QStringList enumGender = { tr("NEUTRAL"), tr("WOMAN"), tr("MAN") };
    //QStringList enumGender_French = { "NEUTRE", "FEMME", "HOMME" };

    CharacteristicM *characteristicBool = new CharacteristicM("Booléen", CharacteristicValueTypes::CHARACTERISTIC_ENUM);
    characteristicBool->setenumValues(enumBool);

    CharacteristicM *characteristicYesNo = new CharacteristicM("Oui/Non", CharacteristicValueTypes::CHARACTERISTIC_ENUM);
    characteristicYesNo->setenumValues(enumYesNo);

    CharacteristicM *characteristicGender = new CharacteristicM("Genre", CharacteristicValueTypes::CHARACTERISTIC_ENUM);
    characteristicGender->setenumValues(enumGender);
    */
}


/**
 * @brief Destructor
 */
AssessmentsModelManager::~AssessmentsModelManager()
{
    qInfo() << "Delete IngeScape Assessments Model Manager";

    if (_currentExperimentationsGroup != nullptr) {
        setcurrentExperimentationsGroup(nullptr);
    }

    if (_currentExperimentation != nullptr) {
        setcurrentExperimentation(nullptr);
    }


    //
    // Cleaning about Cassandra DataBase
    //
    if (_cassUuidGen != nullptr) {
        cass_uuid_gen_free(_cassUuidGen);
    }

    if (_cassSession != nullptr)
    {
        // Closes the session instance, outputs a close future which can be used to determine when the session has been terminated.
        // This allows in-flight requests to finish.
        //cass_session_close(_cassSession);

        cass_session_free(_cassSession);
    }

    if (_cassCluster != nullptr) {
        cass_cluster_free(_cassCluster);
    }

    // Mother class is automatically called
    //IngeScapeModelManager::~IngeScapeModelManager();
}


/**
 * @brief Get the Cassandra Cluster
 * @return
 */
CassCluster* AssessmentsModelManager::getCassCluster()
{
    return _cassCluster;
}


/**
 * @brief Get the Cassandra Session
 * @return
 */
CassSession* AssessmentsModelManager::getCassSession()
{
    return _cassSession;
}


/**
 * @brief Get the Cassandra UUID generator
 * @return
 */
CassUuidGen* AssessmentsModelManager::getCassUuidGen()
{
    return _cassUuidGen;
}
