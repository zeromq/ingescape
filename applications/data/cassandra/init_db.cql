-- cleaning first
DROP KEYSPACE ingescape;

-- Keyspace definition
CREATE KEYSPACE ingescape WITH replication = {'class': 'SimpleStrategy', 'replication_factor': '1'}  AND durable_writes = true;

-- Tables definitions
-- - igs_recorder
CREATE TABLE ingescape.record (id timeuuid, id_experimentation timeuuid, year_month_day date, time_of_day time, end_time time, end_date date, name_record text, offset_tl bigint,
platform text, id_task_instance timeuuid, PRIMARY KEY (id_task_instance, id)) WITH gc_grace_seconds = 0;
CREATE INDEX ON ingescape.record (id_experimentation);
CREATE INDEX ON ingescape.record (id_task_instance);

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
CREATE TABLE ingescape.task (id timeuuid, id_experimentation timeuuid, name text, platform_file text, PRIMARY KEY (id_experimentation, id)) WITH gc_grace_seconds = 0;
CREATE TABLE ingescape.independent_var (id timeuuid, id_experimentation timeuuid, id_task timeuuid, name text, description text, value_type tinyint, enum_values list<text>, PRIMARY KEY (id_experimentation, id_task, id)) WITH gc_grace_seconds = 0;
CREATE TABLE ingescape.dependent_var (id timeuuid, id_experimentation timeuuid, id_task timeuuid, name text, description text, agent_name text, output_name text, PRIMARY KEY (id_experimentation, id_task, id)) WITH gc_grace_seconds = 0;
CREATE TABLE ingescape.characteristic_value (id_experimentation timeuuid, id_subject timeuuid, id_characteristic timeuuid, characteristic_value text, PRIMARY KEY (id_experimentation, id_subject, id_characteristic)) WITH gc_grace_seconds = 0;
CREATE TABLE ingescape.task_instance (id timeuuid, id_experimentation timeuuid, id_subject timeuuid, id_task timeuuid, name text, comment text, start_date date, start_time time, end_date date, end_time time, PRIMARY KEY (id)) WITH gc_grace_seconds = 0;
CREATE TABLE ingescape.independent_var_value (id_experimentation timeuuid, id_task_instance timeuuid, id_independent_var timeuuid, independent_var_value text, PRIMARY KEY (id_experimentation, id_task_instance, id_independent_var)) WITH gc_grace_seconds = 0;


--Insert data for test at ESTIA
INSERT INTO ingescape.task_instance (id_experimentation, id_subject, id_task, id, comment, end_date, end_time, name, start_date, start_time) 
VALUES(f74f1300-20b7-11ea-b92d-dbe62da11c1c,
05580fb0-20b8-11ea-b92d-dbe62da11c1c,
272b1e70-20b8-11ea-b92d-dbe62da11c1c,
400d34a0-20b8-11ea-b92d-dbe62da11c1c,
'',
'2019-12-17',
'10:30:30.000000000',
'MAXIM30101 - S-191217-112852-395 - 17/12/19',
'2019-12-17',
'10:30:30.000000000');


INSERT INTO ingescape.record (id ,end_date,end_time,id_experimentation,id_task_instance,name_record,offset_tl ,platform,time_of_day,year_month_day)
VALUES(1305a950-20e1-11ea-b3c0-15165d4db5c3,
'2019-12-17',
'15:23:01.694989000',
f74f1300-20b7-11ea-b92d-dbe62da11c1c,
400d34a0-20b8-11ea-b92d-dbe62da11c1c,
'MAXIM30101 - S-191217-112852-395 - 17/12/19 (00:00:00.000)',
 0,
{"agents":[{"agentName":"MAX30101Agent","definitions":[{"clones":[{"commandLine":"D:\\\\OneDrive - ESTIA\\\\CLIENTS\\\\AIRBUS\\\\WRIST-DOCS\\\\Livraison-Wrist-v2.0.0\\\\MAX30101Agent\\\\MAX30101Agent.exe","hostname":"PB-ALIEN-PEPSS"}],"definition":{"calls":[],"description":"NO_DESCRIPTION","inputs":[{"name":"ISMONITORING","type":"BOOL","value":"false"},{"name":"SampleRateHz","type":"INTEGER","value":0},{"name":"RedLedCurrentmA","type":"DOUBLE","value":0},{"name":"IrLedCurrentmA","type":"DOUBLE","value":0},{"name":"GreenLedCurrentmA","type":"DOUBLE","value":0}],"name":"NO_NAME","outputs":[{"name":"SENSOR1","type":"STRING","value":""},{"name":"SENSOR2","type":"STRING","value":""},{"name":"SENSOR1-LED-RED","type":"STRING","value":""},{"name":"SENSOR1-LED-IR","type":"STRING","value":""},{"name":"SENSOR1-LED-GREEN","type":"STRING","value":""},{"name":"SENSOR2-LED-RED","type":"STRING","value":""},{"name":"SENSOR2-LED-IR","type":"STRING","value":""},{"name":"SENSOR2-LED-GREEN","type":"STRING","value":""}],"parameters":[],"version":"1.0"}}]},{"agentName":"WristGUIAgent","definitions":[{"clones":[{"commandLine":"D:\\\\OneDrive - ESTIA\\\\CLIENTS\\\\AIRBUS\\\\WRIST-DOCS\\\\Livraison-Wrist-v2.0.0\\\\WristMainIHM\\\\WristMainIHM.exe","hostname":"PB-ALIEN-PEPSS"}],"definition":{"calls":[],"description":"NO_DESCRIPTION","inputs":[{"name":"SENSOR1","type":"STRING","value":""},{"name":"SENSOR2","type":"STRING","value":""},{"name":"REDBPM1","type":"DOUBLE","value":0},{"name":"IRBPM1","type":"DOUBLE","value":0},{"name":"GREENBPM1","type":"DOUBLE","value":0},{"name":"REDBPM2","type":"DOUBLE","value":0},{"name":"IRBPM2","type":"DOUBLE","value":0},{"name":"GREENBPM2","type":"DOUBLE","value":0},{"name":"REDBPM1MEAN","type":"DOUBLE","value":0},{"name":"IRBPM1MEAN","type":"DOUBLE","value":0},{"name":"GREENBPM1MEAN","type":"DOUBLE","value":0},{"name":"REDBPM2MEAN","type":"DOUBLE","value":0},{"name":"IRBPM2MEAN","type":"DOUBLE","value":0},{"name":"GREENBPM2MEAN","type":"DOUBLE","value":0}],"name":"NO_NAME","outputs":[{"name":"ISMONITORING","type":"BOOL","value":"false"},{"name":"RECORDING-FILE-PATH","type":"STRING","value":""},{"name":"SampleRateHz","type":"INTEGER","value":0},{"name":"RedLedCurrentmA","type":"DOUBLE","value":0},{"name":"IrLedCurrentmA","type":"DOUBLE","value":0},{"name":"GreenLedCurrentmA","type":"DOUBLE","value":0}],"parameters":[],"version":"NO_VERSION"}}]},{"agentName":"WristRecorderAgent","definitions":[{"clones":[{"commandLine":"D:\\\\OneDrive - ESTIA\\\\CLIENTS\\\\AIRBUS\\\\WRIST-DOCS\\\\Livraison-Wrist-v2.0.0\\\\WristRecorder\\\\WristRecorder.exe","hostname":"PB-ALIEN-PEPSS"}],"definition":{"calls":[],"description":"NO_DESCRIPTION","inputs":[{"name":"SENSOR1","type":"STRING","value":""},{"name":"SENSOR2","type":"STRING","value":""},{"name":"ISMONITORING","type":"BOOL","value":"false"},{"name":"RECORDING-FILE-PATH","type":"STRING","value":""},{"name":"SampleRateHz","type":"INTEGER","value":0}],"name":"NO_NAME","outputs":[{"name":"REDBPM1","type":"DOUBLE","value":0},{"name":"IRBPM1","type":"DOUBLE","value":0},{"name":"GREENBPM1","type":"DOUBLE","value":0},{"name":"REDBPM2","type":"DOUBLE","value":0},{"name":"IRBPM2","type":"DOUBLE","value":0},{"name":"GREENBPM2","type":"DOUBLE","value":0},{"name":"REDBPM1MEAN","type":"DOUBLE","value":0},{"name":"IRBPM1MEAN","type":"DOUBLE","value":0},{"name":"GREENBPM1MEAN","type":"DOUBLE","value":0},{"name":"REDBPM2MEAN","type":"DOUBLE","value":0},{"name":"IRBPM2MEAN","type":"DOUBLE","value":0},{"name":"GREENBPM2MEAN","type":"DOUBLE","value":0}],"parameters":[],"version":"NO_VERSION"}}]}],"mapping":[{"actions_mapping":[{"input_name":"ISMONITORING","output_name":"ISMONITORING"},{"input_name":"SampleRateHz","output_name":"SampleRateHz"},{"input_name":"RedLedCurrentmA","output_name":"RedLedCurrentmA"},{"input_name":"IrLedCurrentmA","output_name":"IrLedCurrentmA"},{"input_name":"GreenLedCurrentmA","output_name":"GreenLedCurrentmA"}],"agentName":"MAX30101Agent","mapping":{"description":"Mapping description of MAX30101Agent in IngeScape Editor","mapping_out":[{"agent_name":"WristGUIAgent","input_name":"ISMONITORING","output_name":"ISMONITORING"},{"agent_name":"WristGUIAgent","input_name":"SampleRateHz","output_name":"SampleRateHz"},{"agent_name":"WristGUIAgent","input_name":"RedLedCurrentmA","output_name":"RedLedCurrentmA"},{"agent_name":"WristGUIAgent","input_name":"IrLedCurrentmA","output_name":"IrLedCurrentmA"},{"agent_name":"WristGUIAgent","input_name":"GreenLedCurrentmA","output_name":"GreenLedCurrentmA"}],"name":"Mapping name of MAX30101Agent in IngeScape Editor","version":"0.0"},"position":"1684.43, 131.452","width":258},{"actions_mapping":[{"input_name":"SENSOR1","output_name":"SENSOR1"},{"input_name":"SENSOR2","output_name":"SENSOR2"},{"input_name":"REDBPM1","output_name":"REDBPM1"},{"input_name":"IRBPM1","output_name":"IRBPM1"},{"input_name":"GREENBPM1","output_name":"GREENBPM1"},{"input_name":"REDBPM2","output_name":"REDBPM2"},{"input_name":"IRBPM2","output_name":"IRBPM2"},{"input_name":"GREENBPM2","output_name":"GREENBPM2"},{"input_name":"REDBPM1MEAN","output_name":"REDBPM1MEAN"},{"input_name":"IRBPM1MEAN","output_name":"IRBPM1MEAN"},{"input_name":"GREENBPM1MEAN","output_name":"GREENBPM1MEAN"},{"input_name":"REDBPM2MEAN","output_name":"REDBPM2MEAN"},{"input_name":"IRBPM2MEAN","output_name":"IRBPM2MEAN"},{"input_name":"GREENBPM2MEAN","output_name":"GREENBPM2MEAN"}],"agentName":"WristGUIAgent","mapping":{"description":"Mapping description of WristGUIAgent in IngeScape Editor","mapping_out":[{"agent_name":"MAX30101Agent","input_name":"SENSOR1","output_name":"SENSOR1"},{"agent_name":"MAX30101Agent","input_name":"SENSOR2","output_name":"SENSOR2"},{"agent_name":"WristRecorderAgent","input_name":"REDBPM1","output_name":"REDBPM1"},{"agent_name":"WristRecorderAgent","input_name":"IRBPM1","output_name":"IRBPM1"},{"agent_name":"WristRecorderAgent","input_name":"GREENBPM1","output_name":"GREENBPM1"},{"agent_name":"WristRecorderAgent","input_name":"REDBPM2","output_name":"REDBPM2"},{"agent_name":"WristRecorderAgent","input_name":"IRBPM2","output_name":"IRBPM2"},{"agent_name":"WristRecorderAgent","input_name":"GREENBPM2","output_name":"GREENBPM2"},{"agent_name":"WristRecorderAgent","input_name":"REDBPM1MEAN","output_name":"REDBPM1MEAN"},{"agent_name":"WristRecorderAgent","input_name":"IRBPM1MEAN","output_name":"IRBPM1MEAN"},{"agent_name":"WristRecorderAgent","input_name":"GREENBPM1MEAN","output_name":"GREENBPM1MEAN"},{"agent_name":"WristRecorderAgent","input_name":"REDBPM2MEAN","output_name":"REDBPM2MEAN"},{"agent_name":"WristRecorderAgent","input_name":"IRBPM2MEAN","output_name":"IRBPM2MEAN"},{"agent_name":"WristRecorderAgent","input_name":"GREENBPM2MEAN","output_name":"GREENBPM2MEAN"}],"name":"Mapping name of WristGUIAgent in IngeScape Editor","version":"0.0"},"position":"1573.2, 401.9","width":258},{"actions_mapping":[{"input_name":"ISMONITORING","output_name":"ISMONITORING"},{"input_name":"SampleRateHz","output_name":"SampleRateHz"},{"input_name":"RECORDING-FILE-PATH","output_name":"RECORDING-FILE-PATH"},{"input_name":"SENSOR1","output_name":"SENSOR1"},{"input_name":"SENSOR2","output_name":"SENSOR2"}],"agentName":"WristRecorderAgent","mapping":{"description":"Mapping description of WristRecorderAgent in IngeScape Editor","mapping_out":[{"agent_name":"WristGUIAgent","input_name":"ISMONITORING","output_name":"ISMONITORING"},{"agent_name":"WristGUIAgent","input_name":"SampleRateHz","output_name":"SampleRateHz"},{"agent_name":"WristGUIAgent","input_name":"RECORDING-FILE-PATH","output_name":"RECORDING-FILE-PATH"},{"agent_name":"MAX30101Agent","input_name":"SENSOR1","output_name":"SENSOR1"},{"agent_name":"MAX30101Agent","input_name":"SENSOR2","output_name":"SENSOR2"}],"name":"Mapping name of WristRecorderAgent in IngeScape Editor","version":"0.0"},"position":"401.717, 693.272","width":258}],"scenario":{"actions":[],"actions_palette":[],"actions_timeline":[]},"timeline":{"pixels_per_minute":700},"version":"1.0"},
'15:22:44.709460000',
'2019-12-17');

INSERT INTO ingescape.record (id ,end_date, end_time, id_experimentation, id_task_instance, name_record, offset_tl , platform,time_of_day, year_month_day)
VALUES(1305a950-20e1-11ea-b3c0-15165d4db5c3,
'2019-12-17',
'15:23:01.694989000',
f74f1300-20b7-11ea-b92d-dbe62da11c1c,
400d34a0-20b8-11ea-b92d-dbe62da11c1c,
'MAXIM30101 - S-191217-112852-395 - 17/12/19 (00:00:00.000)',
 0,
'test platform',
'15:22:44.709460000',
'2019-12-17');

INSERT INTO ingescape.record (id ,end_date, end_time, id_experimentation, id_task_instance, name_record, offset_tl , platform,time_of_day, year_month_day)
VALUES(1305a950-20e1-11ea-b3c0-15165d4db5c4,
'2019-12-17',
'15:23:01.694989000',
aaa059a3-98ec-11e9-b475-0800200c9a66,
aaa059a3-98ec-11e9-b475-0800200c9a66,
'MAXIM30101 - S-191217-112852-395 - 17/12/19 (00:00:00.000)',
 0,
'test platform',
'15:22:44.709460000',
'2019-12-17');
