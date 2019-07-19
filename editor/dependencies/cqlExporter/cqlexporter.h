#ifndef CQLEXPORTER_H
#define CQLEXPORTER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#include "cassandra.h"


////BDD
void connectToBDD(char* bddName);
void setCassSession(CassSession * cassSession);
void disconnectToBDD(void);

////File management
void openFile(char* fileFullPath);
void writeIntoFile(char * content, int saveIntoCurrentLine);
void closeFileOpened(void);

////Main function of the exporting API
void exportAllRecordsFromIdExpAndTableRecordSetup(CassUuid idExp);

#endif // CQLEXPORTER_H
