#------------------------------------------------------#
#- Author: Allen Lee
#- Mail: trainlee1024@gmail.com
#------------------------------------------------------#

# make_file

CC=gcc
file=client.o ccommon.o

client : $(file)
	@# make client
	${CC} -o $@ ${file} -lncursesw

#.PHONY: debug
debug: ${file}
	${CC} -g -DDEBUG -o $@ ${file} -lpthread

client.o: client.c
	${CC} -c client.c -o $@

ccommon.o: ccommon.c
	${CC} -c ccommon.c -o $@

#.PHONY: clear
clear: 
	rm *.o client debug
	@echo "clear OK"
