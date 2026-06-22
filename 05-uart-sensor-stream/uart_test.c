#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#define UART_PORT   "/dev/ttyAMA0"
#define BAUD_RATE   B115200

int main()
{
    int fd;
    struct termios options;
    char tx_buf[] = "SENSOR:REQUEST\n";
    char rx_buf[256];
    int bytes_read;

    /* Step 1 — Open UART port */
    fd = open(UART_PORT, O_RDWR | O_NOCTTY);
    if (fd < 0) {
        perror("Failed to open UART");
        return -1;
    }
    printf("UART opened successfully\n");

    /* Step 2 — Configure UART */
    tcgetattr(fd, &options);
    cfsetispeed(&options, BAUD_RATE);
    cfsetospeed(&options, BAUD_RATE);
    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_cc[VMIN]  = 0;
    options.c_cc[VTIME] = 10;
    tcsetattr(fd, TCSANOW, &options);
    printf("UART configured: 115200 8N1\n");

    /* Step 3 — Send data */
    write(fd, tx_buf, strlen(tx_buf));
    printf("Sent: %s", tx_buf);

    /* Step 4 — Read response */
    sleep(1);
    bytes_read = read(fd, rx_buf, sizeof(rx_buf) - 1);
    if (bytes_read > 0) {
        rx_buf[bytes_read] = '\0';
        printf("Received: %s\n", rx_buf);
    } else {
        printf("No response received\n");
    }

    close(fd);
    return 0;
}

