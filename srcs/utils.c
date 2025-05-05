#include "../Includes/ping.h"

void	init_ping_struct(t_ping *ping, char *ip)
{
	ping->ip_name = ip;
	ping->ip.sin_family = AF_INET;
	ping->ip.sin_addr.s_addr = inet_addr(ip);
	ping->seq = 0;
	ping->pid = getpid();
	ping->ttl = 64;
}
