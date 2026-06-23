#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/spi/spi.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/string.h>

#define DRIVER_NAME  "spi_loopback"
#define LOOPBACK_MSG "HELLO FROM SPI"
#define MSG_LEN      14

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sohail Ali Hassan Abbasi");
MODULE_DESCRIPTION("SPI loopback test driver");

struct spi_loopback_data {
    struct spi_device *spi;
    struct kobject *kobj;
    char last_response[MSG_LEN + 1];
};

static struct spi_loopback_data *g_data;

static int spi_do_loopback(struct spi_loopback_data *data)
{
    u8 tx_buf[MSG_LEN];
    u8 rx_buf[MSG_LEN];
    struct spi_transfer t = {
        .tx_buf = tx_buf,
        .rx_buf = rx_buf,
        .len    = MSG_LEN,
    };
    struct spi_message m;

    memcpy(tx_buf, LOOPBACK_MSG, MSG_LEN);
    memset(rx_buf, 0, MSG_LEN);

    spi_message_init(&m);
    spi_message_add_tail(&t, &m);
    spi_sync(data->spi, &m);

    memcpy(data->last_response, rx_buf, MSG_LEN);
    data->last_response[MSG_LEN] = '\0';

    return 0;
}
static ssize_t loopback_show(struct kobject *kobj,
                              struct kobj_attribute *attr, char *buf)
{
    if (!g_data)
        return -ENODEV;
    spi_do_loopback(g_data);
    return sprintf(buf, "%s\n", g_data->last_response);
}

static struct kobj_attribute loopback_attr =
    __ATTR(loopback, 0444, loopback_show, NULL);

static int spi_loopback_probe(struct spi_device *spi)
{
    int ret;
    struct spi_loopback_data *data;

    data = devm_kzalloc(&spi->dev, sizeof(*data), GFP_KERNEL);
    if (!data)
        return -ENOMEM;

    data->spi = spi;
    /* Configure SPI */
    spi->max_speed_hz = 500000;
    spi->mode = SPI_MODE_0;
    spi->bits_per_word = 8;
    spi_setup(spi);
    g_data = data;
    spi_set_drvdata(spi, data);

    data->kobj = kobject_create_and_add("spi_loopback", kernel_kobj);
    if (!data->kobj)
        return -ENOMEM;

    ret = sysfs_create_file(data->kobj, &loopback_attr.attr);
    if (ret) {
        kobject_put(data->kobj);
        return ret;
    }

    dev_info(&spi->dev, "SPI loopback ready\n");
    dev_info(&spi->dev, "Read: cat /sys/kernel/spi_loopback/loopback\n");
    return 0;
}
static void spi_loopback_remove(struct spi_device *spi)
{
    struct spi_loopback_data *data = spi_get_drvdata(spi);
    sysfs_remove_file(data->kobj, &loopback_attr.attr);
    g_data = NULL;
    kobject_put(data->kobj);
    dev_info(&spi->dev, "SPI loopback removed\n");
}
static const struct spi_device_id spi_loopback_id[] = {
    { "spi_loopback", 0 },
    { }
};
MODULE_DEVICE_TABLE(spi, spi_loopback_id);

static struct spi_driver spi_loopback_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .probe    = spi_loopback_probe,
    .remove   = spi_loopback_remove,
    .id_table = spi_loopback_id,
};

module_spi_driver(spi_loopback_driver);
