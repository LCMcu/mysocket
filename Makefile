OBJ_S=myserver 
OBJ_C=myclient
SRC_S=myserver.c
SRC_C=myclient.c
MYLIB_PATH=/home/virbox/work/train/sqlite_socket/mylib

all: $(OBJ_S) $(OBJ_C)

#myserver:
$(OBJ_S):$(SRC_S)
	gcc -pthread -o $@  $^ -L $(MYLIB_PATH) -l mysqlite -lsqlite3

#myclient:
$(OBJ_C):$(SRC_C)
	gcc -o $@  $^

clean: 
	rm ./$(OBJ_S) $(OBJ_C)

##   cc -pthread -o sender sender.o common.o

