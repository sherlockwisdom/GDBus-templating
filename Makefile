all:
	g++ `pkg-config --libs --cflags dbus-1 gio-2.0 gio-unix-2.0` main.cxx -o main -std=c++1z