#include "lib.h"

char	*strchr(const char *s, int c)
{
	while (*s)
	{
		if (*s == (unsigned char)c)
			return ((char *)s);
		s++;
	}
	if (*s == c)
		return ((char *)s);
	return (NULL);
}

size_t strlen(const char *str)
{
    size_t i = 0;

    while (str[i++]);
    return (i);
}

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