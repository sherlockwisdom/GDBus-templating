
/*
 * https://docs.gtk.org/gio/index.html
 *
 * */

#include <iostream>
#include <cstdlib>
/*
#include <dbus/dbus.h>
#include <dbus/dbus-glib.h>
#include <glib.h>
*/
#include <gio/gio.h>


/*
void listener(GDBusError &error, GDBusConnection *connection) {
	GMainLoop *loop;
	loop = g_main_loop_new(NULL, FALSE);

	g_dbus_bus_add_match(connection,
			"type='signal', \
			interface='org.freedesktop.ModemManager1'",
			NULL);
	
	g_dbus_bus_add_match(connection,
			"type='signal', \
			interface='org.freedesktop.NetworkManager.AccessPoint'",
			NULL);

	g_dbus_connection_add_filter(connection,
			g_dbus_filter, loop, NULL);
	// deprecated
	// g_dbus_connection_setup_with_g_main(connection, NULL);
	// g_main_context_push_thread_default(NULL);
	g_main_loop_run(loop);
}
*/

void compose_message(GDBusConnection *connection, GError* error, 
		const gchar* connection_bus_name,
		const gchar* path,
		const gchar* interface_,
		const gchar* method) {

	GDBusMessage *g_dbus_message;
	GDBusMessage *g_dbus_reply;

	g_dbus_message = ::g_dbus_message_new_method_call(
			connection_bus_name,
			path, 
			interface_,
			method);

	// https://docs.gtk.org/glib/gvariant-format-strings.html
	// GVariant* boolean_enable = g_variant_new("(b)", 1);
	GVariant* boolean_enable = NULL;
	::g_dbus_message_set_body(
			g_dbus_message, 
			boolean_enable);

	g_dbus_message = g_dbus_connection_send_message_with_reply_sync(
			connection,
			g_dbus_message,
			G_DBUS_SEND_MESSAGE_FLAGS_NONE,
			-1,
			NULL,
			NULL,
			&error);
	if(error) {
		gchar* error_message = g_dbus_error_get_remote_error(error);
		std::cout << "Error sending message: " 
			<< error_message << std::endl;

		g_error_free(error);
		return;
	}
	std::cout << method << " method called successfully" << std::endl;

	GDBusMessageType g_dbus_message_type = g_dbus_message_get_message_type(g_dbus_message);
	switch(g_dbus_message_type) {
		case G_DBUS_MESSAGE_TYPE_INVALID:
			std::cout << "Message = Invalid" << std::endl;
			break;
		case G_DBUS_MESSAGE_TYPE_METHOD_CALL:
			std::cout << "Message = Method call" << std::endl;
			break;
		case G_DBUS_MESSAGE_TYPE_METHOD_RETURN:
			std::cout << "Message = Method return" << std::endl;
			break;
		case G_DBUS_MESSAGE_TYPE_ERROR:
			std::cout << "Message = Error" << std::endl;
			break;
		case G_DBUS_MESSAGE_TYPE_SIGNAL:
			std::cout << "Message = Signal" << std::endl;
			break;
		default:
			std::cout << "Fuck!" << std::endl;
	}
	
	
	// https://developer-old.gnome.org/gio/stable/GDBusMessage.html#g-dbus-message-print
	std::cout << g_dbus_message_print(g_dbus_message, 1) << std::endl;


	const gchar* result;
	GVariantIter *iter;
	g_variant_get(g_dbus_message_get_body(g_dbus_message), "(a{oa{sa{sv}}})", &iter);
	while (g_variant_iter_loop (iter, "{oa{sa{sv}}}", &result)) {
		// g_print("%s\n", result);
	}

	// std::cout << result << std::endl;





	/*
	const char *g_dbus_result;
	::g_dbus_message_get_args(
			g_dbus_reply, &error, 
			DBUS_TYPE_STRING, &g_dbus_result, 
			DBUS_TYPE_INVALID);

	if(g_dbus_error_is_remote_error(&error)) {
		std::cout << "Error communication to daemon bus: " 
			<< error.message << std::endl;

		g_dbus_error_free(&error);
		return;
	}

	std::cout << "Instrospection Result---\n" << std::endl;
	std::cout << g_dbus_result << std::endl;
	*/

	::g_object_unref(g_dbus_message);
}


void callbackFunction(GObject* source_object, GAsyncResult* res, gpointer user_data) {
	GError* error = NULL;
	GDBusObjectManager* g_dbus_object_manager = ::g_dbus_object_manager_client_new_finish( res, &error);
	if(error) {
		gchar* error_message = g_dbus_error_get_remote_error(error);
		std::cerr << error_message << std::endl;

		g_error_free(error);
		return;
	}

	std::cout << "* new object manager client created..." << std::endl;
}


void get_interfaces(GDBusConnection *connection, const gchar* path, const gchar* name, GError* error) {
	g_dbus_object_manager_client_new( 
			connection,
			G_DBUS_OBJECT_MANAGER_CLIENT_FLAGS_DO_NOT_AUTO_START,
			name,
			path,
			NULL,
			NULL,
			NULL,
			NULL,
			callbackFunction,
			NULL);
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


	std::cout << "- Connection Bus Name: " << 
		connection_bus_name << std::endl;
	std::cout << "- Interface: " << interface_ << std::endl;
	std::cout << "- Path: " << path << std::endl;
	std::cout << "- Method: " << method << std::endl;

	// get_interfaces(connection, path, connection_bus_name, error);

	
	compose_message(connection, error, 
			connection_bus_name, 
			path, 
			interface_,
			method);

	::g_dbus_connection_close_sync(connection, NULL, &error);
	return 0;
}
