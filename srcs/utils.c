#include "../Includes/ping.h"


char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	unsigned int	i;
	int				total_len;
	char			*str;

	i = 0;
	if (!s)
		return (0);
	if (start > strlen(s))
		len = 0;
	if (strlen(s) <= len)
		total_len = strlen(s) - start + 1;
	else
		total_len = len + 1;
	str = malloc(total_len * sizeof(char));
	if (str == NULL)
		return (NULL);
	while (i < len && s[i + start])
	{
		str[i] = s[start + i];
		i++;
	}
	str[i] = 0;
	return (str);
}

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

char *ft_strjoin(char *s1, char *s2, int free_s1, int free_s2)
{
	if (!s1 || !s2)
		return NULL;

	size_t len1 = strlen(s1);
	size_t len2 = strlen(s2);
	char *result = malloc(len1 + len2 + 1);

	if (!result)
		return NULL;

	strcpy(result, s1);
	strcat(result, s2);

	if (free_s1)
		free(s1);
	if (free_s2)
		free(s2);
	return result;
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

void	init_ping_struct(t_ping *ping, char *ip)
{
	ping->ip_name = ip;
	ping->usable_ip = setusable_ip(ip);
	ping->ip.sin_family = AF_INET;
	ping->ip.sin_addr.s_addr = inet_addr(ip);
	ping->seq = 0;
	ping->pid = getpid();
	ping->ttl = 58;
	ping->time_of_send = 0;
	ping->time_of_recv = 0;
	ping->time_of_wait = 0;
}

void	free_ping_struct(t_ping *ping)
{
	if (ping->usable_ip)
		free(ping->usable_ip);
	free(ping);
}