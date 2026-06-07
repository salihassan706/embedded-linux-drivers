#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "mydevice"
#define BUFFER_SIZE 256

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sohail");
MODULE_DESCRIPTION("Simple character device driver");

static int    major_number;
static char   device_buffer[BUFFER_SIZE];
static int    buffer_len = 0;

// Write these four functions:
static int dev_open(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO "mydevice: device opened\n");
    return 0;
}

static int dev_release(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO "mydevice: device closed\n");
    return 0;
}

static ssize_t dev_read(struct file *filep, char __user *user_buf,
                         size_t len, loff_t *offset)
{
    int bytes_to_send = buffer_len;

    if (bytes_to_send == 0) {
        printk(KERN_INFO "mydevice: nothing to read\n");
        return 0;
    }

    // copy_to_user: safely copy from kernel buffer to user space
    // Returns number of bytes that could NOT be copied (0 = success)
    if (copy_to_user(user_buf, device_buffer, bytes_to_send)) {
        printk(KERN_ERR "mydevice: failed to send data to user\n");
        return -EFAULT;
    }

    printk(KERN_INFO "mydevice: sent %d bytes to user\n", bytes_to_send);
    buffer_len = 0;    // clear after read
    return bytes_to_send;
}

static ssize_t dev_write(struct file *filep, const char __user *user_buf,
                          size_t len, loff_t *offset)
{
    // copy_from_user: safely copy from user space to kernel buffer
    if (len > BUFFER_SIZE - 1) len = BUFFER_SIZE - 1;

    if (copy_from_user(device_buffer, user_buf, len)) {
        printk(KERN_ERR "mydevice: failed to receive data from user\n");
        return -EFAULT;
    }

    device_buffer[len] = '\0';
    buffer_len = len;
    printk(KERN_INFO "mydevice: received %zu bytes from user: %s\n",
           len, device_buffer);
    return len;
}
static struct file_operations fops = {
    .open    = dev_open,
    .read    = dev_read,
    .write   = dev_write,
    .release = dev_release,
};
static int __init chardev_init(void)
{
    // Register character device — kernel assigns major number
    major_number = register_chrdev(0, DEVICE_NAME, &fops);

    if (major_number < 0) {
        printk(KERN_ERR "mydevice: failed to register, error %d\n",
               major_number);
        return major_number;
    }

    printk(KERN_INFO "mydevice: registered with major number %d\n",
           major_number);
    printk(KERN_INFO "mydevice: create device with:\n");
    printk(KERN_INFO "  sudo mknod /dev/mydevice c %d 0\n", major_number);
    return 0;
}

static void __exit chardev_exit(void)
{
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO "mydevice: unregistered\n");
}

module_init(chardev_init);
module_exit(chardev_exit);
