#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/i2c.h>
#include <linux/of.h>          // ← ADD THIS for Device Tree support

#define DRIVER_NAME "i2c_dummy"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sohail");
MODULE_DESCRIPTION("I2C dummy driver with Device Tree support");

static int i2c_dummy_probe(struct i2c_client *client)
{
    printk(KERN_INFO "%s: device found at address 0x%02x\n",
           DRIVER_NAME, client->addr);
    return 0;
}

static void i2c_dummy_remove(struct i2c_client *client)
{
    printk(KERN_INFO "%s: device removed from address 0x%02x\n",
           DRIVER_NAME, client->addr);
}

// Original ID table — for manual instantiation
static const struct i2c_device_id i2c_dummy_id[] = {
    { "i2c_dummy", 0 },
    { }
};
MODULE_DEVICE_TABLE(i2c, i2c_dummy_id);

// Device Tree match table — for automatic boot-time binding
static const struct of_device_id i2c_dummy_of_match[] = {
    { .compatible = "i2c_dummy" },
    { }
};
MODULE_DEVICE_TABLE(of, i2c_dummy_of_match);

static struct i2c_driver i2c_dummy_driver = {
    .driver = {
        .name           = DRIVER_NAME,
        .owner          = THIS_MODULE,
        .of_match_table = i2c_dummy_of_match,  // ← ADD THIS
    },
    .probe    = i2c_dummy_probe,
    .remove   = i2c_dummy_remove,
    .id_table = i2c_dummy_id,
};

module_i2c_driver(i2c_dummy_driver);
