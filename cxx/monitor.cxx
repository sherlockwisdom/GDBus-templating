
#include <iostream>
#include <cstdlib>
#include <gio/gio.h>


void manage_objects(
		GDBusConnection *connection, 
		const gchar* name, 
		gchar* path,
		GError* error) {
	/*
	GDBusObjectManagerServer* manager = g_dbus_object_manager_server_new(path);

	std::cout << "Manager path: " << 
		::g_dbus_object_manager_get_object_path(manager) << std::endl;

	::g_object_unref(manager);
	*/

	GDBusObjectManager* manager = ::g_dbus_object_manager_client_new(
			connection,
			G_DBUS_OBJECT_MANAGER_CLIENT_FLAGS_DO_NOT_AUTO_START,
			name,
			path,
			NULL,
			NULL,
			NULL,
			NULL,
			(GAsyncReadyCallback) callback,
			null);

	if(error) {
		std::cerr << "Some error happenend!" << std::endl;
	}
}


int main(int argc, gchar** argv) {
	gchar* path;
	gchar* method;
	const gchar* connection_bus_name = "org.freedesktop.ModemManager1";
	// const gchar* interface_ = "org.freedesktop.ModemManager1.Modem";
	const gchar* interface_ = "org.freedesktop.DBus.ObjectManager";

	for(int i=1; i<argc;++i) {
		if(std::string(argv[i]) == "--method") {
			method = argv[i+1];
			++i;
		}
		else if(std::string(argv[i]) == "--path") {
			path = argv[i+1];
			++i;
		}
	}

	if(path == NULL || method == NULL) {
		if(path == NULL) 
			std::cout<< "Path cannot be NULL" << std::endl;
		if(method == NULL) 
			std::cout<< "Method cannot be NULL" << std::endl;
		return 1;
	}

	GDBusConnection *connection;
	GError *error = NULL;

	connection = g_bus_get_sync(
			G_BUS_TYPE_SYSTEM, NULL, &error);

	if(error) {
		gchar* error_message = g_dbus_error_get_remote_error(error);
		std::cout << "Error connecting to daemon bus: " 
			<< error_message << std::endl;

		g_error_free(error);
		return 1;
	}

	const gchar* bus_unique_name = 
		::g_dbus_connection_get_unique_name(connection);

	std::cout << "Connected to D-Bus as -> " << 
		bus_unique_name << std::endl;

	manage_objects(connection, connection_bus_name, path, error);

	GMainLoop* main_loop = g_main_loop_new(NULL, false);
	g_main_loop_quit(main_loop);

	::g_dbus_connection_close_sync(connection, NULL, &error);
	return 0;
}
