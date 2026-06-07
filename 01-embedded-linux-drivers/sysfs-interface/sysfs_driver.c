#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/string.h>

#define DRIVER_NAME "sysfs_driver"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sohail");
MODULE_DESCRIPTION("sysfs interface driver");

static struct kobject *my_kobj;

// Simulated sensor data
static int temperature = 234;   // 23.4 degrees
static int humidity    = 650;   // 65.0 percent
static char status[32] = "ok";

// --- temperature attribute ---
static ssize_t temperature_show(struct kobject *kobj,
                                 struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d.%d\n", temperature / 10, temperature % 10);
}

static ssize_t temperature_store(struct kobject *kobj,
                                  struct kobj_attribute *attr,
                                  const char *buf, size_t count)
{
    int val;
    if (kstrtoint(buf, 10, &val) == 0)
        temperature = val;
    return count;
}

// --- humidity attribute ---
static ssize_t humidity_show(struct kobject *kobj,
                              struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d.%d\n", humidity / 10, humidity % 10);
}

// read-only — no store function, use NULL
static ssize_t status_show(struct kobject *kobj,
                            struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%s\n", status);
}

// Declare attributes
static struct kobj_attribute temperature_attr =
    __ATTR(temperature, 0664, temperature_show, temperature_store);

static struct kobj_attribute humidity_attr =
    __ATTR(humidity, 0444, humidity_show, NULL);

static struct kobj_attribute status_attr =
    __ATTR(status, 0444, status_show, NULL);

// Group all attributes together
static struct attribute *my_attrs[] = {
    &temperature_attr.attr,
    &humidity_attr.attr,
    &status_attr.attr,
    NULL    // null terminator required
};

static struct attribute_group my_attr_group = {
    .attrs = my_attrs,
};

static int __init sysfs_driver_init(void)
{
    int ret;

    // Create /sys/kernel/sysfs_driver/
    my_kobj = kobject_create_and_add(DRIVER_NAME, kernel_kobj);
    if (!my_kobj) {
        printk(KERN_ERR "%s: failed to create kobject\n", DRIVER_NAME);
        return -ENOMEM;
    }

    // Create all attribute files at once using the group
    ret = sysfs_create_group(my_kobj, &my_attr_group);
    if (ret) {
        printk(KERN_ERR "%s: failed to create sysfs group\n", DRIVER_NAME);
        kobject_put(my_kobj);
        return ret;
    }

    printk(KERN_INFO "%s: loaded\n", DRIVER_NAME);
    printk(KERN_INFO "%s: check /sys/kernel/%s/\n", DRIVER_NAME, DRIVER_NAME);
    return 0;
}

static void __exit sysfs_driver_exit(void)
{
    sysfs_remove_group(my_kobj, &my_attr_group);
    kobject_put(my_kobj);
    printk(KERN_INFO "%s: unloaded\n", DRIVER_NAME);
}

module_init(sysfs_driver_init);
module_exit(sysfs_driver_exit);
