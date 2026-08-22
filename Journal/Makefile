CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17

LIB = libJournalDLL.so
CLIENT = Client

all: $(LIB) $(CLIENT)

$(CLIENT): JournalClient/Client.cpp
	$(CXX) $(CXXFLAGS) JournalClient/Client.cpp -IJournalDLL -L. -lJournalDLL -o $(CLIENT)
 
$(LIB): JournalDLL/JournalDLL.cpp JournalDLL/JournalDLL.h
	$(CXX) $(CXXFLAGS) -fPIC -shared JournalDLL/JournalDLL.cpp -o $(LIB)
 
clean:
	rm -f $(CLIENT) $(LIB)