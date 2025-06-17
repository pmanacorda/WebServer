build:
	g++ -std=c++23 -Wall \
		-Iserver/controllers -Iserver/http -Iserver/json -Iserver/socket -Iserver/services \
		main.cpp server/controllers/*.cpp server/http/*.cpp server/json/*.cpp server/socket/*.cpp server/services/*.cpp \
		-lssl -lcrypto \
		-o webserver

local:
	g++ -std=c++23 -Wall -g \
		-Iserver/controllers -Iserver/http -Iserver/json -Iserver/socket -Iserver/services \
		main.cpp server/controllers/*.cpp server/http/*.cpp server/json/*.cpp server/socket/*.cpp server/services/*.cpp \
		-lssl -lcrypto \
		-o webserver