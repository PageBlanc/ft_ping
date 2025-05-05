#ifndef PING_H
# define PING_H

# include <stdio.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <signal.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <netinet/in.h>
# include <netinet/ip_icmp.h>
# include <netdb.h>
# include <arpa/inet.h>
# include <ctype.h>
# include <time.h>
# include <errno.h>

extern int	g_run;

typedef struct s_ping
{
	struct sockaddr_in	ip;
	int					seq;
	pid_t				pid;
	int					ttl;
	char				*ip_name;

}	t_ping;

int		check_ip(char *ip);
void	init_ping_struct(t_ping *ping, char *ip);
void	signal_handler(void);

#endif