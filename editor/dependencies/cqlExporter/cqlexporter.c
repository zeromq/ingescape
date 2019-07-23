#include "cqlexporter.h"

#define INT64_MAX_CHARACT 65
#define INT32_MAX_CHARACT 33
#define DOUBLE_MAX_CHARACT 65 //TOCHECK
#define CASS

#ifdef WIN32
    #define strdup _strdup
#endif

typedef enum {
    REC_INTEGER_T = 1,  ///< integer value type
    REC_DOUBLE_T,       ///< double value type
    REC_STRING_T,       ///< string value type
    REC_BOOL_T,         ///< bool value type
    REC_IMPULSION_T,    ///< impulsion value type
    REC_DATA_T,         ///< data value type
    REC_UNKNOWN_T,      ///< for unknown parsed types
    REC_MAPPING_T,      ///< mapping changes
    REC_ACTION_T,       ///< action on the platform
} recordType_t;

//Global variable
FILE * fp;
char* currentLine;

CassCluster* cluster;
CassSession* session;
CassFuture* connect_future;

/*
 * BDD
 */

void connectToBDD(char* bddHost)
{
    /* Setup and connect to cluster */
    cluster = cass_cluster_new();
    session = cass_session_new();

    /* Add contact points */
    cass_cluster_set_contact_points(cluster, bddHost);

    /* Provide the cluster object as configuration to connect the session */
    connect_future = cass_session_connect(session, cluster);

    /* This operation will block until the result is ready */
    CassError rc = cass_future_error_code(connect_future);

    if (rc != CASS_OK) {
        /* Display connection error message */
        const char* message;
        size_t message_length;
        cass_future_error_message(connect_future, &message, &message_length);
        fprintf(stderr, "Connect error: '%.*s'\n", (int)message_length, message);
    }else{
        printf("CONNECTED to the database : %s", bddHost);
    }
}

void setCassSession(CassSession * cassSession){session = cassSession;}

void disconnectToBDD(void)
{
    cass_future_free(connect_future);
    cass_session_free(session);
    cass_cluster_free(cluster);
}

/*
 * write header functions
 */

void writeHeaderExperimentation(void){
    writeIntoFile("Group name of experimentation;", 0);
    writeIntoFile("Name of experimentation;", 0);
    writeIntoFile("datetime of experimentation;", 0);
}

void writeHeaderSubject(void){
    writeIntoFile("subject id;", 0);
}

void writeHeaderIndependentVar(CassUuid id_experimentation){
    char uuidExperimentationStr[CASS_UUID_STRING_LENGTH];
    cass_uuid_string ( id_experimentation, uuidExperimentationStr);

    printf("WRITE all the independent variable names from id experimentation : %s \n", uuidExperimentationStr);

    /* Create a statement with zero parameters */
    CassStatement* statement = cass_statement_new("SELECT * FROM ingescape.independent_var WHERE id_experimentation = ? ORDER BY id_task,id ASC;", 1);
    cass_statement_bind_uuid(statement, 0, id_experimentation);
    CassFuture* query_future = cass_session_execute(session, statement);

    /* Statement objects can be freed immediately after being executed */
    cass_statement_free(statement);

    /* This will block until the query has finished */
    CassError rc = cass_future_error_code(query_future);

    if(cass_future_error_code(query_future) == CASS_OK)
    {
        const CassResult* result = cass_future_get_result(query_future);
        cass_future_free(query_future);
        CassIterator* row_iterator = cass_iterator_from_result(result);

        // iterate over the outputs
        while(cass_iterator_next(row_iterator))
        {
            const CassRow* current_row = cass_iterator_get_row(row_iterator);

            if(current_row != NULL)
            {
                const char* name;
                size_t size_name;
                cass_value_get_string(cass_row_get_column_by_name(current_row, "name"), &name, &size_name);

                writeIntoFile(name, 0);
                writeIntoFile(";", 0);
            }
        }

    }else {
        printf("Query result: %s \n", cass_error_desc(rc));
    }
}

void writeHeaderTask(void){
        writeIntoFile("task name;", 0);
        writeIntoFile("platform json name of the task;", 0);
}

void writeHeaderRecordSetup(void){
    writeIntoFile("Record Setup Name;", 0);
    writeIntoFile("Start Datetime RecordSetup;", 0);
    writeIntoFile("End Datetime RecordSetup;", 0);
}

void writeHeaderRecord(void){
    writeIntoFile("End Datetime Record", 0);
    writeIntoFile(";", 0);
    writeIntoFile("record_name_record", 0);
    writeIntoFile(";", 0);
    writeIntoFile("record_offset_tl", 0);
    writeIntoFile(";", 0);
    writeIntoFile("record_platform_json", 0);
    writeIntoFile(";", 0);
    writeIntoFile("Start Datetime Record", 0);
    writeIntoFile(";", 0);
}

void writeHeaderEvent(void){
//    writeIntoFile("time of event", 0);
//    writeIntoFile(";", 0);
    writeIntoFile("agent name", 0);
    writeIntoFile(";", 0);
    writeIntoFile("output name", 0);
    writeIntoFile(";", 0);
    writeIntoFile("Datetime event", 0);
    writeIntoFile(";", 0);
    writeIntoFile("type of event", 0);
    writeIntoFile(";", 0);

    writeIntoFile("value of event", 0);
    writeIntoFile(";", 0);
}

void writeHeaderCharacteristic(CassUuid id_experimentation){
    char uuidStr[CASS_UUID_STRING_LENGTH];
    cass_uuid_string ( id_experimentation, uuidStr);

    printf("WRITE all characteristic name from id_experimentation : %s \n", uuidStr);

    /* Create a statement with zero parameters */
    CassStatement* statement = cass_statement_new("SELECT * FROM ingescape.characteristic  WHERE id_experimentation = ? ORDER BY id ASC;", 1);
    cass_statement_bind_uuid(statement, 0, id_experimentation);
    CassFuture* query_future = cass_session_execute(session, statement);

    /* Statement objects can be freed immediately after being executed */
    cass_statement_free(statement);

    /* This will block until the query has finished */
    CassError rc = cass_future_error_code(query_future);

    char * strToWrite = NULL;
    if(cass_future_error_code(query_future) == CASS_OK)
    {
        const CassResult* result = cass_future_get_result(query_future);
        cass_future_free(query_future);

        CassIterator* row_iterator = cass_iterator_from_result(result);

        // iterate over the outputs
        while(cass_iterator_next(row_iterator))
        {
            const CassRow* current_row = cass_iterator_get_row(row_iterator);

            if(current_row != NULL)
            {
                // Gets the informations we need to store
                CassUuid id;
                char idStr[CASS_UUID_STRING_LENGTH];
                cass_value_get_uuid(cass_row_get_column_by_name(current_row, "id"), &id);
                cass_uuid_string ( id, idStr );

                const char* name_characteristic;
                size_t size_name_charact;
                cass_value_get_string(cass_row_get_column_by_name(current_row, "name"), &name_characteristic, &size_name_charact);
                writeIntoFile(name_characteristic, 0);
                writeIntoFile(";", 0);
            }
        }
        cass_iterator_free(row_iterator);

    }else {
        printf("Query result error : %s \n", cass_error_desc(rc));
    }
}

/*
 * Writing functions
 */

void writeRecordInfoFromIdRecord(CassUuid id_record){
    char uuidStr[CASS_UUID_STRING_LENGTH];
    cass_uuid_string ( id_record, uuidStr);

    printf("WRITE all data of one record from id : %s \n", uuidStr);

    /* Create a statement with zero parameters */
    CassStatement* statement = cass_statement_new("SELECT * FROM ingescape.record  WHERE id = ?;", 1);
    cass_statement_bind_uuid(statement, 0, id_record);
    CassFuture* query_future = cass_session_execute(session, statement);

    /* Statement objects can be freed immediately after being executed */
    cass_statement_free(statement);

    /* This will block until the query has finished */
    CassError rc = cass_future_error_code(query_future);

    if(cass_future_error_code(query_future) == CASS_OK)
    {
        const CassResult* result_record = cass_future_get_result(query_future);
        cass_future_free(query_future);

        //One unique result
        const CassRow* row_record = cass_result_first_row(result_record);

        if(row_record != NULL)
        {
            // Gets the informations we need to store
            CassUuid id;
            cass_value_get_uuid(cass_row_get_column_by_name(row_record, "id"), &id);

            cass_uint32_t end_date;
            cass_value_get_uint32(cass_row_get_column_by_name(row_record, "end_date"), &end_date);

            cass_int64_t end_time;
            cass_value_get_int64(cass_row_get_column_by_name(row_record, "end_time"), &end_time);

            //Creation of the date time epoch
            cass_int64_t secsdateTimeEnd = cass_date_time_to_epoch(end_date, end_time);

            //Convert the date time to string human readable
            char dateTimeEnd[20];
            strftime(dateTimeEnd, 20, "%Y-%m-%d %H:%M:%S", localtime(&secsdateTimeEnd));
            writeIntoFile(dateTimeEnd, 1);
            writeIntoFile(";", 1);

            const char* name_record;
            size_t size_output;
            cass_value_get_string(cass_row_get_column_by_name(row_record, "name_record"), &name_record, &size_output);
            writeIntoFile(strdup(name_record), 1);
            writeIntoFile(";", 1);

            cass_int64_t offset_tl;
            cass_value_get_int64(cass_row_get_column_by_name(row_record, "offset_tl"), &offset_tl);
            char offset_tl_str[INT64_MAX_CHARACT];

            snprintf(offset_tl_str,INT64_MAX_CHARACT-1, "%lld", offset_tl);

            writeIntoFile(offset_tl_str, 1);
            writeIntoFile(";", 1);

//            TODO  : HANDLE THE PLATFORM DESCRIPTION
//            const char* platform;
//            size_t size_platform;
//            cass_value_get_string(cass_row_get_column_by_name(row_record, "platform"), &platform, &size_platform);
            writeIntoFile("FIXME : platform full plain json string", 1);
            writeIntoFile(";", 1);

            cass_uint32_t year_month_day;
            cass_value_get_uint32(cass_row_get_column_by_name(row_record, "year_month_day"), &year_month_day);

            cass_int64_t time_of_day;
            cass_value_get_int64(cass_row_get_column_by_name(row_record, "time_of_day"), &time_of_day);

            //Creation of the date time epoch
            cass_int64_t secsdateTimeStart = cass_date_time_to_epoch(year_month_day, time_of_day);

            //Convert the date time to string human readable
            char dateTimeStart[20];
            strftime(dateTimeStart, 20, "%Y-%m-%d %H:%M:%S", localtime(&secsdateTimeStart));
            writeIntoFile(dateTimeStart, 1);
            writeIntoFile(";", 1);
        }
    }else {
        printf("Query result: %s \n", cass_error_desc(rc));
    }
}

char* getTypeOfEventName(cass_int8_t typeOfEvent){
    char * typeOfEventName;
    switch (typeOfEvent) {
    case REC_INTEGER_T:
        typeOfEventName = strdup("integer");
        break;
    case REC_DOUBLE_T:
        typeOfEventName = strdup("double");
        break;
    case REC_STRING_T:
        typeOfEventName = strdup("string");
        break;
    case REC_BOOL_T:
        typeOfEventName = strdup("boolean");
        break;
    case REC_IMPULSION_T :
        typeOfEventName = strdup("impulsion");
        break;
    case REC_DATA_T :
        typeOfEventName = strdup("data");
        break;
    case REC_UNKNOWN_T :
        typeOfEventName = strdup("unknown");
        break;
    case REC_MAPPING_T:
       typeOfEventName = strdup("mapping");
        break;
    case REC_ACTION_T:
        typeOfEventName = strdup("action");
        break;
    default:
        typeOfEventName = strdup("unknown type of event number");
        break;
    }

    return typeOfEventName;
}

void writeValueOfEvent( CassUuid time, cass_int8_t type){

    //GET the name of the table
    char * cassandraTableName;
    switch (type) {
    case REC_ACTION_T:
        cassandraTableName = strdup("action");
        break;
    case REC_MAPPING_T:
        cassandraTableName = strdup("mapping");
        break;
    case REC_INTEGER_T:
        cassandraTableName = strdup("int");
        break;
    case REC_DOUBLE_T:
        cassandraTableName = strdup("double");
        break;
    case REC_STRING_T:
        cassandraTableName = strdup("string");
        break;
    case REC_BOOL_T:
        cassandraTableName = strdup("bool");
        break;
    case REC_DATA_T :
        cassandraTableName = strdup("data");
        break;
    default:
        return;
    }

    char uuidStr[CASS_UUID_STRING_LENGTH];
    cass_uuid_string (time, uuidStr);

    printf("WRITE one value of event FROM time : %s INTO THE TABLE %s \n", uuidStr, cassandraTableName);

    /*
     * Construct the query in function of the type of event
     */
    char query[50];
    strcpy(query, "SELECT * FROM ingescape.");
    strcat(query, cassandraTableName);
    strcat(query, "  WHERE time = ?;");

    /* Create a statement with zero parameters */
    CassStatement* statement = cass_statement_new(query, 1);
    cass_statement_bind_uuid(statement, 0, time);
    CassFuture* query_future = cass_session_execute(session, statement);

    /* Statement objects can be freed immediately after being executed */
    cass_statement_free(statement);

    /* This will block until the query has finished */
    CassError rc = cass_future_error_code(query_future);

    char * strToWrite = NULL;
    if(cass_future_error_code(query_future) == CASS_OK)
    {
        const CassResult* result = cass_future_get_result(query_future);
        cass_future_free(query_future);

        //One unique result row
        const CassRow* row = cass_result_first_row(result);

        if(row != NULL)
        {
            //Handle the value type of variable
            switch (type) {
            case REC_ACTION_T:
            {
                cass_int32_t line_tl;
                cass_value_get_int32(cass_row_get_column_by_name(row, "line_tl"), &line_tl);

                //Convert the value into char *
                strToWrite = (char *) malloc(INT32_MAX_CHARACT);
                snprintf(strToWrite,INT32_MAX_CHARACT-1, "%d", line_tl);
                break;
            }
            case REC_MAPPING_T:
            {
//                const char* mapping;
//                size_t size_value_mapping;
//                cass_value_get_string(cass_row_get_column_by_name(row, "mapping"), &mapping, &size_value_mapping);

//                //FIXME: we need to force ending \0 on output, certainly a bug in cassandra
//                strToWrite = (char *) malloc(size_value_mapping + 1);
//                strncpy(strToWrite, mapping, size_value_mapping);
//                strToWrite[size_value_mapping] = '\0';

                //FIXME : Handle the specific case of the mapping & the return line inside it
                strToWrite = strdup("FIXME : mapping json dump");
                break;
            }
            case REC_INTEGER_T:
            {
                cass_int32_t intNumber;
                cass_value_get_int32(cass_row_get_column_by_name(row, "value"), &intNumber);

                //Convert the value into char *
                strToWrite = (char *) malloc(INT32_MAX_CHARACT);
                snprintf(strToWrite,INT32_MAX_CHARACT-1, "%d", intNumber);
                break;
            }
            case REC_DOUBLE_T:
            {
                cass_double_t doubleNumber;
                cass_value_get_double(cass_row_get_column_by_name(row, "value"), &doubleNumber);

                //Convert the value into char *
                strToWrite = (char *) malloc(DOUBLE_MAX_CHARACT);
                snprintf(strToWrite,DOUBLE_MAX_CHARACT-1, "%f", doubleNumber);
                break;
            }
            case REC_STRING_T:
            {
                const char* value_string;
                size_t size_value_string;
                cass_value_get_string(cass_row_get_column_by_name(row, "value"), &value_string, &size_value_string);

                //FIXME: we need to force ending \0 on output, certainly a bug in cassandra
                strToWrite = (char *) malloc(size_value_string + 1);
                strncpy(strToWrite, value_string, size_value_string);
                strToWrite[size_value_string] = '\0';
                break;
            }
            case REC_BOOL_T:
            {
                cass_bool_t bool;
                cass_value_get_bool(cass_row_get_column_by_name(row, "value"), &bool);
                if (bool == cass_true)
                    strToWrite = strdup("true");
                else
                    strToWrite = strdup("false");
                break;
            }
            case REC_DATA_T:
            {
                //TODO : Handle the specific case of the binary type format
                strToWrite = strdup("TODO : binary data");
                break;
            }
            default:
                return;
            }

            writeIntoFile(strToWrite, 0);
            writeIntoFile(";", 0);

            //Release internal memory
            free(strToWrite);
        }


    }else {
        printf("Query result: %s \n", cass_error_desc(rc));
    }
}

void writeAllEventsFromIdRecord(CassUuid id_record){
    char uuidStr[CASS_UUID_STRING_LENGTH];
    cass_uuid_string ( id_record, uuidStr);

    printf("WRITE all events of one record from id record : %s \n", uuidStr);

    /* Create a statement with zero parameters */
    CassStatement* statement = cass_statement_new("SELECT * FROM ingescape.event  WHERE id_record = ?;", 1);
    cass_statement_bind_uuid(statement, 0, id_record);
    CassFuture* query_future = cass_session_execute(session, statement);

    /* Statement objects can be freed immediately after being executed */
    cass_statement_free(statement);

    /* This will block until the query has finished */
    CassError rc = cass_future_error_code(query_future);

    if(cass_future_error_code(query_future) == CASS_OK)
    {
        const CassResult* result = cass_future_get_result(query_future);
        cass_future_free(query_future);

        CassIterator* row_iterator = cass_iterator_from_result(result);

        //Initialize the row with the data comming from the assement side
        //Handle the special case of a record setup contain record which are not contained any events
        if(getNumberOfEventsFromIdRecord(id_record) == 0){
            return;
        }

        //Iterate over the results
        int iterator = 0;
        while(cass_iterator_next(row_iterator))
        {
            const CassRow* row = cass_iterator_get_row(row_iterator);

            if(row != NULL)
            {
                if(iterator > 0){
                    //Start a new line with the initial content
                    writeIntoFile("\n", 0);
                    writeIntoFile(currentLine, 0);
                }

                //Id of event
                CassUuid id;
                char idStr[CASS_UUID_STRING_LENGTH];
                cass_value_get_uuid(cass_row_get_column_by_name(row, "id_record"), &id);
                cass_uuid_string ( id, idStr );

                //Time event
                CassUuid time;
                cass_value_get_uuid(cass_row_get_column_by_name(row, "time"), &time);

                //Agent name
                const char* agent_name;
                size_t size_agent_name;
                cass_value_get_string(cass_row_get_column_by_name(row, "agent"), &agent_name, &size_agent_name);
                writeIntoFile(agent_name, 0);
                writeIntoFile(";", 0);

                //Agent's output name
                const char* output_name;
                size_t size_output_name;
                cass_value_get_string(cass_row_get_column_by_name(row, "output"), &output_name, &size_output_name);
                writeIntoFile(output_name, 0);
                writeIntoFile(";", 0);
                /*
                 * DateTime of event
                 */
                cass_uint32_t year_month_day;
                cass_value_get_uint32(cass_row_get_column_by_name(row, "year_month_day"), &year_month_day);

                cass_int64_t time_of_day;
                cass_value_get_int64(cass_row_get_column_by_name(row, "time_of_day"), &time_of_day);


                //Creation of the date time epoch
                cass_int64_t secsdateTime = cass_date_time_to_epoch(year_month_day, time_of_day);

                //Convert the date time to string human readable
                char dateTimeEvent[20];
                strftime(dateTimeEvent, 20, "%Y-%m-%d %H:%M:%S", localtime(&secsdateTime));
                writeIntoFile(dateTimeEvent, 0);
                writeIntoFile(";", 0);

                //Type of event
                cass_int8_t type;
                cass_value_get_int8(cass_row_get_column_by_name(row, "type"), &type);
                writeIntoFile(getTypeOfEventName(type), 0);
                writeIntoFile(";", 0);

                //Value of event
                writeValueOfEvent(time, type);

                //Increment the iterator
                iterator++;
            }
        }
        cass_iterator_free(row_iterator);
    }else {
        printf("Query result:%s \n", cass_error_desc(rc));
    }

    //Free the gloabal variable to store the line
    free(currentLine);
    currentLine = NULL;
}

void writeOneExpInfoFromId(CassUuid id_experimentation){
    char uuidStr[CASS_UUID_STRING_LENGTH];
    cass_uuid_string ( id_experimentation, uuidStr);

    printf("WRITE one experimentation from id : %s \n", uuidStr);

    /* Create a statement with zero parameters */
    CassStatement* statement = cass_statement_new("SELECT * FROM ingescape.experimentation  WHERE id = ?;", 1);
    cass_statement_bind_uuid(statement, 0, id_experimentation);
    CassFuture* query_future = cass_session_execute(session, statement);

    /* Statement objects can be freed immediately after being executed */
    cass_statement_free(statement);

    /* This will block until the query has finished */
    CassError rc = cass_future_error_code(query_future);

    char *dateExp = NULL, *timeExp = NULL, *temp = NULL, *strToWrite = NULL;
    if(cass_future_error_code(query_future) == CASS_OK)
    {
        const CassResult* result = cass_future_get_result(query_future);
        cass_future_free(query_future);

        //One unique result row
        const CassRow* row = cass_result_first_row(result);

        if(row != NULL)
        {
            //Group name
            const char* groupName;
            size_t size_groupName;
            cass_value_get_string(cass_row_get_column_by_name(row, "group_name"), &groupName, &size_groupName);
            writeIntoFile(groupName, 1);
            writeIntoFile(";", 1);

            // Name of experimentation
            const char* name;
            size_t size_name = 0;
            cass_value_get_string(cass_row_get_column_by_name(row, "name"), &name, &size_name);
            //FIXME: we need to force ending \0 on output, certainly a bug in cassandra
            free(strToWrite);
            strToWrite = (char *) malloc(size_name + 1);
            strncpy(strToWrite, name, size_name);
            strToWrite[size_name] = '\0';

            writeIntoFile(strToWrite, 1);
            writeIntoFile(";", 1);

            //Release the memory
            free(strToWrite);

            //Creation date
            cass_uint32_t creation_date;
            cass_value_get_uint32(cass_row_get_column_by_name(row, "creation_date"), &creation_date);

            //Creation time
            cass_int64_t creation_time;
            cass_value_get_int64(cass_row_get_column_by_name(row, "creation_time"), &creation_time);

            //Creation of the date time epoch
            cass_int64_t secsdateTime = cass_date_time_to_epoch(creation_date, creation_time);

            //Convert the date time to string human readable
            char dateTimeExp[20];
            strftime(dateTimeExp, 20, "%Y-%m-%d %H:%M:%S", localtime(&secsdateTime));
            writeIntoFile(dateTimeExp, 1);
            writeIntoFile(";", 1);
        }
    }else {
        printf("Query result: %s\n", cass_error_desc(rc));
    }
}

void writeOneSubjectNameFromIdExpAndIdSubject(CassUuid id_experimentation, CassUuid id_subject)
{
    char uuidExpStr[CASS_UUID_STRING_LENGTH];
    cass_uuid_string ( id_experimentation, uuidExpStr);

    char uuidSubjectStr[CASS_UUID_STRING_LENGTH];
    cass_uuid_string ( id_subject, uuidSubjectStr);

    printf("WRITE one subject id from id experimentation: %s & id subject: %s \n", uuidExpStr, uuidSubjectStr);

    /* Create a statement with zero parameters */
    CassStatement* statement = cass_statement_new("SELECT * FROM ingescape.subject  WHERE id_experimentation = ? AND id = ?;", 2);
    cass_statement_bind_uuid(statement, 0, id_experimentation);
    cass_statement_bind_uuid(statement, 1, id_subject);
    CassFuture* query_future = cass_session_execute(session, statement);

    /* Statement objects can be freed immediately after being executed */
    cass_statement_free(statement);

    /* This will block until the query has finished */
    CassError rc = cass_future_error_code(query_future);

    char * strToWrite = NULL;
    if(cass_future_error_code(query_future) == CASS_OK)
    {
        const CassResult* result_record = cass_future_get_result(query_future);
        cass_future_free(query_future);

        //One unique result row
        const CassRow* row_record = cass_result_first_row(result_record);

        if(row_record != NULL)
        {
            // Gets the informations we need to store
            CassUuid id;
            char idStr[CASS_UUID_STRING_LENGTH];
            cass_value_get_uuid(cass_row_get_column_by_name(row_record, "id"), &id);
            cass_uuid_string ( id, idStr );

            const char* name;
            size_t size_name;
            cass_value_get_string(cass_row_get_column_by_name(row_record, "displayed_id"), &name, &size_name);
            //FIXME: we need to force ending \0 on output, certainly a bug in cassandra
            free(strToWrite);
            strToWrite = (char *) malloc(size_name + 1);
            strncpy(strToWrite, name, size_name);
            strToWrite[size_name] = '\0';

            writeIntoFile(strToWrite, 1);
            writeIntoFile(";", 1);

            //Release the memory
            free(strToWrite);
        }
    }else {
        printf("Query result: %s \n", cass_error_desc(rc));
    }
}

void writeOneTaskInfoById(CassUuid id_experimentation, CassUuid id_task){
    char uuidStr[CASS_UUID_STRING_LENGTH];
    cass_uuid_string ( id_task, uuidStr);

    printf("WRITE one task info(name, platform) from id_experimentation & id task : %s \n", uuidStr);

    /* Create a statement with zero parameters */
    CassStatement* statement = cass_statement_new("SELECT * FROM ingescape.task  WHERE id_experimentation = ? AND id = ?;", 2);
    cass_statement_bind_uuid(statement, 0, id_experimentation);
    cass_statement_bind_uuid(statement, 1, id_task);
    CassFuture* query_future = cass_session_execute(session, statement);

    /* Statement objects can be freed immediately after being executed */
    cass_statement_free(statement);

    /* This will block until the query has finished */
    CassError rc = cass_future_error_code(query_future);

    char * strToWrite = NULL;
    if(cass_future_error_code(query_future) == CASS_OK)
    {
        const CassResult* result = cass_future_get_result(query_future);
        cass_future_free(query_future);

        //One unique result row
        const CassRow* row = cass_result_first_row(result);

        if(row != NULL)
        {
            // Gets the informations we need to store
            CassUuid id_experimentation;
            char idExpStr[CASS_UUID_STRING_LENGTH];
            cass_value_get_uuid(cass_row_get_column_by_name(row, "id_experimentation"), &id_experimentation);
            cass_uuid_string ( id_experimentation, idExpStr );

            const char* name;
            size_t size_name;
            cass_value_get_string(cass_row_get_column_by_name(row, "name"), &name, &size_name);
            writeIntoFile(name, 1);
            writeIntoFile(";", 1);

            const char* platform_file;
            size_t size_platform_file;
            cass_value_get_string(cass_row_get_column_by_name(row, "platform_file"), &platform_file, &size_platform_file);

            //FIXME: we need to force ending \0 on output, certainly a bug in cassandra
            free(strToWrite);
            strToWrite = (char *) malloc(size_platform_file + 1);
            strncpy(strToWrite, platform_file, size_platform_file);
            strToWrite[size_platform_file] = '\0';

            writeIntoFile(strToWrite, 1);
            writeIntoFile(";", 1);

            //Release the memory
            free(strToWrite);

        }
    }else {
        printf("Query result: %s \n", cass_error_desc(rc));
    }
}

void writeAllCharactValuesFromIdExpAndIdSubject(CassUuid id_exp, CassUuid id_subject) {
    char uuidIdSubjectStr[CASS_UUID_STRING_LENGTH];
    cass_uuid_string ( id_subject, uuidIdSubjectStr);

    char uuidIdExpStr[CASS_UUID_STRING_LENGTH];
    cass_uuid_string ( id_exp, uuidIdExpStr);

    printf("WRITE all characterictic values from  the id experimentation & the id subject : %s %s\n", uuidIdExpStr, uuidIdSubjectStr);

    /* Create a statement with zero parameters */
    CassStatement* statement = cass_statement_new("SELECT * FROM ingescape.characteristic_value_of_subject  WHERE id_experimentation = ? AND id_subject = ? ORDER BY id_subject,id_characteristic ASC;", 2);
    cass_statement_bind_uuid(statement, 0, id_exp);
    cass_statement_bind_uuid(statement, 1, id_subject);
    CassFuture* query_future = cass_session_execute(session, statement);

    /* Statement objects can be freed immediately after being executed */
    cass_statement_free(statement);

    /* This will block until the query has finished */
    CassError rc = cass_future_error_code(query_future);

    char *strToWrite = NULL;
    if(cass_future_error_code(query_future) == CASS_OK) {

        const CassResult* result = cass_future_get_result(query_future);
        cass_future_free(query_future);
        CassIterator* row_iterator = cass_iterator_from_result(result);

        // iterate over the outputs
        while(cass_iterator_next(row_iterator))
        {
            const CassRow* current_row = cass_iterator_get_row(row_iterator);

            if(current_row != NULL)
            {
                // Gets the informations we need to store
                CassUuid id_subjectIntoTable;
                char id_subjectStr[CASS_UUID_STRING_LENGTH];
                cass_value_get_uuid(cass_row_get_column_by_name(current_row, "id_subject"), &id_subjectIntoTable);
                cass_uuid_string ( id_subjectIntoTable, id_subjectStr );

                CassUuid id;
                char idStr[CASS_UUID_STRING_LENGTH];
                cass_value_get_uuid(cass_row_get_column_by_name(current_row, "id"), &id);
                cass_uuid_string ( id, idStr );

                CassUuid id_characteristic;
                char id_characteristicStr[CASS_UUID_STRING_LENGTH];
                cass_value_get_uuid(cass_row_get_column_by_name(current_row, "id_characteristic"), &id_characteristic);
                cass_uuid_string ( id_characteristic, id_characteristicStr );

                const char* characteristic_value;
                size_t size_characteristic_value;
                cass_value_get_string(cass_row_get_column_by_name(current_row, "characteristic_value"), &characteristic_value, &size_characteristic_value);

                //FIXME: we need to force ending \0 on output, certainly a bug in cassandra
                strToWrite = (char *) malloc(size_characteristic_value + 1);
                strncpy(strToWrite, characteristic_value, size_characteristic_value);
                strToWrite[size_characteristic_value] = '\0';

                writeIntoFile(strToWrite, 1);
                writeIntoFile(";", 1);

                //Release the memory
                free(strToWrite);
            }
        }
        cass_iterator_free(row_iterator);
    }else {
        printf("Query result: %s \n", cass_error_desc(rc));
    }
}

void writeAllIndepentVarFromIdExpAndIdRecord(CassUuid id_experimentation, CassUuid id_record_setup){
    char uuidIdExpStr[CASS_UUID_STRING_LENGTH];
    cass_uuid_string ( id_experimentation, uuidIdExpStr);
    char uuidIdRecordStr[CASS_UUID_STRING_LENGTH];
    cass_uuid_string ( id_record_setup, uuidIdRecordStr);

    printf("WRITE all independent variables from id experimentation & id_record_setup : %s %s \n", uuidIdExpStr, uuidIdRecordStr);

    /* Create a statement with zero parameters */
    CassStatement* statement = cass_statement_new("SELECT * FROM ingescape.independent_var_value_of_record_setup  WHERE id_experimentation = ? AND id_record_setup = ? ORDER BY id_record_setup, id_independent_var ASC;", 2);
    cass_statement_bind_uuid(statement, 0, id_experimentation);
    cass_statement_bind_uuid(statement, 1, id_record_setup);
    CassFuture* query_future = cass_session_execute(session, statement);

    /* Statement objects can be freed immediately after being executed */
    cass_statement_free(statement);

    /* This will block until the query has finished */
    CassError rc = cass_future_error_code(query_future);

    char * strToWrite = NULL;
    if(cass_future_error_code(query_future) == CASS_OK) {
        const CassResult* result = cass_future_get_result(query_future);
        cass_future_free(query_future);
        CassIterator* row_iterator = cass_iterator_from_result(result);

        // iterate over the outputs
        while(cass_iterator_next(row_iterator))
        {
            const CassRow* current_row = cass_iterator_get_row(row_iterator);

            if(current_row != NULL)
            {
                //id_independent_var
                CassUuid id_independent_var;
                cass_value_get_uuid(cass_row_get_column_by_name(current_row, "id_independent_var"), &id_independent_var);

                //id
                CassUuid id;
                cass_value_get_uuid(cass_row_get_column_by_name(current_row, "id"), &id);

                //independent_var_value
                const char* independent_var_value;
                size_t size_independent_var_value;
                cass_value_get_string(cass_row_get_column_by_name(current_row, "independent_var_value"), &independent_var_value, &size_independent_var_value);

                //FIXME: we need to force ending \0 on output, certainly a bug in cassandra
                strToWrite = (char *) malloc(size_independent_var_value + 1);
                strncpy(strToWrite, independent_var_value, size_independent_var_value);
                strToWrite[size_independent_var_value] = '\0';

                writeIntoFile(strToWrite, 1);
                writeIntoFile(";", 1);

                //Release the memory
                free(strToWrite);
            }
        }
        cass_iterator_free(row_iterator);
    }else {
        printf("Query result: %s \n", cass_error_desc(rc));
    }
}

int getNumberOfEventsFromIdRecord(CassUuid id_record){
    char uuidRecordStr[CASS_UUID_STRING_LENGTH];
    cass_uuid_string ( id_record, uuidRecordStr);
    int numberOfEvent = 0;

    printf("GET number of event FROM id record : %s \n", uuidRecordStr);

    /* Create a statement with zero parameters */
    CassStatement* statement = cass_statement_new("SELECT COUNT(*) FROM ingescape.event WHERE id_record = ?;", 1);
    cass_statement_bind_uuid(statement, 0, id_record);
    CassFuture* query_future = cass_session_execute(session, statement);

    /* Statement objects can be freed immediately after being executed */
    cass_statement_free(statement);

    /* This will block until the query has finished */
    CassError rc = cass_future_error_code(query_future);

    if(cass_future_error_code(query_future) == CASS_OK)
    {
        const CassResult* result = cass_future_get_result(query_future);
        cass_future_free(query_future);

        //One unique result
        const CassRow* row = cass_result_first_row(result);

        if(row != NULL)
        {
            cass_int64_t number;
            cass_value_get_int64(cass_row_get_column_by_name(row, "count"), &number);
            numberOfEvent = number;
        }

    }else {
        printf("Query result: %s \n", cass_error_desc(rc));
    }

    return numberOfEvent;
}

/*
 * The MAIN function for the export
 */

void exportAllRecordsFromIdExpAndTableRecordSetup(CassUuid idExp){
    char idExpStr[CASS_UUID_STRING_LENGTH];
    cass_uuid_string(idExp, idExpStr);
    printf("EXPORT the record setup from id exp : %s \n", idExpStr);

    /* Create a statement with zero parameters */
    CassStatement* statement = cass_statement_new("SELECT * FROM ingescape.record_setup  WHERE id_experimentation = ?;", 1);
    cass_statement_bind_uuid(statement, 0, idExp);
    CassFuture* query_future = cass_session_execute(session, statement);

    /* Statement objects can be freed immediately after being executed */
    cass_statement_free(statement);

    /* This will block until the query has finished */
    CassError rc = cass_future_error_code(query_future);

    /*
      * Header excel
      */
    writeHeaderExperimentation();
    writeHeaderSubject();
    writeHeaderTask();
    writeHeaderRecordSetup();
    writeHeaderCharacteristic(idExp);
    writeHeaderIndependentVar(idExp);
    writeHeaderRecord();
    writeHeaderEvent();

    //Add a return line at the end of the header line
    writeIntoFile("\n", 0);

    if(cass_future_error_code(query_future) == CASS_OK) {
        // iterate over the outputs
        const CassResult* result_recordsetup = cass_future_get_result(query_future);
        cass_future_free(query_future);
        CassIterator* rows_recordsetup = cass_iterator_from_result(result_recordsetup);

        //Iterate over the results
        while(cass_iterator_next(rows_recordsetup))
        {
            const CassRow* row_recordsetup = cass_iterator_get_row(rows_recordsetup);

            if(row_recordsetup != NULL)
            {
                /*
                 * RECORD SETUP
                 *
                 */
                // Id Record setup
                CassUuid id_recordSetup;
                cass_value_get_uuid(cass_row_get_column_by_name(row_recordsetup, "id"), &id_recordSetup);

                //Id Experimentation
                CassUuid id_experimentation;
                cass_value_get_uuid(cass_row_get_column_by_name(row_recordsetup, "id_experimentation"), &id_experimentation);

                //Experimentation name
                writeOneExpInfoFromId(id_experimentation);

                //ID Subject
                CassUuid id_subject;
                cass_value_get_uuid(cass_row_get_column_by_name(row_recordsetup, "id_subject"), &id_subject);

                //Subject Name
                writeOneSubjectNameFromIdExpAndIdSubject(id_experimentation, id_subject);

                //Id Task
                CassUuid id_task;
                cass_value_get_uuid(cass_row_get_column_by_name(row_recordsetup, "id_task"), &id_task);

                //Task name
                writeOneTaskInfoById(id_experimentation, id_task);

                //Record Setup name
                const char* recordsetup_name;
                size_t size_record_name;
                cass_value_get_string(cass_row_get_column_by_name(row_recordsetup, "name"), &recordsetup_name, &size_record_name);
                writeIntoFile(strdup(recordsetup_name), 1);
                writeIntoFile(";", 1);

                //Record Start date
                cass_uint32_t start_date_recordsetup;
                cass_value_get_uint32(cass_row_get_column_by_name(row_recordsetup, "start_date"), &start_date_recordsetup);

                //Record Start time
                cass_int64_t start_time_recordsetup;
                cass_value_get_int64(cass_row_get_column_by_name(row_recordsetup, "start_time"), &start_time_recordsetup);

                //Creation of the date time epoch
                time_t secsdateTimeStart = cass_date_time_to_epoch(start_date_recordsetup, start_time_recordsetup);

                //Convert the date time to string human readable
                char dateTimeStart[20];
                strftime(dateTimeStart, 20, "%Y-%m-%d %H:%M:%S", localtime(&secsdateTimeStart));
                writeIntoFile(dateTimeStart, 1);
                writeIntoFile(";", 1);

                //Record Stop date
                cass_uint32_t end_date_record;
                cass_value_get_uint32(cass_row_get_column_by_name(row_recordsetup, "end_date"), &end_date_record);

                //Record Stop time
                cass_int64_t end_time_record;
                cass_value_get_int64(cass_row_get_column_by_name(row_recordsetup, "end_time"), &end_time_record);

                //Creation of the date time epoch
                time_t secsdateTimeEnd = cass_date_time_to_epoch(end_date_record, end_time_record);

                //Convert the date time to string human readable
                char dateTimeEnd[20];
                strftime(dateTimeEnd, 20, "%Y-%m-%d %H:%M:%S", localtime(&secsdateTimeEnd));
                writeIntoFile(dateTimeEnd, 1);
                writeIntoFile(";", 1);

                /*
                * WRITE all values of the characteristics of the subject for this specific exp
                * */
                writeAllCharactValuesFromIdExpAndIdSubject(id_experimentation, id_subject);

                /*
                * WRITE all values of the independent variable for the specific record
                * */
                writeAllIndepentVarFromIdExpAndIdRecord(idExp, id_recordSetup);

                /*
                 * WRITE all informations about a record indexed with this specific record
                 * */
                CassUuid current_id_record;
                cass_value_get_uuid(cass_row_get_column_by_name(row_recordsetup, "id_records"), &current_id_record);
                writeRecordInfoFromIdRecord(current_id_record);

                /*
                 * WRITE all events stored by the agents indexed with this specific record
                 * */
                //Handle the multiple event associated with the same record
                writeAllEventsFromIdRecord(current_id_record);

                //Add a return line at the end of the process of the all events linked to one record for the next one
                writeIntoFile("\n", 0);
            }
        }
        cass_iterator_free(rows_recordsetup);
    }else {
        printf("Query result: %s \n", cass_error_desc(rc));
    }
}

/*
 * File management : create, close, write
 */

void openFile(char* fileFullPath){
    fp = fopen(fileFullPath, "w+");
    if ( fp == NULL ) {
            fprintf( stderr, "Cannot open file %s\n", fileFullPath);
            exit( 0 );
        }
}

void writeIntoFile(char * content, int saveIntoCurrentLine){
    size_t sizeOflastCurrentLineContent = 0;
    size_t sizeOfContent = 0;
    char * lastCurrentLineContent = NULL;

    //Si le content n'est pas null
    if(content){

        if(saveIntoCurrentLine > 0){

        if(currentLine != NULL){
            //allocation of memory
            sizeOflastCurrentLineContent = strlen (currentLine) + 1;
            lastCurrentLineContent = (char *) malloc(sizeOflastCurrentLineContent);

            //Copy the last content
            strcpy(lastCurrentLineContent, currentLine);

            //Alloc the size of the string for the new content
            sizeOfContent = strlen(content) + 1;
            sizeOflastCurrentLineContent = strlen(lastCurrentLineContent) + 1;
            free(currentLine);
            currentLine = (char *) malloc( sizeOflastCurrentLineContent + sizeOfContent);

            //Copy the previous content
            strcpy(currentLine, lastCurrentLineContent);

            //Concatenate the string
            strcat(currentLine, content);

            //Release memory
            free(lastCurrentLineContent);
        }else{
            sizeOfContent = strlen(content) + 1;
            currentLine = (char *) malloc( sizeOfContent);

            //Copy the first content
            strcpy(currentLine, content);
        }
    }


        fprintf(fp, content);
        fflush(fp);
    }
}

void  closeFileOpened(){
    fclose(fp);
}

/*
 * Create header functions
 */

