#include "ft_president.h"

#define READ_SIZE 512

char *read_data()
{
	char	*data;
	char	*ptr;
	size_t	cur_size;
	size_t	mem_size;
	ssize_t	bytes_read;

	data = malloc(sizeof(char) * (READ_SIZE + 1));
	if (!data)
		return (NULL);
	cur_size = 0;
	mem_size = READ_SIZE + 1;
	while ((bytes_read = read(STDIN_FILENO, data + cur_size, READ_SIZE)))
	{
		if (bytes_read == -1)
			return (free(data), NULL);
		cur_size += bytes_read;
		if (cur_size + READ_SIZE > mem_size - 1)
		{
			ptr = realloc(data, mem_size * 2);
			if (!ptr)
				return (free(data), NULL);
			data = ptr;
			mem_size *= 2;
		}
	}
	return (data);
}
