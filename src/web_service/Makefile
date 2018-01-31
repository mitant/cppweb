CXX = g++
CPPFLAGS = -std=c++11 -Wall -I/usr/local/Cellar/boost/1.66.0/include
CXXFLAGS = -g -Wall -DBOOST_ALL_NO_LIB
LDFLAGS = -L/usr/local/Cellar/boost/1.66.0/lib
LDLIBS = -lssl -lcrypto -lpthread -lboost_log_setup -lboost_log -lboost_system -lboost_thread -lboost_filesystem -lboost_atomic -lboost_chrono -lboost_date_time -lboost_regex

OBJECTS = web_service.o listener.o http_session.o http_error_handlers.o http_path.o root_certificates.hpp http_auth_client.o routes.o main.o

web_service: $(OBJECTS)
	$(CXX) $(CPPFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)
	