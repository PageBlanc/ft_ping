#include "../Includes/ping.h"

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

int	free_ping_struct(t_ping *ping, int exit_code)
{
	static t_ping	*ping_static = NULL;

	if (ping)
	{
		ping_static = ping;
		return (exit_code);
	}
	if (!ping_static)
		return (exit_code);
	if (ping_static->arg)
	{
		free(ping_static->arg);
		ping_static->arg = NULL;
	}
	close(ping_static->sockfd);
	free(ping_static);
	return (exit_code);
}
