DEST=/usr/local
INC_DEST=$(DEST)/include/mastic
LIB_DEST=$(DEST)/lib
HEADERS_DIR=../src/include
LIB_DIR=linux

YAJL_DIR=../dependencies/yajl/lloyd-yajl-2.1.0/Linux/lib
ZYRE_DIR=../dependencies/zyre/bin/Linux

install:
	@echo "install include and lib for MASTIC"
	mkdir -p $(INC_DEST)
	sudo install $(HEADERS_DIR)/*.h -D $(INC_DEST)
	mkdir -p $(INC_DEST)/uthash
	sudo install $(HEADERS_DIR)/uthash/* -D $(INC_DEST)/uthash/
	sudo install $(LIB_DIR)/libmastic.* -D $(LIB_DEST)
	sudo install $(YAJL_DIR)/libyajl* -D $(LIB_DEST)
	sudo install $(ZYRE_DIR)/* -D $(LIB_DEST)
	sudo ldconfig
