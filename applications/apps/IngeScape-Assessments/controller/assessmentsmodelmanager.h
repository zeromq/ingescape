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
#include <I2PropertyHelpers.h>
#include <controller/ingescapemodelmanager.h>

#include "cassandra.h"


/**
 * @brief Equality operator to be able to compare CassUuid struct directly.
 * Required to use CassUuid type as QHash key.
 * @param left
 * @param right
 * @return
 */
bool operator==(const CassUuid& left, const CassUuid& right);

/**
 * @brief qHash function to compute a hash key for the given CassUuid.
 * Required to use CassUuid type as QHash key.
 * @param cassUuid
 * @return
 */
uint qHash(const CassUuid& cassUuid);


/**
 * @brief The AssessmentsModelManager class defines the manager for the data model of our IngeScape Assessments application
 */
class AssessmentsModelManager : public QObject
{
    Q_OBJECT

    // IP Address of the Cassandra database
    I2_QML_PROPERTY_CUSTOM_SETTER(QString, databaseAddress)

    // Flag indicating if we are connected to the Cassandra database
    I2_QML_PROPERTY_READONLY(bool, isConnectedToDatabase)

    // Error message when a connection to the database attempt fails
    I2_QML_PROPERTY_READONLY(QString, errorMessageWhenDatabaseConnectionFailed)


public:

    /**
     * @brief Accessor to the singleton instance
     * @return
     */
    static AssessmentsModelManager* instance();


    /**
     * @brief Method used to provide a singleton to QML
     * @param engine
     * @param scriptEngine
     * @return
     */
     static QObject* qmlSingleton(QQmlEngine* engine, QJSEngine* scriptEngine);


     /**
      * @brief Constructor
      * @param parent
      */
     explicit AssessmentsModelManager(QObject *parent = nullptr);


     /**
      * @brief Destructor
      */
     ~AssessmentsModelManager();


    /**
     * @brief Retrieve a 'text' value of given column inside the given row
     * and convert it to QString before returning it
     * @param row
     * @param columnName
     * @return
     */
    static QString getStringValueFromColumnName(const CassRow* row, const char* columnName);


    /**
     * @brief Retrive a full collection of 'text' for the given value inside the given row
     * @param row
     * @param columnName
     * @return
     */
    static QStringList getStringListFromColumnName(const CassRow* row, const char* columnName);


    /**
     * @brief Retrive a date and a time value from the given columns inside the given row
     * and convert it to a QDateTime before returning it
     * @param row
     * @param dateColumnName
     * @param timeColumnName
     * @return
     */
    static QDateTime getDateTimeFromColumnNames(const CassRow* row, const char* dateColumnName, const char* timeColumnName);


    /**
     * @brief Delete an entry of the template type from Cassandra DB.
     * The primary key list represents the entries to delete. Multiple keys my be passed for a single key.
     *   (in which case, the WHERE clause will be like 'column IN (?, ?, ...)')
     * Values must be in the order of the primary keys.
     * If more values than there are keys are given, only the n-first will be used in the query.
     * If less values than there are keys are given, the query will not filter against the extra keys
     */
    template <typename ModelClass>
    static bool deleteEntry(QList<QList<CassUuid>> filterValues)
    {
        // Lambda -> return if the given list if empty. Declared here for readability.
        auto isEmptyLambda = [](const QList<CassUuid>& uuidList) { return uuidList.isEmpty(); };

        // Check that filter values were given
        if (filterValues.isEmpty()
            || std::any_of(filterValues.begin(), filterValues.end(), isEmptyLambda)) {
            return false;
        }

        // Create an editable copy of the primary keys list
        QStringList copyKeysList = ModelClass::primaryKeys;

        // Removing extra primary keys
        while (copyKeysList.size() > filterValues.size()) {
            copyKeysList.removeLast();
        }

        // Removing extra filter values
        while (filterValues.size() > copyKeysList.size()) {
            filterValues.removeLast();
        }

        // Build the query with '?' placeholders
        QString queryStr = "DELETE FROM " + ModelClass::table + " WHERE ";
        size_t placeholderCount = 0;
        auto keyIt = copyKeysList.cbegin();
        auto valueIt = filterValues.cbegin();
        while (keyIt != copyKeysList.cend()) {
            // Join clauses with " AND "
            if (keyIt > copyKeysList.cbegin()) {
                queryStr += " AND ";
            }

            queryStr += *keyIt;
            if (valueIt->size() > 1) {
                // Multiple filter values for this key
                queryStr += " IN (";
                for (int i(0) ; i < valueIt->size() ; ++i) {
                    if (i > 0)
                    {
                        queryStr += ", ";
                    }
                    queryStr += "?";
                    ++placeholderCount;
                }
                queryStr += ")";
            }
            else {
                // Single filter value for this key
                queryStr += " = ?";
                ++placeholderCount;
            }

            ++keyIt;
            ++valueIt;
        }
        queryStr += ";";

        // Create the Cassandra query (aka. CassStatement)
        CassStatement* cassStatement = cass_statement_new(queryStr.toStdString().c_str(), placeholderCount);

        // Bind values to the query
        size_t idx = 0;
        for (valueIt = filterValues.cbegin() ; valueIt < filterValues.cend() ; ++valueIt)
        {
            for (CassUuid uuid : *valueIt) {
                cass_statement_bind_uuid(cassStatement, idx, uuid);
                ++idx;
            }
        }

        // Execute the query or bound statement
        CassFuture* cassFuture = cass_session_execute(AssessmentsModelManager::instance()->getCassSession(), cassStatement);
        CassError cassError = cass_future_error_code(cassFuture);
        if (cassError == CASS_OK)
        {
            qInfo() << typeid(ModelClass).name() << "has been successfully deleted from the DB";
        }
        else {
            qCritical() << "Could not delete" << typeid(ModelClass).name() << "from the DB:" << cass_error_desc(cassError);
        }

        // Clean-up cassandra objects
        cass_future_free(cassFuture);
        cass_statement_free(cassStatement);

        return cassError == CASS_OK;
    }


    /**
     * @brief Overload of bool deleteEntry(QList<QList<CassUuid>> filterValues)
     * Simply create a single-element QList<CassUuid> for every CassUuid from filterValues and call deleteEntry(QList<QList<CassUuid>> filterValues)
     */
    template <typename ModelClass>
    static bool deleteEntry(QList<CassUuid> filterValues)
    {
        // Check that filter values were given
        if (filterValues.isEmpty()) {
            return false;
        }

        QList<QList<CassUuid>> listOfList;
        for (CassUuid uuid : filterValues) {
            listOfList.append({ uuid });
        }

        return deleteEntry<ModelClass>(listOfList);
    }


    /**
     * @brief Execute a SELECT request and return the corresponding list of entries.
     * Filter values MUST be in the order of the primary keys.
     * If more values than there are keys are given, only the n-first will be used in the query.
     * If less values than there are keys are given, the query will not filter against the extra keys
     */
    template<class ModelClass>
    static QList<ModelClass*> select(QList<CassUuid> filterValues)
    {
        QList<ModelClass*> objectList;

        // Create an editable copy of the primary keys list
        QStringList copyKeysList = ModelClass::primaryKeys;

        // Removing extra primary keys
        while (copyKeysList.size() > filterValues.size()) {
            copyKeysList.removeLast();
        }

        // Removing extra filter values
        while (filterValues.size() > copyKeysList.size()) {
            filterValues.removeLast();
        }

        // Build the query with '?' placeholders
        QString queryStr = "SELECT * FROM " + ModelClass::table;
        if (!filterValues.isEmpty())
        {
            queryStr += " WHERE ";
            for (auto keyIt = copyKeysList.cbegin() ; keyIt != copyKeysList.cend() ; ++keyIt) {
                // Join clauses with " AND "
                if (keyIt > copyKeysList.cbegin()) {
                    queryStr += " AND ";
                }
                queryStr += *keyIt + " = ?";
            }
        }
        queryStr += ";";

        // Creates the new query statement
        CassStatement* cassStatement = cass_statement_new(queryStr.toStdString().c_str(), static_cast<size_t>(filterValues.size()));

        // Bind values to query
        size_t idx(0);
        for (CassUuid uuid : filterValues) {
            cass_statement_bind_uuid(cassStatement, idx, uuid);
            ++idx;
        }

        // Execute the query or bound statement
        CassFuture* cassFuture = cass_session_execute(AssessmentsModelManager::instance()->getCassSession(), cassStatement);
        CassError cassError = cass_future_error_code(cassFuture);
        if (cassError == CASS_OK)
        {
            qDebug() << "SELECT query for" << typeid(ModelClass).name() << "succeeded";
        }
        else {
            qCritical() << "SELECT query for" << typeid(ModelClass).name() << "failed. Error:" << cass_error_desc(cassError);
        }

        // Retrieve result set and iterate over the rows
        const CassResult* cassResult = cass_future_get_result(cassFuture);
        if (cassResult != nullptr)
        {
            CassIterator* cassIterator = cass_iterator_from_result(cassResult);

            while(cass_iterator_next(cassIterator))
            {
                const CassRow* row = cass_iterator_get_row(cassIterator);
                ModelClass* objectInstance = ModelClass::createFromCassandraRow(row);
                if (objectInstance != nullptr)
                {
                    objectList.append(objectInstance);
                }
            }

            cass_iterator_free(cassIterator);
        }

        return objectList;
    }


    /**
     * @brief Execute an INSERT query to update the entry corresponding to the given instance
     */
    template<class ModelClass>
    static bool insert(const ModelClass& modelInstance)
    {
        // Create an INSERT bound statement from the given model instance
        CassStatement* cassStatement = ModelClass::createBoundInsertStatement(modelInstance);

        // Execute the query or bound statement
        CassFuture* cassFuture = cass_session_execute(AssessmentsModelManager::instance()->getCassSession(), cassStatement);
        CassError cassError = cass_future_error_code(cassFuture);
        bool success = (cassError == CASS_OK);
        if (success)
        {
            qDebug() << "INSERT query for" << typeid(ModelClass).name() << "succeeded";
        }
        else {
            qCritical() << "INSERT query for" << typeid(ModelClass).name() << "failed. Error:" << cass_error_desc(cassError);
        }

        cass_statement_free(cassStatement);
        cass_future_free(cassFuture);

        return success;
    }


    /**
     * @brief Execute an UPDATE to update the entry corresponding to the given instance
     */
    template<class ModelClass>
    static bool update(const ModelClass& modelInstance)
    {
        // Create an UPDATE bound statement from the given model instance
        CassStatement* cassStatement = ModelClass::createBoundUpdateStatement(modelInstance);

        // Execute the query or bound statement
        CassFuture* cassFuture = cass_session_execute(AssessmentsModelManager::instance()->getCassSession(), cassStatement);
        CassError cassError = cass_future_error_code(cassFuture);
        bool success = (cassError == CASS_OK);
        if (success)
        {
            qDebug() << "UPDATE query for" << typeid(ModelClass).name() << "succeeded";
        }
        else {
            qCritical() << "UPDATE query for" << typeid(ModelClass).name() << "failed. Error:" << cass_error_desc(cassError);
        }

        cass_statement_free(cassStatement);
        cass_future_free(cassFuture);

        return success;
    }


    /**
     * @brief Get the Cassandra Cluster
     * @return
     */
    CassCluster* getCassCluster();


    /**
     * @brief Get the Cassandra Session
     * @return
     */
    CassSession* getCassSession();


    /**
     * @brief Get the Cassandra UUID generator
     * @return
     */
    CassUuidGen* getCassUuidGen();


    /**
     * @brief Connect to the Cassandra Database
     * @return
     */
    Q_INVOKABLE void connectToDatabase();


    /**
     * @brief DIS-connect from the Cassandra Database
     */
    void disconnectFromDatabase();


    /**
     * @brief Converts the given CassUuid into a readable QString
     * This is a static utility function and does not interact with the Cassandra server
     * @param cassUuid
     * @return
     */
    static QString cassUuidToQString(CassUuid cassUuid);


    /**
     * @brief Helper function generating a Cassandra UUID
     * @return
     */
    static CassUuid genCassUuid();


private:
    /**
     * @brief A cluster object describes the configuration of the Cassandra cluster and is used
     * to construct a session instance. Unlike other DataStax drivers the cluster object
     * does not maintain the control connection.
     */
    CassCluster* _cassCluster = nullptr;


    /**
     * @brief A session object is used to execute queries and maintains cluster state through
     * the control connection. The control connection is used to auto-discover nodes and
     * monitor cluster changes (topology and schema). Each session also maintains multiple
     * pools of connections to cluster nodes which are used to query the cluster.
     */
    CassSession* _cassSession = nullptr;


    /**
     * @brief A UUID generator privoded by Cassandra
     */
    CassUuidGen* _cassUuidGen = nullptr;

};

QML_DECLARE_TYPE(AssessmentsModelManager)

#endif // ASSESSMENTSMODELMANAGER_H
