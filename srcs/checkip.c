#include "../Includes/ping.h"

int checkip(t_ping *ping)
{
	struct addrinfo	hints;
	struct addrinfo	*res;

	if (inet_pton(AF_INET, ping->ipstr, &ping->ip.sin_addr) <= 0)
	{
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_RAW;
		if (getaddrinfo(ping->ipstr, NULL, &hints, &res) != 0)
		{
			printf("ft_ping: %s: Unknown host\n", ping->ipstr);
			return (free_ping_struct(ping, 1));
		}
		memcpy(&ping->ip.sin_addr, &((struct sockaddr_in *)res->ai_addr)->sin_addr, sizeof(struct in_addr));
		freeaddrinfo(res);
	}
	return (0);
}
