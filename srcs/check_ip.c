#include "../Includes/ping.h"

int	ft_isalldigit(char *str)
{
	int	i;

	i = 0;
	while (str[i])
	{
		if (!isdigit(str[i]))
			return (0);
		i++;
	}
	return (1);
}

int	set_option(int *option, char **av, int i, int ac)
{
	if (ac < i + 1)
	{
		printf("ft_ping: invalide argument: %s\n", av[i]);
		return (1);
	}
	av++;
	option[0] = 1;
	if (ft_isalldigit(av[i]))
	{
		option[1] = atoi(av[i]);
		if (option[1] <= 0)
		{
			printf("ft_ping: invalide argument: %s\n", av[i]);
			return (1);
		}
	}
	else
	{
		printf("ft_ping: invalide argument: %s\n", av[i]);
		return (1);
	}
	return (0);
}

int	defined_allarg(t_ping *ping, char **av, int ac)
{
	int	i;

	i = 0;
	ping->arg = malloc(sizeof(t_typearg));
	if (!ping->arg)
	{
		perror("malloc");
		return (1);
	}
	memset(ping->arg, 0, sizeof(t_typearg));
	while (av[++i])
	{
		if (av[i][0] == '-')
		{
			if (strcmp(av[i], "-v") == 0)
				ping->arg->is_v = 1;
			else if (strcmp(av[i], "-f") == 0)
				ping->arg->is_f = 1;
			else if (strcmp(av[i], "-w") == 0)
			{
				if (set_option(ping->arg->is_w, av, i, ac))
					return (1);
				i++;
			}
			else if (strcmp(av[i], "-i") == 0)
			{
				if (set_option(ping->arg->is_i, av, i, ac))
					return (1);
				i++;
			}
			else if (strcmp(av[i], "-s") == 0)
			{
				if (set_option(ping->arg->is_s, av, i, ac))
					return (1);
				i++;
			}
			else if (strcmp(av[i], "-c") == 0)
			{
				if (set_option(ping->arg->is_c, av, i, ac))
					return (1);
				i++;
			}
			else if (strcmp(av[i], "-h") == 0 || strcmp(av[i], "-?") == 0)
			{
				printf("Usage: ft_ping [options] <destination>\n");
				printf("Options:\n");
				printf("  -v\tVerbose output\n");
				printf("  -f\tFlood ping\n");
				printf("  -w\tDeadline in seconds\n");
				printf("  -i\tSet an Interval between each packet\n");
				printf("  -s\tSize of packets\n");
				printf("  -c\tStop after sending a NUMBER of packets\n");
				return (1);
			}
			else
				return (printf("ft_ping: option invalide: %s\n", av[i]));
		}
		else
		{
			if (ping->ipstr[0] == '\0')
			{
				strncpy(ping->ipstr, av[i], INET_ADDRSTRLEN);
				ping->ipstr[INET_ADDRSTRLEN - 1] = '\0';
			}
			else
				return (printf("ft_ping: option invalide : %s\n", av[i]));
		}
	}
	return (0);
}

int	defined_allopt(t_ping *ping)
{
	if (ping->arg->is_f == 1 && ping->arg->is_i[0] == 1)
		return (printf("ft_ping: options i et f incompatible\n"));
	if (!ping->arg->is_i[0])
		ping->arg->is_i[1] = 1;
	if (ping->arg->is_f == 1)
		print_stats(ping, 3);
	if (ping->arg->is_s[0] == 1)
		ping->size = ping->arg->is_s[1];
	return (0);
}
