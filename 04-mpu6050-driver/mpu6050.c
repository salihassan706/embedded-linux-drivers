#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/i2c.h>
#include <linux/of.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/delay.h>

#define DRIVER_NAME         "mpu6050"
#define REG_PWR_MGMT_1      0x6B
#define REG_ACCEL_XOUT_H    0x3B
#define REG_ACCEL_YOUT_H    0x3D
#define REG_ACCEL_ZOUT_H    0x3F
#define REG_GYRO_XOUT_H     0x43
#define REG_GYRO_YOUT_H     0x45
#define REG_GYRO_ZOUT_H     0x47
#define ACCEL_SENSITIVITY   16384
#define GYRO_SENSITIVITY    131

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sohail Ali Hassan Abbasi");
MODULE_DESCRIPTION("MPU6050 6-axis IMU driver");

struct mpu6050_data {
    struct i2c_client *client;
    struct kobject *kobj;
};

static struct mpu6050_data *g_data;

static s16 mpu6050_read_pair(struct mpu6050_data *data, u8 reg_high)
{
    s16 high, low;
    high = i2c_smbus_read_byte_data(data->client, reg_high);
    low  = i2c_smbus_read_byte_data(data->client, reg_high + 1);
    return (high << 8) | low;
}

static ssize_t accel_x_show(struct kobject *kobj,
                              struct kobj_attribute *attr, char *buf)
{
    s16 raw;
    int milli_g;
    if (!g_data)
        return -ENODEV;
    raw     = mpu6050_read_pair(g_data, REG_ACCEL_XOUT_H);
    milli_g = (raw * 1000) / ACCEL_SENSITIVITY;
    return sprintf(buf, "%d.%03d\n", milli_g / 1000, abs(milli_g % 1000));
}

static ssize_t accel_y_show(struct kobject *kobj,
                              struct kobj_attribute *attr, char *buf)
{
    s16 raw;
    int milli_g;
    if (!g_data)
        return -ENODEV;
    raw     = mpu6050_read_pair(g_data, REG_ACCEL_YOUT_H);
    milli_g = (raw * 1000) / ACCEL_SENSITIVITY;
    return sprintf(buf, "%d.%03d\n", milli_g / 1000, abs(milli_g % 1000));
}

static ssize_t accel_z_show(struct kobject *kobj,
                              struct kobj_attribute *attr, char *buf)
{
    s16 raw;
    int milli_g;
    if (!g_data)
        return -ENODEV;
    raw     = mpu6050_read_pair(g_data, REG_ACCEL_ZOUT_H);
    milli_g = (raw * 1000) / ACCEL_SENSITIVITY;
    return sprintf(buf, "%d.%03d\n", milli_g / 1000, abs(milli_g % 1000));
}

static ssize_t gyro_x_show(struct kobject *kobj,
                             struct kobj_attribute *attr, char *buf)
{
    s16 raw;
    int milli_dps;
    if (!g_data)
        return -ENODEV;
    raw       = mpu6050_read_pair(g_data, REG_GYRO_XOUT_H);
    milli_dps = (raw * 1000) / GYRO_SENSITIVITY;
    return sprintf(buf, "%d.%03d\n", milli_dps / 1000, abs(milli_dps % 1000));
}

static ssize_t gyro_y_show(struct kobject *kobj,
                             struct kobj_attribute *attr, char *buf)
{
    s16 raw;
    int milli_dps;
    if (!g_data)
        return -ENODEV;
    raw       = mpu6050_read_pair(g_data, REG_GYRO_YOUT_H);
    milli_dps = (raw * 1000) / GYRO_SENSITIVITY;
    return sprintf(buf, "%d.%03d\n", milli_dps / 1000, abs(milli_dps % 1000));
}

static ssize_t gyro_z_show(struct kobject *kobj,
                             struct kobj_attribute *attr, char *buf)
{
    s16 raw;
    int milli_dps;
    if (!g_data)
        return -ENODEV;
    raw       = mpu6050_read_pair(g_data, REG_GYRO_ZOUT_H);
    milli_dps = (raw * 1000) / GYRO_SENSITIVITY;
    return sprintf(buf, "%d.%03d\n", milli_dps / 1000, abs(milli_dps % 1000));
}

static struct kobj_attribute accel_x_attr = __ATTR(accel_x, 0444, accel_x_show, NULL);
static struct kobj_attribute accel_y_attr = __ATTR(accel_y, 0444, accel_y_show, NULL);
static struct kobj_attribute accel_z_attr = __ATTR(accel_z, 0444, accel_z_show, NULL);
static struct kobj_attribute gyro_x_attr  = __ATTR(gyro_x,  0444, gyro_x_show,  NULL);
static struct kobj_attribute gyro_y_attr  = __ATTR(gyro_y,  0444, gyro_y_show,  NULL);
static struct kobj_attribute gyro_z_attr  = __ATTR(gyro_z,  0444, gyro_z_show,  NULL);

static int mpu6050_probe(struct i2c_client *client)
{
    int ret;
    struct mpu6050_data *data;

    i2c_smbus_write_byte_data(client, REG_PWR_MGMT_1, 0x00);
    msleep(10);

    data = devm_kzalloc(&client->dev, sizeof(*data), GFP_KERNEL);
    if (!data)
        return -ENOMEM;

    data->client = client;
    g_data = data;
    i2c_set_clientdata(client, data);

    data->kobj = kobject_create_and_add("mpu6050", kernel_kobj);
    if (!data->kobj)
        return -ENOMEM;

    ret = sysfs_create_file(data->kobj, &accel_x_attr.attr);
    if (ret) { kobject_put(data->kobj); return ret; }

    ret = sysfs_create_file(data->kobj, &accel_y_attr.attr);
    if (ret) { kobject_put(data->kobj); return ret; }

    ret = sysfs_create_file(data->kobj, &accel_z_attr.attr);
    if (ret) { kobject_put(data->kobj); return ret; }

    ret = sysfs_create_file(data->kobj, &gyro_x_attr.attr);
    if (ret) { kobject_put(data->kobj); return ret; }

    ret = sysfs_create_file(data->kobj, &gyro_y_attr.attr);
    if (ret) { kobject_put(data->kobj); return ret; }

    ret = sysfs_create_file(data->kobj, &gyro_z_attr.attr);
    if (ret) { kobject_put(data->kobj); return ret; }

    dev_info(&client->dev, "MPU6050 ready\n");
    dev_info(&client->dev, "Read: cat /sys/kernel/mpu6050/accel_x\n");
    return 0;
}

static void mpu6050_remove(struct i2c_client *client)
{
    struct mpu6050_data *data = i2c_get_clientdata(client);
    sysfs_remove_file(data->kobj, &accel_x_attr.attr);
    sysfs_remove_file(data->kobj, &accel_y_attr.attr);
    sysfs_remove_file(data->kobj, &accel_z_attr.attr);
    sysfs_remove_file(data->kobj, &gyro_x_attr.attr);
    sysfs_remove_file(data->kobj, &gyro_y_attr.attr);
    sysfs_remove_file(data->kobj, &gyro_z_attr.attr);
    g_data = NULL;
    kobject_put(data->kobj);
    dev_info(&client->dev, "MPU6050 removed\n");
}

static const struct i2c_device_id mpu6050_id[] = {
    { "mpu6050", 0 },
    { }
};
MODULE_DEVICE_TABLE(i2c, mpu6050_id);

static const struct of_device_id mpu6050_of_match[] = {
    { .compatible = "invensense,mpu6050" },
    { }
};
MODULE_DEVICE_TABLE(of, mpu6050_of_match);

static struct i2c_driver mpu6050_driver = {
    .driver = {
        .name           = DRIVER_NAME,
        .of_match_table = mpu6050_of_match,
    },
    .probe    = mpu6050_probe,
    .remove   = mpu6050_remove,
    .id_table = mpu6050_id,
};

module_i2c_driver(mpu6050_driver);
