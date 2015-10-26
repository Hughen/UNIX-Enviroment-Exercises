/*
 * 如果使用追加标志打开一个文件以便读、写，能否仍用lseek在任意位置开始读？能否用lseek更新文件中任一部分的数据？请编写一段程序验证。
 */
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

void readFile(int fd)
{
	int s = sizeof(char) * 4096;
	char *buf = new char[s];
	while (read(fd, buf, s) > 0)
	{
		printf("%s", buf);
	}
	delete[] buf;
}

void readFile(int fd, off_t offset)
{
	if (lseek(fd, offset, SEEK_SET) == -1)
	{
		perror("read file error");
		return;
	}
	readFile(fd);
}

int main()
{
	/*
	 * readme.txt
	 * line 1: It hurts to love someone and not be loved in return.
	 * line 2: But what is more painful is to love someone and never find the courage to let that person know how you feel.
	 */
	int handle = open("readme.txt", O_RDWR | O_APPEND);

	if (handle == -1)
	{
		perror("open readme.txt error");
		return 0;
	}

	char buf[] = "Power by Hughen";
	int len = strlen(buf);

	if (write(handle, buf, len) != len)
		perror("write error");

	// print readme.txt
	printf("read readme.txt to check open mode: O_APPEND\n");
	readFile(handle);
	printf("\n==========================================\n");

	// beginning from readme.txt 12 bytes
	printf("beginning from readme.txt 12 bytes\n");
	readFile(handle, 12);
	printf("\n==========================================\n");

	if (lseek(handle, 0, SEEK_SET) == -1)
		perror("seek file error");

	char buf1[] = "seek here";
	len = strlen(buf1);

	if (write(handle, buf1, len) != len)
		perror("write error");

	printf("read readme.txt no offset:\n");
	readFile(handle);
	printf("read all readme.txt\n");
	readFile(handle, 0);
	printf("\n==========================================\n");

	close(handle);
	return 0;
}