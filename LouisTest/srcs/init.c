#include "ft_president.h"

uint8_t	init(t_ft_president *president, const char *count)
{
	int32_t	count_i;

	count_i = atoi(count);
	if (count_i <= 0 || count_i > MAX_REGION)
		return (0);
	president->res_size = count_i;
	return (1);
}
