/*
 * ds7505-configure - DS7505 test program. Sets up the chip.
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
	struct i2c_msg i2c_msgs[3];
	struct i2c_rdwr_ioctl_data i2c_transfer;
	unsigned int address;
	unsigned int s_config;
	float s_tos;
	float s_thyst;
	uint8_t config[2]; 
	uint8_t tos[3];
	uint8_t thyst[3];
	int fd;

	/* Parse arguments. */
	if (argc < 4 || argc == 5 || argc > 6) {
		fprintf(stderr, "USAGE: ds7505-configure <i2c-dev> <i2c slave address> <configuration value> [<t_os> <t_hyst>]\n");
		return 127;
	}	
	sscanf(argv[2], "0x%x", &address);
	if (address < 0x08 || address > 0x77) {
		fprintf(stderr, "ds7505-configure: i2c slave address has to be in the range 0x08..0x77.\n");
		return 127;
	}
	sscanf(argv[3], "0x%x", &s_config);
	if (s_config > 0x7f) {
		fprintf(stderr, "ds7505-configure: configuration value has to be in the range 0x00..0x7f.\n");
		return 127;
	}
	if (argc != 4) {
		sscanf(argv[4], "%f", &s_tos);
		if (s_tos < -55 || s_tos > 125) {
			fprintf(stderr, "ds7505-configure: t_os has to be in the range -55..125.\n");
			return 127;
		}
		sscanf(argv[5], "%f", &s_thyst);
		if (s_thyst < -55 || s_thyst > 125) {
			fprintf(stderr, "ds7505-configure: t_hyst has to be in the range -55..125.\n");
			return 127;
		}
		if (s_thyst >= s_tos) {
			fprintf(stderr, "ds7505-configure: t_hyst has to be smaller than t_os.\n");
			return 127;
		}
	}

	/* Set up messages from parameters. */
	config[0] = 0x01;  /* configuration register address. */
	config[1] = s_config;

	if (argc != 4) {
		tos[0]    = 0x03;  /* t_os register address. */
		tos[1]    = ((int16_t)(s_tos * 256)) >> 8;
		tos[2]    = (int16_t)(s_tos * 256);

		thyst[0]  = 0x02;  /* t_hyst register address. */
		thyst[1]  = ((int16_t)(s_thyst * 256)) >> 8;
		thyst[2]  = (int16_t)(s_thyst * 256);
	}

	/* Open I2C device. */
	fd = open(argv[1], O_RDWR);

	/* Select configuration register, write value. */
	i2c_msgs[0].addr  = address;
	i2c_msgs[0].flags = 0;
	i2c_msgs[0].len   = 2;
	i2c_msgs[0].buf   = (char*)config;

	if (argc != 4) {
		/* Configure t_os and t_hyst, too. */
		i2c_msgs[1].addr  = address;
		i2c_msgs[1].flags = 0;
		i2c_msgs[1].len   = 3;
		i2c_msgs[1].buf   = (char*)tos;

		i2c_msgs[2].addr  = address;
		i2c_msgs[2].flags = 0;
		i2c_msgs[2].len   = 3;
		i2c_msgs[2].buf   = (char*)thyst;
	}

	i2c_transfer.msgs  = i2c_msgs;
	i2c_transfer.nmsgs = (argc != 4) ? 3 : 1;

	if (ioctl(fd, I2C_RDWR, &i2c_transfer) < 0) {
		perror("ds7505-configure: configuration/t_os/thyst set");
		return 2;
	};

	/* Finish. */
	close(fd);
	return 0;
}
