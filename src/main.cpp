#include <libudev.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <iostream>
#include <string>

std::string get_udev_path(uint16_t vid, uint16_t pid)
{
  std::string full_path;

  struct udev *udev = udev_new();
  if(udev) {
    struct udev_enumerate *enumerate = udev_enumerate_new(udev);
    udev_enumerate_add_match_subsystem(enumerate, "tty");
    udev_enumerate_scan_devices(enumerate);

    struct udev_list_entry *devices, *dev_list_entry;
    devices = udev_enumerate_get_list_entry(enumerate);
    udev_list_entry_foreach(dev_list_entry, devices)
    {
      const char         *path = udev_list_entry_get_name(dev_list_entry);
      struct udev_device *dev  = udev_device_new_from_syspath(udev, path);
      std::string         node = std::string(udev_device_get_devnode(dev));
      std::cout << "node:" << node << std::endl;
      dev =
          udev_device_get_parent_with_subsystem_devtype(dev, "usb", "usb_device");
      auto vid_s = udev_device_get_sysattr_value(dev, "idVendor");
      auto pid_s = udev_device_get_sysattr_value(dev, "idProduct");
      std::cout << vid_s << ":" << pid_s << std::endl;
      if(vid_s != nullptr && pid_s != nullptr &&
         vid == strtol(vid_s, NULL, 16) && pid == strtol(pid_s, NULL, 16))
      {
        full_path = node;
      }
      udev_device_unref(dev);
    }

    udev_enumerate_unref(enumerate);
    udev_unref(udev);
  }
  return full_path;
}

int main(void)
{
  std::cout << get_udev_path(0xA110, 0x303B) << std::endl;
  return 0;
}