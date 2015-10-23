#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

int duplicate_dup2(int fd, int fd2)
{
	if (fd < 0
#ifdef OPEN_MAX
		|| fd >= OPEN_MAX
#endif // OPEN_MAX
		)
	{
		printf("the fd is error in this system\n");
		return -1;
	}

	if (fd2 < 0
#ifdef OPEN_MAX
		|| fd2 >= OPEN_MAX
#endif // OPEN_MAX
		)
	{
		printf("the fd2 error in this system\n");
		return -1;
	}

	if (fd == fd2)
		return fd2;

	int val;
	if ((val = dup(fd2)) != -1)
	{
		if (close(fd2) == -1)
			perror("dup error");
		if (close(val) == -1)
			perror("dup error");
	}

	int error_val = 0;
	int i = 0;
	int *fds = new int[fd2];
	while ((val = dup(fd)) != fd2)
	{
		if (val == -1)
		{
			error_val = val;
			break;
		}

		if (val > fd2)
		{
			printf("there is no more file hanlder to allocate\n");
			break;
		}

		fds[i++] = val;
	}

	int j = 0;
	for (; j < i; j++)
	{
		if (close(fds[j]) == -1)
			perror("free file hanlder error");
	}
	delete[] fds;
	fds = NULL;

	if (error_val != 0)
	{
		errno = error_val;
		return -1;
	}

	return fd2;
}

int main()
{
	int fd;
	char buf[] = "hello world, hello file handle.\n";
	int len = strlen(buf);

	printf("duplicate_dup2(STDOUT_FILENO, 5): ");
	fd = duplicate_dup2(STDOUT_FILENO, 5);
	printf("fd: %d\n", fd);
	if (write(fd, buf, len) != len)
		perror("write error");

	printf("\n");

	printf("duplicate_dup2(-1, 10): ");
	fd = duplicate_dup2(-1, 5);
	printf("fd: %d\n", fd);
	if (write(fd, buf, len) != len)
		perror("write error");

	printf("\n");

	printf("duplicate_dup2(1, 5): ");
	fd = duplicate_dup2(1, 5);
	printf("fd: %d\n", fd);
	if (write(fd, buf, len) != len)
		perror("write error");

	printf("\n");

	printf("duplicate_dup2(1, -1): ");
	fd = duplicate_dup2(1, -1);
	printf("fd: %d\n", fd);
	if (write(fd, buf, len) != len)
		perror("write error");

	return 0;
}