#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <global.h>
#include <md5.h>
#include <common.h>
#include <log.h>

void checksum(void *out, void *in, unsigned long len)
{
	uint32_t *src = in;
	char *init = "*BITMAIN-SOPHON*";
	uint32_t result[4];
	memcpy(result, init, sizeof(result));
	unsigned long block = len >> 4;
	unsigned long left = len & 0xf;
	unsigned long i, j;
	for (i = 0; i < block; ++i, src += 4) {
		for (j = 0; j < 4; ++j) {
			result[j] ^= src[j];
		}
	}
	for (i = 0; i < left; ++i) {
		((uint8_t *)result)[i] ^= ((uint8_t *)src)[i];
	}
	memcpy(out, result, sizeof(result));
}

int load_file(struct comp *comp, const char *file)
{
	int fd;
	int err = -1;
	struct stat stat;

	memset(comp, 0x00, sizeof(*comp));

	comp->file = file;

	fd = open(file, O_RDONLY);
	if (fd < 0) {
		error("cannot load file %s\n", file);
		return -1;
	}

	err = fstat(fd, &stat);
	if (err < 0) {
		error("cannot stat file %s\n", file);
		goto close_file;
	}
	comp->size = stat.st_size;
	comp->buf = malloc(comp->size);
	if (comp->buf == NULL) {
		error("cannot malloc buffer for file %s\n", file);
		err = -1;
		goto close_file;
	}
	if (read(fd, comp->buf, comp->size) != comp->size) {
		error("cannot load hole file %s\n", file);
		err = -1;
		goto close_file;
	}

	checksum(comp->efie.checksum, comp->buf, comp->size);
	comp->efie.length = comp->size;

	err = 0;
close_file:
	close(fd);
	return err;
}

void unload_file(struct comp *comp)
{
	if (comp->buf)
		free(comp->buf);
}

int store_file(void *buf, unsigned long size, const char *file)
{
	int fd;
	int err = -1;

	fd = open(file, O_RDWR | O_CREAT,
		  S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
	if (fd < 0) {
		error("cannot open file %s\n", file);
		perror("");
		return -1;
	}

	if (write(fd, buf, size) != size) {
		error("cannot store hole file %s\n", file);
		err = -1;
		goto close_file;
	}
	err = 0;
close_file:
	close(fd);
	return err;
}

void print_efie(struct efie *efie)
{
	printf("[DEBUG]: ");
	if (efie->padding[0])
		printf("name %s ", efie->padding);
	printf("offset 0x%08x length 0x%08x checksum ",
	       efie->offset, efie->length);
	int i;
	for (i = 0; i < 16; ++i)
		printf("%02x", efie->checksum[i]);
	printf("\n");
}

