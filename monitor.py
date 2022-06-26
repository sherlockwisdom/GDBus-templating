#!/usr/bin/env python3


import dbus
from dbus.mainloop.glib import DBusGMainLoop
from gi.repository import GLib



DBusGMainLoop(set_as_default=True)
loop = GLib.MainLoop()
bus = dbus.SystemBus()

name = 'org.freedesktop.ModemManager1'
path = '/org/freedesktop/ModemManager1'
interface = 'org.freedesktop.DBus.ObjectManager'

dbus_modem_manager = bus.get_object(name, path, True)

dbus_interface_obj_mnger = dbus.Interface(dbus_modem_manager, dbus_interface=interface)

def modem_added(modem_path):
    """
    """
    print("- modem added: %s" % (modem_path))
    dbus_interface_obj_mnger = dbus.Interface(dbus_modem_manager, dbus_interface=interface)

    """
    dbus_interface_obj_mnger.connect_to_signal(
            "InterfacesRemoved", 
            handler_function=handler_function_interfaces_changed, 
            path_keyword='path', 
            member_keyword='member', 
            interface_keyword='interface', 
            destination_keyword='destination',
            sender_keyword='sender')
    """


def modem_removed(modem_path):
    """
    """
    print("- modem removed: %s" % (modem_path))


interface_added_str = "InterfacesAdded"
interface_removed_str = "InterfacesRemoved"


def handler_function_interfaces_changed(*args, **kwargs):
    # print(args)
    print(kwargs)

    modem_path = ""

    if kwargs['interface'] == interface:
        modem_path = args[0]

    # check if truly added
    if kwargs['member'] == interface_added_str:
        modem_added(modem_path)

    elif kwargs['member'] == interface_removed_str:
        modem_removed(modem_path)


dbus_interface_obj_mnger.connect_to_signal(
        "InterfacesRemoved", 
        handler_function=handler_function_interfaces_changed, 
        path_keyword='path', 
        member_keyword='member', 
        interface_keyword='interface', 
        destination_keyword='destination',
        sender_keyword='sender')

dbus_interface_obj_mnger.connect_to_signal(
        "InterfacesAdded", 
        handler_function=handler_function_interfaces_changed, 
        path_keyword='path', 
        member_keyword='member', 
        interface_keyword='interface', 
        destination_keyword='destination',
        sender_keyword='sender')

loop.run()
