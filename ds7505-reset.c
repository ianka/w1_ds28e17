/*
 * ds7505-reset - DS7505 test program. Performs a power-on reset.
 *
 * Copyright (c) 2016 Jan Kandziora <jjj@gmx.de>
 *
 * This source code is licensed under the GNU General Public License,
 * Version 2. See the file COPYING for more details.
 *
 */

#include <stdio.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <inttypes.h>
#include <linux/i2c-dev.h>
#include <errno.h>

int main(int argc, char* argv[]) {
	struct i2c_msg i2c_msgs[1];
	struct i2c_rdwr_ioctl_data i2c_transfer;
	unsigned int address;
	int fd;

	/* Parse arguments. */
	if (argc != 3) {
		fprintf(stderr, "USAGE: ds7505-reset <i2c-dev> <i2c slave address>\n");
		return 127;
	}	
	sscanf(argv[2], "0x%x", &address);
	if (address < 0x08 || address > 0x77) {
		fprintf(stderr, "ds7505-reset: i2c slave address has to be in the range 0x08..0x77.\n");
		return 127;
	}

	/* Open I2C device. */
	fd = open(argv[1], O_RDWR);
	if (fd < 0) {
		perror("ds7505-reset: i2c device open");
		return 2;
	}

	/* Setup reset command. */
	i2c_msgs[0].addr   = address;
	i2c_msgs[0].flags  = 0;
	i2c_msgs[0].len    = 1;
	i2c_msgs[0].buf    = "\x54";

	i2c_transfer.msgs  = i2c_msgs;
	i2c_transfer.nmsgs = 1;

	if (ioctl(fd, I2C_RDWR, &i2c_transfer) < 0) {
		/* This transfer always fails with -EIO, as the I2C slave chip resets. */
		if (errno != EIO) {
			perror("ds7505-recall: i2c reset command");
			return 2;
		}	
	};

	/* Finish. */
	close(fd);
	return 0;
}
