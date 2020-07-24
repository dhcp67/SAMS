#------------------------------------------------------#
#- Author: Allen Lee
#- Mail: trainlee1024@gmail.com
#------------------------------------------------------#

# make_file

CC=gcc
file=server.o scommon.o

server : $(file)
	@# make server
	${CC} -o $@ ${file} -lpthread

#.PHONY: debug
debug: ${file}
	${CC} -g -DDEBUG -o $@ ${file} -lpthread

scommon.o: scommon.c
	${CC} -c scommon.c -o $@

#.PHONY: clear
clear: 
	rm *.o server debug
	@echo "OK"
