
OBJ_S=myserver 
OBJ_C=myclient
SRC_S=myserver.c
SRC_C=myclient.c

myserver:
$(OBJ_S):$(SRC_S)
	gcc -o $@  $^

myclient:
$(OBJ_C):$(SRC_C)
	gcc -o $@  $^

clean: 
	rm ./$(OBJ_S) $(OBJ_C)

##   cc -pthread -o sender sender.o common.o

