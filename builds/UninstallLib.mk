DEST=/usr/local
INC_DEST=$(DEST)/include/mastic
LIB_DEST=$(DEST)/lib

all :
	sudo rm -rf $(INC_DEST)/*
	sudo rm -rf $(LIB_DEST)/libmastic.*
