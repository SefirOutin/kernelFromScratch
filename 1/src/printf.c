#include "lib.h"

#define DECBASE "0123456789"
#define HEXBASE "0123456789abcdef"
#define HEXBASEUPPER "0123456789ABCDEF"
#define BASELEN(BASE) sizeof(BASE) - 1 / sizeof(char)

static int	check_format(const char *str);
static void	oula(const char *str, va_list arg, size_t *len);
size_t	pf_putchar(int c);
size_t	pf_putstr(char *s);
void	pf_putnbr(int nb, size_t *len);
// static inline void	pf_putnbr_base(unsigned long nb, const char *base, size_t *len, size_t baseLen);
void	pf_convert_base(unsigned long nb, char index, size_t *len);


int	printf(const char *str, ...)
{
	va_list	args;
	size_t	len;

	len = 0;
	if (!str)
		return (-1);
	va_start(args, str);
	if (check_format(str))
		oula(str, args, &len);
	va_end(args);
	return (len);
}

static int	check_format(const char *str)
{
	while (*str)
	{
		if (strchr(str, '%'))
		{
			str = strchr(str, '%') + 1;
			if (!strchr("%cspdiuxX", *str))
				return (0);
		}
		str++;
	}
	return (1);
}

static void	oula(const char *str, va_list arg, size_t *len)
{
	while (*str)
	{
		if (*str == 37)
		{
			if (*++str == 'c')
				*len += pf_putchar(va_arg(arg, int));
			else if (*str == 's')
				*len += pf_putstr(va_arg(arg, char *));
			else if (*str == 'p')
				pf_convert_base(va_arg(arg, unsigned long), 'p', len);
			else if (*str == 'd' || *str == 'i')
				pf_putnbr(va_arg(arg, int), len);
			else if (*str == 'u')
				pf_convert_base(va_arg(arg, unsigned int), 'u', len);
			else if (*str == 'x')
				pf_convert_base(va_arg(arg, unsigned int), 'x', len);
			else if (*str == 'X')
				pf_convert_base(va_arg(arg, unsigned int), 'X', len);
			else if (*str == '%')
				*len += pf_putchar(*str);
		}
		else
			*len += pf_putchar(*str);
		str++;
	}
}

size_t	pf_putchar(int c)
{
	serialWrite((char *)&c, 1);
	return (1);
}

void	pf_putnbr(int nb, size_t *len)
{
	long	n;

	n = nb;
	if (n < 0)
	{
		*len += pf_putchar('-');
		n = -n;
	}
	if (n > 9)
		pf_putnbr(n / 10, len);
	*len += pf_putchar(n % 10 + 48);
}

size_t	pf_putstr(char *s)
{
	if (!s)
		return (serialWrite("(null)", 6));
	return (serialWrite(s, strlen(s)));
}

static inline void	pf_putnbr_base(unsigned long nb, const char *base, size_t *len, size_t baseLen)
{
	if (baseLen < 2)
		return;
	if (nb >= baseLen)
	{
		pf_putnbr_base(nb / baseLen, base, len, baseLen);
	}
	*len += pf_putchar(base[nb % baseLen]);
}

void	pf_convert_base(unsigned long nb, char index, size_t *len)
{
	switch (index)
	{
		case 'u': {
			pf_putnbr_base(nb, DECBASE, len, BASELEN(DECBASE));
			break;
		}
		case 'x': {
			pf_putnbr_base(nb, HEXBASE, len, BASELEN(HEXBASE));
			break;
		}
		case 'X': {
			pf_putnbr_base(nb, HEXBASEUPPER, len, BASELEN(HEXBASEUPPER));
			break;
		} 
		case 'p': {
			if (!nb)
			{
				*len += serialWrite("(nil)", 5);
				return ;
			}
			*len += serialWrite("0x", 2);
			pf_putnbr_base(nb, HEXBASE, len, BASELEN(HEXBASE));
		}
	}
}