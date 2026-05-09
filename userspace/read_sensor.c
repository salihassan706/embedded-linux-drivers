/*
 * read_sensor.c - User-space sensor reader
 * Reads sensor data from sysfs interface
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(void)
{
    int fd;
    char buffer[256];
    ssize_t bytes_read;
    
    fd = open("/sys/kernel/sensor_driver/sensor_value", O_RDONLY);
    if (fd < 0) {
        perror("open");
        return EXIT_FAILURE;
    }
    
    bytes_read = read(fd, buffer, sizeof(buffer) - 1);
    if (bytes_read < 0) {
        perror("read");
        close(fd);
        return EXIT_FAILURE;
    }
    
    buffer[bytes_read] = '\0';
    printf("Sensor value: %s", buffer);
    
    close(fd);
    return EXIT_SUCCESS;
}
