#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#define SENSOR_WIDTH           54
#define SENSOR_HEIGHT          128
#define SENSOR_BIT_WIDE        12
#define SENSOR_BYTES_PERPIXEL (((SENSOR_BIT_WIDE + 0x07) & (~0x07)) >> 3)
#define SENSOR_BUFFER_LENGTH (SENSOR_WIDTH * SENSOR_HEIGHT * SENSOR_BYTES_PERPIXEL)

int cfp_get_image(char *image_path, char *image_buffer)
{
    int fd = -1;
    int filesize = -1;
    char *bmpBuf = NULL;
    fd = open(image_path, O_RDONLY, 0700);
    if (fd < 0)
    {
        return -1;
    }
    filesize = lseek(fd, 0L, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    bmpBuf = (char *)malloc(filesize);
    read(fd, bmpBuf, filesize);
    close(fd);
    char *ptr = bmpBuf;
    int i = 0;
    for (; i < SENSOR_HEIGHT; ++i)
    {
        memcpy(image_buffer + i * SENSOR_WIDTH, ptr + (SENSOR_HEIGHT - i - 1) * SENSOR_WIDTH, SENSOR_WIDTH);
    }
    free(bmpBuf);
    return 0;
}

int fp_SaveGrayBitmap(const char *FilePath, unsigned char *pData, int row, int colume)
{
	/*Check parameter*/
	int fd = 0;
	unsigned char grayBitmapHeader[1078] = {0};
	unsigned char pad[4] = {0};
	int colume_t = 0;
	int row_t = 0;
	int i = 0;
	if (NULL == pData)
	{
		return -1;
	}
	fd = open(FilePath, O_CREAT | O_RDWR);
	if (0 == fd)
	{
		return -1;
	}
	/*Gray bitmap file header.*/

	//fileOutStream.clear();
	{
		colume_t = (colume + 3) & 0xFC;
		row_t = (row + 3) & 0xFC;
		grayBitmapHeader[0] = 0x42;
		grayBitmapHeader[1] = 0x4d;
		grayBitmapHeader[2] = 0x36;
		grayBitmapHeader[3] = 0x28;
		grayBitmapHeader[4] = 0x00;
		grayBitmapHeader[5] = 0x00;
		grayBitmapHeader[6] = 0x00;
		grayBitmapHeader[7] = 0x00;
		grayBitmapHeader[8] = 0x00;
		grayBitmapHeader[9] = 0x00;
		grayBitmapHeader[10] = 0x36;
		grayBitmapHeader[11] = 0x04;
		grayBitmapHeader[12] = 0x00;
		grayBitmapHeader[13] = 0x00;
		grayBitmapHeader[14] = 0x28;
		grayBitmapHeader[15] = 0x00;
		grayBitmapHeader[16] = 0x00;
		grayBitmapHeader[17] = 0x00;

		grayBitmapHeader[18] = (colume_t & 0xFF);
		grayBitmapHeader[19] = (colume_t >> 8 & 0xFF);
		grayBitmapHeader[20] = (colume_t >> 16 & 0xFF);
		grayBitmapHeader[21] = (colume_t >> 24 & 0xFF);

		grayBitmapHeader[22] = (row_t & 0xFF);
		grayBitmapHeader[23] = (row_t >> 8 & 0xFF);
		grayBitmapHeader[24] = (row_t >> 16 & 0xFF);
		grayBitmapHeader[25] = (row_t >> 24 & 0xFF);

		grayBitmapHeader[26] = 0x01;
		grayBitmapHeader[27] = 0x00;

		grayBitmapHeader[28] = 0x08;
		grayBitmapHeader[29] = 0x00;

		for (i = 0; i < 256; i++)
		{
			grayBitmapHeader[54 + i * 4] = i;
			grayBitmapHeader[54 + i * 4 + 1] = i;
			grayBitmapHeader[54 + i * 4 + 2] = i;
		}
		write(fd, (char *)grayBitmapHeader, 1078);
		/*Be careful , bitmap save turn is buttom to top*/
		for (i = 0; i < row; i++)
		{
			write(fd, (char *)(pData + (row - i - 1) * colume), colume);
			if (colume_t > colume)
				write(fd, (char *)pad, colume_t - colume);
		}
		close(fd);
	}
	return 0;
}

int main(int argc, char *argv[])
{
    printf("%s\n", argv[1]);
	char * image_buffer[SENSOR_BUFFER_LENGTH] = {0};
    int de = cfp_get_image(argv[1], image_buffer);
	fp_SaveGrayBitmap(argv[2], image_buffer, SENSOR_HEIGHT, SENSOR_WIDTH);
    return 0;
}