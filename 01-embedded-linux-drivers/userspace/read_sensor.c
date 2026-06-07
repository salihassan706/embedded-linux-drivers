#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    char buffer[16];
    float temp;
    
    // 1. Open the sysfs attribute created by your driver
    int fd = open("/sys/kernel/sysfs_driver/temperature", O_RDONLY);
    if (fd == -1) {
        perror("Error opening sysfs node");
        return 1;
    }

    // 2. Read the value (stored as a string in sysfs)
    ssize_t bytesRead = read(fd, buffer, sizeof(buffer) - 1);
    if (bytesRead == -1) {
        perror("Error reading");
        close(fd);
        return 1;
    }
    buffer[bytesRead] = '\0';

    // 3. Parse the string into a float
    sscanf(buffer, "%f", &temp);
    printf("Current temperature: %.2f°C\n", temp);

    close(fd);
    return 0;
}
