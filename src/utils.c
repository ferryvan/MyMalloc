// copyright libft from 42 school

#include "malloc.h"

//////////////////////////////////////////////////////////////
void ft_bzero(void *p, size_t size)
{
	if (p == NULL)
		return;

	for (size_t i = 0; i < size; ++i)
		((char *)p)[i] = 0;
}

static void ft_putchar(char c)
{
	write(1, &c, 1);
}

void ft_putnbr(size_t n)
{
	if (n > 9)
	{
		ft_putnbr(n / 10);
		ft_putnbr(n % 10);
	}
	else
		ft_putchar(n + '0');
}

void *ft_memcpy(void *dest, const void *src, size_t n)
{
	size_t i;

	i = 0;
	if (dest == src || src == NULL || dest == NULL)
		return (NULL);
	while (i < n)
	{
		((char *)dest)[i] = ((char *)src)[i];
		i++;
	}
	return (dest);
}

int ft_strlen(char const *s)
{
	if (s == NULL)
		return 0;

	int len = 0;
	while (s[len])
		len++;
	return len;
}

void ft_strcpy(char *dest, char const *src)
{
	ft_memcpy(dest, src, ft_strlen(src));
}

void ft_putstr(char const *s)
{
	if (s == NULL)
		return;
	write(1, s, ft_strlen(s));
}

void ft_putnbr16(unsigned long long n)
{
	const char *pref = "0x";
	int b = 16;
	int k = b;
	char a[b];
	char r = 0;
	while (n)
	{
		r = n % b;
		n /= b;
		if (r >= 10 && r <= 15)
			a[--k] = 'A' + r % 10;
		else if (r <= 9)
			a[--k] = '0' + r;
	}
	write(1, pref, 2);
	if (k == b)
		write(1, pref, 1);
	else
		write(1, &a[k], b - k);
}