#include "../Includes/ping.h"

int check_ip(char *ip)
{
	int i;
	int count_dot;
	int count_num;

	i = 0;
	count_dot = 0;
	count_num = 0;
	while (ip[i])
	{
		if (isdigit(ip[i]))
			count_num++;
		else if (ip[i] == '.')
		{
			count_dot++;
			if (count_num < 1 || count_num > 3)
			{
				printf("ft_ping: %s: Adresse IP invalide\n", ip);
				return (1);
			}
			count_num = 0;
		}
		else
		{
			printf("ft_ping: %s: Nom ou service inconnu\n", ip);
			return (1);
		}
		i++;
	}
	if ((count_num < 1 || count_num > 3) || count_dot != 3)
	{
		printf("ft_ping: %s: Adresse IP invalide\n", ip);
		return (1);
	}
	return (0);
}