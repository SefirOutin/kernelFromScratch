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
	return ((void *) 0);
}
