src = $(wildcard ../src/*.cpp ../src/*/*.cpp)

main = ../src/main.cpp
inc = $(filter-out $(main), $(wildcard ../src/*/))
include = $(addprefix -I, $(inc))

args = -std=c++11 -Wall -g $(include) -lpthread -lmysqlclient -ldl -lssl -lcrypto -lresolv -lm -lrt
all:server

server: $(src)
	g++ $^ -o ../bin/$@ $(args)

clean:
	-rm -rf $(obj) ../bin/server
.PHONY: clean all