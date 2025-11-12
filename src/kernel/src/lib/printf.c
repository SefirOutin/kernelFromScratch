#include "lib.h"

#define HEXBASE "0123456789abcdef"
#define HEXBASEUPPER "0123456789ABCDEF"
#define BINBASELEN 2
#define DECBASELEN 10
#define HEXBASELEN 16

static int	check_format(const char *str);
static void	oula(const char *str, va_list arg, size_t *len);
int			pf_putchar(int c);
size_t		pf_putstr(char *s);
void		pf_putnbr(int nb, size_t *len);
void		pf_convert_base(unsigned long nb, char index, size_t *len);


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
			if (!strchr("%cspdiuxXb", *str))
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
			else if (*str == 'b')
				pf_convert_base(va_arg(arg, unsigned int), 'b', len);
			else if (*str == '%')
				*len += pf_putchar(*str);
		}
		else
			*len += pf_putchar(*str);
		str++;
	}
}

int	pf_putchar(int c)
{
	serial_putchar(c);
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
		return (serial_write("(null)", 6));

	return (serial_write(s, strlen(s)));
}

static inline void	pf_putnbr_base(unsigned long nb, const char *base, size_t *len, size_t baseLen)
{
	if (baseLen < 2)
		return;

	if (nb >= baseLen)
		pf_putnbr_base(nb / baseLen, base, len, baseLen);
	
	*len += pf_putchar(base[nb % baseLen]);
}

void	pf_convert_base(unsigned long nb, char index, size_t *len)
{
	switch (index)
	{
		case 'u':
			pf_putnbr_base(nb, HEXBASE, len, DECBASELEN);
			break;
		case 'x':
			pf_putnbr_base(nb, HEXBASE, len, HEXBASELEN);
			break;
		case 'X':
			pf_putnbr_base(nb, HEXBASEUPPER, len, HEXBASELEN);
			break;
		case 'p':
			if (!nb)
				{ *len += serial_write("(nil)", 5); return ; }
			*len += serial_write("0x", 2);
			pf_putnbr_base(nb, HEXBASE, len, HEXBASELEN);
			break;
		case 'b':
			pf_putnbr_base(nb, HEXBASE, len, BINBASELEN);
			break;
	}
}
