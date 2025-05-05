#include "../Includes/ping.h"

int	ret_err(char *msg, char *ip, char *current_num)
{
	if (current_num)
		free(current_num);
	if (msg)
		printf("ft_ping: %s: %s\n", ip, msg);
	return (1);
}

int	check_ip(char *ip)
{
	int		i;
	int		count_dot;
	int		count_num;
	char	*current_num;

	i = 0;
	count_dot = 0;
	count_num = 0;
	current_num = malloc(4);
	while (ip[i])
	{
		if (isdigit(ip[i]))
		{
			count_num++;
			if (count_num > 3)
				return (ret_err("Adresse IP invalide", ip, current_num));
			current_num[count_num - 1] = ip[i];
		}
		else if (ip[i] == '.')
		{
			current_num[count_num] = '\0';
			count_dot++;
			if (count_num < 1 || count_num > 3)
				return (ret_err("Adresse IP invalide", ip, current_num));
			else if (atoi(current_num) > 255)
				return (ret_err("Adresse IP invalide", ip, current_num));
			current_num[0] = '\0';
			count_num = 0;
		}
		else
			return (ret_err("Adresse IP invalide", ip, current_num));
		i++;
	}
	if ((count_num < 1 || count_num > 3) || count_dot != 3)
		return (ret_err("Adresse IP invalide", ip, current_num));
	current_num[count_num] = '\0';
	if (atoi(current_num) > 255)
		return (ret_err("Adresse IP invalide", ip, current_num));
	free(current_num);
	return (0);
}
