#include "ft_president.h"

uint8_t	parse(char *data, t_ft_president *president)
{
	char	*neighbour_names[MAX_REGION][MAX_REGION];
	uint8_t	pos;
	uint8_t	count;

	for (count = 0; *data; count++)
	{
		pos = strcspn(data, " ");
		data[pos] = '\0';
		president->names[count] = data;
		data += pos + 3;
		president->pibs[count] = strtoul(data, &data, 10);
		data += 3;
		pos = strcspn(data, "\n");
		data[pos] = '\0';
		for (uint8_t j = 0;; j++)
		{
			pos = strcspn(data, "-");
			neighbour_names[count][j] = data;
			if (data[pos] == '\0')
			{
				neighbour_names[count][j + 1] = NULL;
				break ;
			}
			data[pos] = '\0';
			data += pos + 1;
		}
		data += pos + 1;
	}
	president->count = count;
	for (uint8_t idx = 0; idx < president->count; idx++)
	{
		uint8_t j;
		for (j = 0; neighbour_names[idx][j]; j++)
		{
			uint8_t k;
			for (k = 0; k < president->count; k++)
				if (strcmp(president->names[k], neighbour_names[idx][j]) == 0)
					break ;
			president->neighbors[idx][j] = k;
		}
		president->neighbors[idx][j] = 0xff;
	}

	uint64_t	mean = 0;
	for (uint8_t idx = 0; idx < president->count; idx++)
		mean += president->pibs[idx];
	mean /= president->count;
	president->mean = mean;
	return (1);
}
