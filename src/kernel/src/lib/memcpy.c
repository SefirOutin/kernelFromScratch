#include "lib.h"

void	*memcpy(void *dest, const void *src, size_t size)
{
	unsigned int	i = 0;
	unsigned char	*d, *s;

	d = (unsigned char *)dest;
	s = (unsigned char *)src;

	if (!src && !dest)
		return ((void *)0);

		while (i++ < size)
	{
		*d++ = *s++;
	}
	return (dest);
}
