#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/ktime.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>


static ktime_t last_interrupt_time;


#define DRIVER_NAME  "gpio_irq"
#define GPIO_PIN     529        // BCM GPIO 17, physical pin 11

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sohail");
MODULE_DESCRIPTION("GPIO interrupt driver");

static int irq_number;
static int interrupt_count = 0;
static int gpio_irq_proc_show(struct seq_file *m, void *v)
{
    seq_printf(m, "GPIO IRQ Driver Status\n");
    seq_printf(m, "IRQ number: %d\n", irq_number);
    seq_printf(m, "Interrupt count: %d\n", interrupt_count);
    return 0;
}

static int gpio_irq_proc_open(struct inode *inode, struct file *file)
{
    return single_open(file, gpio_irq_proc_show, NULL);
}

static const struct proc_ops gpio_irq_proc_ops = {
    .proc_open    = gpio_irq_proc_open,
    .proc_read    = seq_read,
    .proc_lseek   = seq_lseek,
    .proc_release = single_release,
};
static irqreturn_t gpio_irq_handler(int irq, void *dev_id)
{
    last_interrupt_time = ktime_get();

    interrupt_count++;
    printk(KERN_INFO "%s: interrupt fired! count = %d\n",
           DRIVER_NAME, interrupt_count);
    ktime_t now = ktime_get();
    s64 delta_us = ktime_to_us(ktime_sub(now, last_interrupt_time));
    printk(KERN_INFO "%s: interrupt fired. delta = %lld us\n",
           DRIVER_NAME, delta_us);
    last_interrupt_time = now;
    return IRQ_HANDLED;
}

static int __init gpio_irq_init(void)
{
    int ret;

    // Request GPIO pin
    ret = gpio_request(GPIO_PIN, DRIVER_NAME);
    if (ret) {
        printk(KERN_ERR "%s: failed to request GPIO %d\n",
               DRIVER_NAME, GPIO_PIN);
        return ret;
    }

    // Set as input
    ret = gpio_direction_input(GPIO_PIN);
    if (ret) {
        printk(KERN_ERR "%s: failed to set GPIO direction\n",
               DRIVER_NAME);
        gpio_free(GPIO_PIN);
        return ret;
    }

    // Get IRQ number for this GPIO
    irq_number = gpio_to_irq(GPIO_PIN);
    if (irq_number < 0) {
        printk(KERN_ERR "%s: failed to get IRQ number\n", DRIVER_NAME);
        gpio_free(GPIO_PIN);
        return irq_number;
    }
    proc_create("gpio_irq", 0, NULL, &gpio_irq_proc_ops);
    printk(KERN_INFO "%s: GPIO 17 (kernel %d) mapped to IRQ %d\n",
       DRIVER_NAME, GPIO_PIN, irq_number);

    // Register interrupt handler — fires on falling edge
    ret = request_irq(irq_number,
                      gpio_irq_handler,
                      IRQF_TRIGGER_FALLING,
                      DRIVER_NAME,
                      NULL);
    if (ret) {
        printk(KERN_ERR "%s: failed to request IRQ\n", DRIVER_NAME);
        gpio_free(GPIO_PIN);
        return ret;
    }

    printk(KERN_INFO "%s: loaded. Connect GPIO17 (pin 11) to GND "
           "to trigger interrupt\n", DRIVER_NAME);
    return 0;
}

static void __exit gpio_irq_exit(void)
{
    free_irq(irq_number, NULL);
    gpio_free(GPIO_PIN);
    printk(KERN_INFO "%s: unloaded. Total interrupts: %d\n",
           DRIVER_NAME, interrupt_count);
    remove_proc_entry("gpio_irq", NULL);
}

module_init(gpio_irq_init);
module_exit(gpio_irq_exit);
