#include "../Includes/ping.h"

char	*ft_itoa(int n)
{
	char	*str;
	int		len;
	int		i;

	len = 0;
	i = n;
	while (i > 0)
	{
		i /= 10;
		len++;
	}
	str = malloc((len + 1) * sizeof(char));
	if (!str)
		return (NULL);
	str[len] = '\0';
	while (n > 0)
	{
		str[--len] = (n % 10) + '0';
		n /= 10;
	}
	return (str);
}

char	*ft_strjoin(char *s1, char *s2, int free_s1, int free_s2)
{
	size_t	len1;
	size_t	len2;
	char	*result;

	len1 = strlen(s1);
	len2 = strlen(s2);
	if (!s1 || !s2)
		return (NULL);
	result = malloc((len1 + len2 + 1) * sizeof(char));
	if (!result)
		return (NULL);
	strcpy(result, s1);
	strcat(result, s2);
	if (free_s1)
		free(s1);
	if (free_s2)
		free(s2);
	return (result);
}

char	*setusable_ip(char *ip)
{
	char	*current_ip;
	char	*alloc_ip;
	int		i;
	int		j;

	i = 0;
	j = 0;
	current_ip = NULL;
	alloc_ip = malloc(1);
	if (!alloc_ip)
		return (NULL);
	alloc_ip[0] = '\0';
	while (ip[i])
	{
		if (ip[i] == '.')
		{
			j = i;
			current_ip = ft_itoa(atoi(ip + i - j));
			if (!current_ip)
				return (NULL);
			current_ip = ft_strjoin(current_ip, ".", 1, 0);
			if (!current_ip)
			{
				free(alloc_ip);
				return (NULL);
			}
			alloc_ip = ft_strjoin(alloc_ip, current_ip, 1, 1);
			if (!alloc_ip)
			{
				free(current_ip);
				return (NULL);
			}
		}
		i++;
	}
	current_ip = ft_itoa(atoi(ip + i - j));
	if (!current_ip)
		return (NULL);
	current_ip = ft_strjoin(current_ip, "\0", 1, 0);
	if (!current_ip)
	{
		free(alloc_ip);
		return (NULL);
	}
	alloc_ip = ft_strjoin(alloc_ip, current_ip, 1, 1);
	if (!alloc_ip)
	{
		free(current_ip);
		return (NULL);
	}
	return (alloc_ip);
}

void	init_ping_struct(t_ping *ping)
{
	ping->arg = NULL;
	memset(ping->ipstr, 0, INET_ADDRSTRLEN);
	ping->ip.sin_family = AF_INET;
	ping->ip.sin_addr.s_addr = 0;
	ping->seq = 0;
	ping->pid = getpid();
	ping->ttl = 64;
	ping->size = 56;
	ping->time_of_send.tv_sec = 0;
	ping->time_of_send.tv_usec = 0;
	ping->time_of_recv.tv_sec = 0;
	ping->time_of_recv.tv_usec = 0;
	ping->program_start.tv_sec = 0;
	ping->program_start.tv_usec = 0;
	ping->program_end.tv_sec = 0;
	ping->program_end.tv_usec = 0;
}

void	free_ping_struct(t_ping *ping)
{
	static t_ping	*ping_static = NULL;

	if (ping)
	{
		ping_static = ping;
		return ;
	}
	if(!ping_static)
		return ;
	if (ping_static->arg)
	{
		free(ping_static->arg);
		ping_static->arg = NULL;
	}
	close(ping_static->sockfd);
	free(ping_static);
}
