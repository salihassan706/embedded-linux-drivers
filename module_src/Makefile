obj-m += hello_arm64.o

all:
	make -C ../linux-6.1 M=$(PWD) \
		ARCH=arm64 \
		CROSS_COMPILE=aarch64-linux-gnu- \
		modules

clean:
	make -C ../linux-6.1 M=$(PWD) \
		ARCH=arm64 \
		CROSS_COMPILE=aarch64-linux-gnu- \
		clean
