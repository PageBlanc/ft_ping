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

char *parse_ip(char *ip)
{
	char *split_ip = ft_split(ip, '.');
	if (!split_ip)
		return (NULL);
	printf("Parsed IP: %s\n", split_ip);
	return (split_ip);
}

int check_ip(char *ip)
{
	if (!ip)
		return (1);
	int i = 0;
	while (ip[i])
	{
		if (ip[i] < '0' || ip[i] > '9')
			return (1);
		i++;
	}
	return (0);
}

int main(int ac , char **av)
{
	if (ac < 2)
		return (printf("ft_ping: usage error: Adresse de destination requise\n"));

	char *ip = parse_ip(av[1]);

	if (check_ip(ip))
	{
		printf("ft_ping: %s: Adresse IP non valide\n", ip);
		return (1);
	}

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