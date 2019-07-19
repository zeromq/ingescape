-- Commandes d’initalisation de la base

-- IgsRecorder app tables initialization
CREATE KEYSPACE ingescape WITH replication = {'class': 'SimpleStrategy','replication_factor': '1'} AND durable_writes = true;
CREATE TABLE ingescape.record (id timeuuid,id_recordsetup timeuuid, year_month_day date, time_of_day time,end_time time, end_date date, name_record text, offset_tl bigint, platform text, PRIMARY KEY (id));
CREATE TABLE ingescape.event (time timeuuid, year_month_day date, time_of_day time,agent text, output text, type tinyint, id_record timeuuid, PRIMARY KEY (id_record, time) )WITH CLUSTERING ORDER BY (time ASC);
CREATE TABLE ingescape.int (time timeuuid, value int, PRIMARY KEY (time));
CREATE TABLE ingescape.double (time timeuuid, value double, PRIMARY KEY (time));
CREATE TABLE ingescape.string (time timeuuid, value text, PRIMARY KEY (time));
CREATE TABLE ingescape.bool (time timeuuid, value boolean, PRIMARY KEY (time));
CREATE TABLE ingescape.data (time timeuuid, value blob, PRIMARY KEY (time));
CREATE TABLE ingescape.action (time timeuuid, id_action int, line_tl int, PRIMARY KEY(time));
CREATE TABLE ingescape.mapping (time timeuuid, mapping text, PRIMARY KEY(time));


-- Assement & export app tables initialization
CREATE TABLE ingescape.record_setup (id timeuuid, id_experimentation timeuuid,name text, start_date date, start_time time, end_date date, end_time time,id_subject timeuuid, id_task timeuuid,id_records timeuuid, PRIMARY KEY (id_experimentation, id_subject, id_task, id_records, id));
CREATE TABLE ingescape.experimentation (id timeuuid, name text, group_name text, creation_date date,creation_time time, PRIMARY KEY(id));
CREATE TABLE ingescape.characteristic (id timeuuid, id_experimentation timeuuid,name text, value_type tinyint, enum_values text, PRIMARY KEY (id_experimentation, id));
CREATE TABLE ingescape.subject (id timeuuid, id_experimentation timeuuid, name text, PRIMARY KEY (id_experimentation, id) ) WITH CLUSTERING ORDER BY (id ASC);
CREATE TABLE ingescape.task (id timeuuid, id_experimentation timeuuid, name text,platform_file text, PRIMARY KEY (id_experimentation, id) ) WITH CLUSTERING ORDER BY (id ASC);
CREATE TABLE ingescape.independent_var (id timeuuid, id_experimentation timeuuid, id_task timeuuid, name text, description text, value_type tinyint, enum_values text, PRIMARY KEY (id_experimentation, id_task, id)) WITH CLUSTERING ORDER BY (id_task ASC);

-- Assement & export app JOIN tables initialization
CREATE TABLE ingescape.characteristic_value_of_subject(id timeuuid, id_experimentation timeuuid, id_subject timeuuid, id_characteristic timeuuid, characteristic_value text, PRIMARY KEY (id_experimentation, id_subject, id_characteristic, id)) WITH CLUSTERING ORDER BY (id_subject ASC);
CREATE TABLE ingescape.independent_var_value_of_record_setup(id timeuuid, id_experimentation timeuuid, id_record_setup timeuuid, id_independent_var timeuuid, independent_var_value text, PRIMARY KEY (id_experimentation, id_record_setup, id_independent_var, id)) WITH CLUSTERING ORDER BY (id_record_setup ASC);

-- TODO for filtering on export
-- CREATE TABLE ingescape.dependent_var (id timeuuid, id_experimentation timeuuid, id_task timeuuid, name text, description text, agent_name text, output_name text, PRIMARY KEY (id_experimentation, id_task, agent_name, output_name, id))WITH CLUSTERING ORDER BY (id_task ASC);
