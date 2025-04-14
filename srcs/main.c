#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>

int main(int ac , char **av)
{
	if (ac < 2)
		return (printf("ft_ping: usage error: Adresse de destination requise\n"));

	char *ip = av[1];
	
	printf("ip = %s\n", ip);
	return 0;
}