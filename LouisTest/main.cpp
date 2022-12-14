#include "ft_president.h"
#include <stdio.h>
#include <iostream>
#include <fcntl.h>

//#define DEBUG

int main(int argc, char **argv)
{
	t_ft_president	president;
	char			*data;

#ifdef DEBUG
	int fd = open("in.txt", O_RDONLY);
	dup2(fd, STDIN_FILENO);
#endif
	if (argc < 2)
		return (1);
	init(&president, argv[1]);
	data = read_data();
	if (!data)
		return (1);
	if (!parse(data, &president))
		return (free(data), 1);
	solve(&president);
	free(data);
	return (0);
}
