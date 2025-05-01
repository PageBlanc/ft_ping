#include "../Includes/ping.h"

void handler(int signum)
{
	(void)signum;
	exit(0);
}

void signal_handler()
{
	signal(SIGINT, handler);
	signal(SIGQUIT, handler);
	signal(SIGTERM, handler);
}

void send_icmp(int sockfd, char *ip)
{
	(void )sockfd;
	printf("Sending ICMP echo request to %s\n", ip);
}

int main(int ac , char **av)
{
	if (ac < 2)
		return (printf("ft_ping: usage error: Adresse de destination requise\n"));
		
	if (check_ip(av[1]))
		return (1);
	char *ip = av[1];

	signal_handler();

	int sockfd;
	if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}

	while (1)
	{
		send_icmp(sockfd, ip);
		sleep(1);
	}
	
	return 0;
}