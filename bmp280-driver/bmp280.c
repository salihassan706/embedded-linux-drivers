#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/i2c.h>
#include <linux/of.h>
#include <linux/delay.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>

#define DRIVER_NAME   "bmp280"
#define BMP280_ID     0x58
#define REG_CHIP_ID   0xD0
#define REG_CTRL      0xF4
#define REG_TEMP_MSB  0xFA
#define REG_TEMP_LSB  0xFB
#define REG_TEMP_XLSB 0xFC
#define REG_DIG_T1    0x88
#define REG_DIG_T2    0x8A
#define REG_DIG_T3    0x8C

static struct bmp280_data *g_data;


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sohail Ali Hassan Abbasi");
MODULE_DESCRIPTION("BMP280 temperature sensor driver");

struct bmp280_data {
    struct i2c_client *client;
    struct kobject *kobj;
    u16 dig_T1;
    s16 dig_T2;
    s16 dig_T3;
};

static s32 bmp280_read_temperature(struct bmp280_data *data)
{
    s32 msb, lsb, xlsb, raw_temp;
    s32 var1, var2, t_fine, temperature;

    /* Trigger forced mode measurement */
    i2c_smbus_write_byte_data(data->client, REG_CTRL, 0x25);
    msleep(10);

    /* Read raw temperature */
    msb  = i2c_smbus_read_byte_data(data->client, REG_TEMP_MSB);
    lsb  = i2c_smbus_read_byte_data(data->client, REG_TEMP_LSB);
    xlsb = i2c_smbus_read_byte_data(data->client, REG_TEMP_XLSB);
    raw_temp = (msb << 12) | (lsb << 4) | (xlsb >> 4);

    /* Apply compensation formula */
    var1 = ((raw_temp >> 3) - ((s32)data->dig_T1 << 1));
    var1 = (var1 * (s32)data->dig_T2) >> 11;
    var2 = ((raw_temp >> 4) - (s32)data->dig_T1);
    var2 = ((var2 * var2) >> 12) * (s32)data->dig_T3;
    var2 = var2 >> 14;
    t_fine = var1 + var2;
    temperature = (t_fine * 5 + 128) >> 8;

    return temperature;
}

/* sysfs show function — called when user reads the file */
static ssize_t temperature_show(struct kobject *kobj,
                                 struct kobj_attribute *attr, char *buf)
{
    s32 temp;
    if (!g_data)
        return -ENODEV;
    temp = bmp280_read_temperature(g_data);
    return sprintf(buf, "%d.%02d\n", temp / 100, temp % 100);
}
static struct kobj_attribute temperature_attr =
    __ATTR(temperature, 0444, temperature_show, NULL);

static int bmp280_probe(struct i2c_client *client)
{
    int chip_id, ret;
    struct bmp280_data *data;

    /* Verify chip */
    chip_id = i2c_smbus_read_byte_data(client, REG_CHIP_ID);
    if (chip_id != BMP280_ID) {
        dev_err(&client->dev, "Wrong chip ID: 0x%x\n", chip_id);
        return -ENODEV;
    }
    dev_info(&client->dev, "BMP280 found, chip ID: 0x%x\n", chip_id);

    /* Allocate driver data */
    data = devm_kzalloc(&client->dev, sizeof(*data), GFP_KERNEL);
    if (!data)
        return -ENOMEM;

    data->client = client;
    g_data = data;
    i2c_set_clientdata(client, data);

    /* Read calibration */
    data->dig_T1 = i2c_smbus_read_word_data(client, REG_DIG_T1);
    data->dig_T2 = i2c_smbus_read_word_data(client, REG_DIG_T2);
    data->dig_T3 = i2c_smbus_read_word_data(client, REG_DIG_T3);

    dev_info(&client->dev, "Calibration: T1=%u T2=%d T3=%d\n",
             data->dig_T1, data->dig_T2, data->dig_T3);

    /* Create sysfs entry */
    data->kobj = kobject_create_and_add("bmp280", kernel_kobj);
    if (!data->kobj)
        return -ENOMEM;

    ret = sysfs_create_file(data->kobj, &temperature_attr.attr);
    if (ret) {
        kobject_put(data->kobj);
        return ret;
    }

    dev_info(&client->dev, "BMP280 ready. Read: cat /sys/kernel/bmp280/temperature\n");
    return 0;
}

static void bmp280_remove(struct i2c_client *client)
{
    struct bmp280_data *data = i2c_get_clientdata(client);
    sysfs_remove_file(data->kobj, &temperature_attr.attr);
    g_data = NULL;
    kobject_put(data->kobj);
    dev_info(&client->dev, "BMP280 removed\n");
}

static const struct i2c_device_id bmp280_id[] = {
    { "bmp280", 0 },
    { }
};
MODULE_DEVICE_TABLE(i2c, bmp280_id);

static const struct of_device_id bmp280_of_match[] = {
    { .compatible = "bosch,bmp280" },
    { }
};
MODULE_DEVICE_TABLE(of, bmp280_of_match);

static struct i2c_driver bmp280_driver = {
    .driver = {
        .name           = DRIVER_NAME,
        .of_match_table = bmp280_of_match,
    },
    .probe    = bmp280_probe,
    .remove   = bmp280_remove,
    .id_table = bmp280_id,
};

module_i2c_driver(bmp280_driver);
