#ifndef REGION_H
# define REGION_H

# include <stdint.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>

#ifdef __cplusplus
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <iostream>
#endif

# define MAX_REGION 30

typedef struct s_ft_president
{
	char		*names[MAX_REGION];
	uint8_t		neighbors[MAX_REGION][MAX_REGION];
	int32_t		pibs[MAX_REGION];
	uint32_t	mean;
	uint8_t		count;
	uint8_t		res_size;
}	t_ft_president;

#ifdef __cplusplus
extern "C" {
#endif

uint8_t	init(t_ft_president *president, const char *count);

char	*read_data();
uint8_t	parse(char *data, t_ft_president *president);

#ifdef __cplusplus
}
#endif

void	solve(t_ft_president *_president);

#endif // REGION_H
