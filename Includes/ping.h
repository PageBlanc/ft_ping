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
# include <arpa/inet.h>
# include <netdb.h>
# include <ctype.h>
# include <time.h>
# include <sys/time.h>
# include <errno.h>
# include <math.h>

extern int	g_run;

typedef struct s_ping
{
	struct sockaddr_in	ip;
	char				*ip_name;
	char				*usable_ip;
	int					seq;
	pid_t				pid;
	int					ttl;
	clock_t				time_of_send;
	clock_t				time_of_recv;
	clock_t				time_of_wait;
}	t_ping;

typedef struct s_stats
{
	char	*ip_name;
	int		packets_sent;
	int		packets_received;
	double	rtt_min;
	double	rtt_avg;
	double	rtt_max;
	double	rtt_mdev;
}	t_stats;

int		check_ip(char *ip);
void	init_ping_struct(t_ping *ping, char *ip);
char	*ft_substr(char const *s, unsigned int start, size_t len);
void	signal_handler(void);
void	print_stats(t_ping *ping, int set);
void	free_ping_struct(t_ping *ping);
char	*ft_itoa(int n);

#endif