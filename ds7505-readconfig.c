/*
 * ds7505-readconfig - DS7505 test program. Reads out chip configuration.
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
	struct i2c_msg i2c_msgs[1];
	struct i2c_rdwr_ioctl_data i2c_transfer;
	unsigned int address;
	uint8_t config[1]; 
	uint8_t tos[2];
	uint8_t thyst[2];
	int fd;

	/* Parse arguments. */
	if (argc != 3) {
		fprintf(stderr, "USAGE: ds7505-readconfig <i2c-dev> <i2c slave address>\n");
		return 127;
	}	
	sscanf(argv[2], "0x%x", &address);
	if (address < 0x08 || address > 0x77) {
		fprintf(stderr, "ds7505-readconfig: i2c slave address has to be in the range 0x08..0x77.\n");
		return 127;
	}

	/* Open I2C device. */
	fd = open(argv[1], O_RDWR);
	if (fd < 0) {
		perror("ds7505-readconfig: i2c device open");
		return 2;
	}

	/* Select config register. */
	i2c_msgs[0].addr   = address;
	i2c_msgs[0].flags  = 0;
	i2c_msgs[0].len    = 1;
	i2c_msgs[0].buf    = "\x01";

	i2c_transfer.msgs  = i2c_msgs;
	i2c_transfer.nmsgs = 1;

	if (ioctl(fd, I2C_RDWR, &i2c_transfer) < 0) {
		perror("ds7505-readconfig: i2c config select");
		return 2;
	};

	/* This is done in two separate transfers to ensure there is no repeated
	 * start condition, as the DS7505 doesn't support that. */

	/* Config read. */
	i2c_msgs[0].addr   = address;
	i2c_msgs[0].flags  = I2C_M_RD;
	i2c_msgs[0].len    = 1;
	i2c_msgs[0].buf    = (char*)&config;

	i2c_transfer.msgs  = i2c_msgs;
	i2c_transfer.nmsgs = 1;

	if (ioctl(fd, I2C_RDWR, &i2c_transfer) < 0) {
		perror("ds7505-readconfig: i2c config read");
		return 2;
	};

	/* Select t_os register. */
	i2c_msgs[0].addr   = address;
	i2c_msgs[0].flags  = 0;
	i2c_msgs[0].len    = 1;
	i2c_msgs[0].buf    = "\x03";

	i2c_transfer.msgs  = i2c_msgs;
	i2c_transfer.nmsgs = 1;

	if (ioctl(fd, I2C_RDWR, &i2c_transfer) < 0) {
		perror("ds7505-readconfig: i2c t_os select");
		return 2;
	};

	/* This is done in two separate transfers to ensure there is no repeated
	 * start condition, as the DS7505 doesn't support that. */

	/* t_os read. */
	i2c_msgs[0].addr   = address;
	i2c_msgs[0].flags  = I2C_M_RD;
	i2c_msgs[0].len    = 2;
	i2c_msgs[0].buf    = (char*)&tos;

	i2c_transfer.msgs  = i2c_msgs;
	i2c_transfer.nmsgs = 1;

	if (ioctl(fd, I2C_RDWR, &i2c_transfer) < 0) {
		perror("ds7505-readconfig: i2c t_os read");
		return 2;
	};


	/* Select t_hyst register. */
	i2c_msgs[0].addr   = address;
	i2c_msgs[0].flags  = 0;
	i2c_msgs[0].len    = 1;
	i2c_msgs[0].buf    = "\x02";

	i2c_transfer.msgs  = i2c_msgs;
	i2c_transfer.nmsgs = 1;

	if (ioctl(fd, I2C_RDWR, &i2c_transfer) < 0) {
		perror("ds7505-readconfig: i2c t_hyst select");
		return 2;
	};

	/* This is done in two separate transfers to ensure there is no repeated
	 * start condition, as the DS7505 doesn't support that. */

	/* t_hyst read. */
	i2c_msgs[0].addr   = address;
	i2c_msgs[0].flags  = I2C_M_RD;
	i2c_msgs[0].len    = 2;
	i2c_msgs[0].buf    = (char*)&thyst;

	i2c_transfer.msgs  = i2c_msgs;
	i2c_transfer.nmsgs = 1;

	if (ioctl(fd, I2C_RDWR, &i2c_transfer) < 0) {
		perror("ds7505-readconfig: i2c t_hyst read");
		return 2;
	};


	/* Print result. */
	printf("0x%02x %.4f %.4f\n",
		config[0],
		((float)((int16_t) (tos[0] << 8 | tos[1]))) / 256,
		((float)((int16_t) (thyst[0] << 8 | thyst[1]))) / 256);

	/* Finish. */
	close(fd);
	return 0;
}
