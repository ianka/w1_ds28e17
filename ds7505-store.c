/*
 * ds7505-store - DS7505 test program. Store configuration into EEPROM.
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

int main(int argc, char* argv[]) {
	struct i2c_msg i2c_msgs[2];
	struct i2c_rdwr_ioctl_data i2c_transfer;
	unsigned int address;
	uint8_t config[1]; 
	int fd;

	/* Parse arguments. */
	if (argc != 3) {
		fprintf(stderr, "USAGE: ds7505-store <i2c-dev> <i2c slave address>\n");
		return 127;
	}	
	sscanf(argv[2], "0x%x", &address);
	if (address < 0x08 || address > 0x77) {
		fprintf(stderr, "ds7505-store: i2c slave address has to be in the range 0x08..0x77.\n");
		return 127;
	}

	/* Open I2C device. */
	fd = open(argv[1], O_RDWR);
	if (fd < 0) {
		perror("ds7505-store: i2c device open");
		return 2;
	}

	/* Setup store command. */
	i2c_msgs[0].addr   = address;
	i2c_msgs[0].flags  = 0;
	i2c_msgs[0].len    = 1;
	i2c_msgs[0].buf    = "\x48";

	i2c_transfer.msgs  = i2c_msgs;
	i2c_transfer.nmsgs = 1;

	if (ioctl(fd, I2C_RDWR, &i2c_transfer) < 0) {
		perror("ds7505-store: i2c store command");
		return 2;
	};

	/* Loop until EEPROM has been written. */
	do {
		/* Config read. */
		i2c_msgs[0].addr   = address;
		i2c_msgs[0].flags  = 0;
		i2c_msgs[0].len    = 1;
		i2c_msgs[0].buf    = "\x01";

		i2c_msgs[1].addr   = address;
		i2c_msgs[1].flags  = I2C_M_RD;
		i2c_msgs[1].len    = 1;
		i2c_msgs[1].buf    = (char*)&config;

		i2c_transfer.msgs  = i2c_msgs;
		i2c_transfer.nmsgs = 2;

		if (ioctl(fd, I2C_RDWR, &i2c_transfer) < 0) {
			perror("ds7505-readtemp: i2c config read");
			return 2;
		};
	} while (config[0] & 0x80);

	/* Finish. */
	close(fd);
	return 0;
}
