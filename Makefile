build:
	g++ -std=c++23 -Wall \
		-Iserver/controllers -Iserver/http -Iserver/json -Iserver/socket \
		main.cpp server/controllers/*.cpp server/http/*.cpp server/json/*.cpp server/socket/*.cpp \
		-lssl -lcrypto \
		-o webserver

local:
	g++ -std=c++23 -Wall -g \
		-Iserver/controllers -Iserver/http -Iserver/json -Iserver/socket \
		-I/opt/homebrew/opt/openssl@3/include \
		-L /opt/homebrew/opt/openssl@3/lib -lssl -lcrypto \
		main.cpp server/controllers/*.cpp server/http/*.cpp server/json/*.cpp server/socket/*.cpp \
		-o webserver