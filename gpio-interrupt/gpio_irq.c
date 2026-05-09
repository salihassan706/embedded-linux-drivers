#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/gpio/consumer.h>
#include <linux/platform_device.h>

#define DRIVER_NAME "gpio_irq"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sohail");
MODULE_DESCRIPTION("GPIO interrupt driver");

static int irq_number;
static int interrupt_count = 0;
static struct gpio_desc *gpio_button;

static irqreturn_t gpio_irq_handler(int irq, void *dev_id)
{
    interrupt_count++;
    printk(KERN_INFO "%s: interrupt fired! count = %d\n",
           DRIVER_NAME, interrupt_count);
    return IRQ_HANDLED;
}

static int __init gpio_irq_init(void)
{
    int ret;

    // Request GPIO using BCM number directly
    gpio_button = gpio_to_desc(17);
    if (!gpio_button) {
        printk(KERN_ERR "%s: failed to get GPIO descriptor\n", DRIVER_NAME);
        return -ENODEV;
    }

    // Get IRQ number
    irq_number = gpiod_to_irq(gpio_button);
    if (irq_number < 0) {
        printk(KERN_ERR "%s: failed to get IRQ number: %d\n",
               DRIVER_NAME, irq_number);
        return irq_number;
    }

    printk(KERN_INFO "%s: GPIO 17 mapped to IRQ %d\n",
           DRIVER_NAME, irq_number);

    ret = request_irq(irq_number,
                      gpio_irq_handler,
                      IRQF_TRIGGER_FALLING,
                      DRIVER_NAME,
                      NULL);
    if (ret) {
        printk(KERN_ERR "%s: failed to request IRQ %d: %d\n",
               DRIVER_NAME, irq_number, ret);
        return ret;
    }

    printk(KERN_INFO "%s: loaded. Connect GPIO17 pin 11 to GND "
           "pin 9 to trigger\n", DRIVER_NAME);
    return 0;
}

static void __exit gpio_irq_exit(void)
{
    free_irq(irq_number, NULL);
    printk(KERN_INFO "%s: unloaded. Total interrupts: %d\n",
           DRIVER_NAME, interrupt_count);
}

module_init(gpio_irq_init);
module_exit(gpio_irq_exit);
