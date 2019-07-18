-- Keyspace definition
CREATE KEYSPACE ingescape WITH replication = {'class': 'SimpleStrategy', 'replication_factor': '1'}  AND durable_writes = true;

-- Tables definitions
CREATE TABLE ingescape.record (id timeuuid, year_month_day date, time_of_day time, end_time time, end_date date, name_record text, offset_tl bigint,
    platform text, PRIMARY KEY (id));
CREATE TABLE ingescape.event (time timeuuid, year_month_day date, time_of_day time, agent text, output text, type tinyint, id_record timeuuid, PRIMARY KEY (id_record, time) ) WITH CLUSTERING ORDER BY (time ASC);
CREATE TABLE ingescape.int (time timeuuid, value int, PRIMARY KEY (time));
CREATE TABLE ingescape.double (time timeuuid, value double, PRIMARY KEY (time));
CREATE TABLE ingescape.string (time timeuuid, value text, PRIMARY KEY (time));
CREATE TABLE ingescape.bool (time timeuuid, value boolean, PRIMARY KEY (time));
CREATE TABLE ingescape.data (time timeuuid, value blob, PRIMARY KEY (time));
CREATE TABLE ingescape.action (time timeuuid, id_action int, line_tl int, PRIMARY KEY (time));
CREATE TABLE ingescape.mapping (time timeuuid, mapping text, PRIMARY KEY(time));
CREATE TABLE ingescape.experimentation (id timeuuid, name text, creation_date date, creation_time time, group_name text, PRIMARY KEY (id));
CREATE TABLE ingescape.characteristic (id timeuuid, id_experimentation timeuuid, name text, value_type tinyint, enum_values text, PRIMARY KEY (id_experimentation, id) );
CREATE TABLE ingescape.subject (id timeuuid, id_experimentation timeuuid, name text, PRIMARY KEY (id_experimentation, id));
CREATE TABLE ingescape.task (id timeuuid, id_experimentation timeuuid, name text, platform_file text, PRIMARY KEY (id_experimentation, id));
CREATE TABLE ingescape.independent_var (id timeuuid, id_experimentation timeuuid, id_task timeuuid, name text, description text, value_type tinyint, enum_values text, PRIMARY KEY (id_experimentation, id_task, id));
CREATE TABLE ingescape.dependent_var (id timeuuid, id_experimentation timeuuid, id_task timeuuid, name text, description text, agent_name text, output_name text, PRIMARY KEY (id_experimentation, id_task, id));
