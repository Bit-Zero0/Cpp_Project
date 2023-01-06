DUG=debug
PARSER=parser
HTTP_SERVER=http_server

.PHONY:all
all:$(PARSER) $(HTTP_SERVER) $(DUG) 

$(PARSER):parser.cpp
	g++ -o $@ $^ -std=c++11  -lboost_system -lboost_filesystem
$(HTTP_SERVER):http_server.cpp
	g++ -o $@ $^ -std=c++11 -ljsoncpp -lpthread  
$(DUG):debug.cpp
	g++ -o $@ $^ -std=c++11 -ljsoncpp 


.PHONY:clean
clean:
	rm -f $(PARSER) $(DUG)  $(HTTP_SERVER)
