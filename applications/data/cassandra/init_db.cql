-- cleaning first
DROP KEYSPACE ingescape;

-- Keyspace definition
CREATE KEYSPACE ingescape WITH replication = {'class': 'SimpleStrategy', 'replication_factor': '1'}  AND durable_writes = true;

-- Tables definitions
-- - igs_recorder
CREATE TABLE ingescape.record (id timeuuid, id_experimentation timeuuid, year_month_day date, time_of_day time, end_time time, end_date date, name_record text, offset_tl bigint,
platform text, id_session timeuuid, PRIMARY KEY (id)) WITH gc_grace_seconds = 0;
CREATE INDEX ON ingescape.record (id_experimentation);
CREATE INDEX ON ingescape.record (id_session);

CREATE TABLE ingescape.event (time timeuuid, year_month_day date, time_of_day time, agent text, output text, type tinyint, id_record timeuuid, PRIMARY KEY (id_record, time) ) WITH CLUSTERING ORDER BY (time ASC) AND gc_grace_seconds = 0;
CREATE TABLE ingescape.int (time timeuuid, id_record timeuuid, value int, PRIMARY KEY (id_record, time)) WITH CLUSTERING ORDER BY (time ASC) AND gc_grace_seconds = 0;
CREATE TABLE ingescape.double (time timeuuid, id_record timeuuid, value double, PRIMARY KEY (id_record, time)) WITH CLUSTERING ORDER BY (time ASC) AND gc_grace_seconds = 0;
CREATE TABLE ingescape.string (time timeuuid, id_record timeuuid, value text, PRIMARY KEY (id_record, time)) WITH CLUSTERING ORDER BY (time ASC) AND gc_grace_seconds = 0;
CREATE TABLE ingescape.bool (time timeuuid, id_record timeuuid, value boolean, PRIMARY KEY (id_record, time)) WITH CLUSTERING ORDER BY (time ASC) AND gc_grace_seconds = 0;
CREATE TABLE ingescape.data (time timeuuid, id_record timeuuid, value blob, PRIMARY KEY (id_record, time)) WITH CLUSTERING ORDER BY (time ASC) AND gc_grace_seconds = 0;
CREATE TABLE ingescape.action (time timeuuid, id_record timeuuid, id_action int, line_tl int, PRIMARY KEY (id_record, time)) WITH CLUSTERING ORDER BY (time ASC) AND gc_grace_seconds = 0;
CREATE TABLE ingescape.mapping (time timeuuid, id_record timeuuid, mapping text, PRIMARY KEY(id_record, time)) WITH CLUSTERING ORDER BY (time ASC) AND gc_grace_seconds = 0;
CREATE TABLE ingescape.timeline (time timeuuid, id_record timeuuid, state text, PRIMARY KEY(id_record, time)) WITH CLUSTERING ORDER BY (time ASC) AND gc_grace_seconds = 0;

-- - ingescape_assessments
CREATE TABLE ingescape.experimentation (id timeuuid, name text, creation_date date, creation_time time, group_name text, PRIMARY KEY (id)) WITH gc_grace_seconds = 0;
CREATE TABLE ingescape.characteristic (id timeuuid, id_experimentation timeuuid, name text, value_type tinyint, enum_values list<text>, PRIMARY KEY (id_experimentation, id) ) WITH gc_grace_seconds = 0;
CREATE TABLE ingescape.subject (id timeuuid, id_experimentation timeuuid, displayed_id text, PRIMARY KEY (id_experimentation, id)) WITH gc_grace_seconds = 0;
CREATE TABLE ingescape.protocol (id timeuuid, id_experimentation timeuuid, name text, platform_file text, PRIMARY KEY (id_experimentation, id)) WITH gc_grace_seconds = 0;
CREATE TABLE ingescape.independent_var (id timeuuid, id_experimentation timeuuid, id_protocol timeuuid, name text, description text, value_type tinyint, enum_values list<text>, PRIMARY KEY (id_experimentation, id_protocol, id)) WITH gc_grace_seconds = 0;
CREATE TABLE ingescape.dependent_var (id timeuuid, id_experimentation timeuuid, id_protocol timeuuid, name text, description text, agent_name text, output_name text, PRIMARY KEY (id_experimentation, id_protocol, id)) WITH gc_grace_seconds = 0;
CREATE TABLE ingescape.characteristic_value (id_experimentation timeuuid, id_subject timeuuid, id_characteristic timeuuid, characteristic_value text, PRIMARY KEY (id_experimentation, id_subject, id_characteristic)) WITH gc_grace_seconds = 0;
CREATE TABLE ingescape.session (id timeuuid, id_experimentation timeuuid, id_subject timeuuid, id_protocol timeuuid, name text, comment text, start_date date, start_time time, end_date date, end_time time, PRIMARY KEY (id_experimentation, id)) WITH gc_grace_seconds = 0;
CREATE TABLE ingescape.independent_var_value (id_experimentation timeuuid, id_session timeuuid, id_independent_var timeuuid, independent_var_value text, PRIMARY KEY (id_experimentation, id_session, id_independent_var)) WITH gc_grace_seconds = 0;
