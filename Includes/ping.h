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

// define Statistics
# define PRINT					0
# define ADD_SENDPACKETS		1
# define ADD_RECVPACKETS		2
# define SUB_RECVPACKETS		3

#ifndef NI_MAXHOST
#define NI_MAXHOST 1025
#endif

typedef struct s_typearg
{
	int		is_v;  // verbose mode
	int		is_f;  // flood mode without waiting
	int		is_w[2]; // deadline
	int		is_c[2]; // total packets
	int		is_i[2]; // preload size of packets
	int		is_s[2]; // size of packets
	int		is_ttl[2]; // set the TTL
}	t_typearg;

typedef struct s_ping
{
	char				ipstr[INET_ADDRSTRLEN];
	struct sockaddr_in	ip;
	t_typearg			*arg;
	pid_t				pid;
	int					seq;
	int					sockfd;
	int					ttl;
	int					size;
	struct timeval		time_of_send;
	struct timeval		time_of_recv;
	struct timeval		program_start;
	struct timeval		program_end;
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

void	print_stats(t_ping *ping, int set);
void	init_ping_struct(t_ping *ping);
void	signal_handler(void);
int		defined_allarg(t_ping *ping, char **av, int ac);
int		free_ping_struct(t_ping *ping, int exit_code);
int		recv_icmp(int sockfd, t_ping *ping);
int		defined_allopt(t_ping *ping);
int		checkip(t_ping *ping);

#endif