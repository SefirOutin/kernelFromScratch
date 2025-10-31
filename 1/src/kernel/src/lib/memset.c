#include "lib.h"

void	*memset(void *p, int v, size_t size)
{
	unsigned int	i;
	unsigned char	*s;

	i = 0;
	s = p;
	while (i++ < size)
		*s++ = (unsigned char)v;
	return (p);
}
