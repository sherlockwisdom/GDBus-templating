#!/usr/bin/env python3

import dbus
from dbus.mainloop.glib import DBusGMainLoop
from gi.repository import GLib


interface_added_str = "InterfacesAdded"
interface_removed_str = "InterfacesRemoved"

def modem_property_changed(*args, **kwargs):
    print("Modem property changed...")
    print(args, '\n')
    print(kwargs, '\n\n')


def modem_added(modem_path, *args):
    """
    """
    print("- modem added: %s" % (modem_path))

    dbus_modem = bus.get_object(name, modem_path, True)

    modem_interface = 'org.freedesktop.ModemManager1.Modem.Modem3gpp'
    modem_dbus_props_iface = 'org.freedesktop.DBus.Properties'


    props = dbus.Interface(dbus_modem, dbus_interface=modem_dbus_props_iface)

    """
    print(props.GetAll(modem_interface))
    print("Operator code:", props.Get(modem_interface, "OperatorCode"))
    print("Operator name:", props.Get(modem_interface, "OperatorName"))
    """

    props.connect_to_signal(
            "PropertiesChanged",
            handler_function=modem_property_changed,
            path_keyword='path', 
            member_keyword='member', 
            interface_keyword='interface', 
            destination_keyword='destination',
            sender_keyword='sender')


def modem_removed(modem_path):
    """
    """
    print("- modem removed: %s" % (modem_path))


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


if __name__ == '__main__':
    DBusGMainLoop(set_as_default=True)
    loop = GLib.MainLoop()
    bus = dbus.SystemBus()

    name = 'org.freedesktop.ModemManager1'
    path = '/org/freedesktop/ModemManager1'
    interface = 'org.freedesktop.DBus.ObjectManager'

    dbus_mm = bus.get_object(name, path, True)

    mm_iface_obj_mng = dbus.Interface(dbus_mm, dbus_interface=interface)

    managed_objects : dict = mm_iface_obj_mng.GetManagedObjects()

    for object_path, _ in managed_objects.items():
        print("- connected: %s" % (object_path))
        modem_added(object_path)

    mm_iface_obj_mng.connect_to_signal(
            "InterfacesRemoved", 
            handler_function=handler_function_interfaces_changed, 
            path_keyword='path', 
            member_keyword='member', 
            interface_keyword='interface', 
            destination_keyword='destination',
            sender_keyword='sender')

    mm_iface_obj_mng.connect_to_signal(
            "InterfacesAdded", 
            handler_function=handler_function_interfaces_changed, 
            path_keyword='path', 
            member_keyword='member', 
            interface_keyword='interface', 
            destination_keyword='destination',
            sender_keyword='sender')

    loop.run()
