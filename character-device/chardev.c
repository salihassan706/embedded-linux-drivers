#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>

#define DEVICE_NAME "mydevice"
#define BUFFER_SIZE 256

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sohail");
MODULE_DESCRIPTION("Character device driver with locking");

static int    major_number;
static char   device_buffer[BUFFER_SIZE];
static int    buffer_len = 0;

// Declare the mutex at module level
static DEFINE_MUTEX(mydevice_mutex);

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
    int bytes_to_send;

    // Lock before touching shared data
    if (mutex_lock_interruptible(&mydevice_mutex))
        return -ERESTARTSYS;

    bytes_to_send = buffer_len;

    if (bytes_to_send == 0) {
        mutex_unlock(&mydevice_mutex);
        printk(KERN_INFO "mydevice: nothing to read\n");
        return 0;
    }

    if (copy_to_user(user_buf, device_buffer, bytes_to_send)) {
        mutex_unlock(&mydevice_mutex);
        printk(KERN_ERR "mydevice: failed to send data to user\n");
        return -EFAULT;
    }

    printk(KERN_INFO "mydevice: sent %d bytes to user\n", bytes_to_send);
    buffer_len = 0;

    // Unlock after done with shared data
    mutex_unlock(&mydevice_mutex);
    return bytes_to_send;
}

static ssize_t dev_write(struct file *filep, const char __user *user_buf,
                          size_t len, loff_t *offset)
{
    if (mutex_lock_interruptible(&mydevice_mutex))
        return -ERESTARTSYS;

    if (len > BUFFER_SIZE - 1) len = BUFFER_SIZE - 1;

    if (copy_from_user(device_buffer, user_buf, len)) {
        mutex_unlock(&mydevice_mutex);
        printk(KERN_ERR "mydevice: failed to receive data\n");
        return -EFAULT;
    }

    device_buffer[len] = '\0';
    buffer_len = len;
    printk(KERN_INFO "mydevice: received %zu bytes: %s\n",
           len, device_buffer);

    mutex_unlock(&mydevice_mutex);
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
    major_number = register_chrdev(0, DEVICE_NAME, &fops);

    if (major_number < 0) {
        printk(KERN_ERR "mydevice: failed to register\n");
        return major_number;
    }

    printk(KERN_INFO "mydevice: registered with major number %d\n",
           major_number);
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
