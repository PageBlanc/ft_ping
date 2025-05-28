#include "../Includes/ping.h"

unsigned	short	checksum(void *b, int len)
{
	unsigned short	*buf;
	unsigned int	sum;

	buf = b;
	sum = 0;
	for (sum = 0; len > 1; len -= 2)
		sum += *buf++;
	if (len == 1)
		sum += *(unsigned char *) buf;
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	return (~sum);
}

int	send_icmp(int sockfd, t_ping *ping)
{
	char			packet[ping->size];
	struct icmphdr	*icmp;

	icmp = (struct icmphdr *)packet;
	memset(packet, 0, sizeof(packet));
	icmp->type = ICMP_ECHO;
	icmp->code = 0;
	icmp->un.echo.id = getpid() & 0xFFFF;
	icmp->un.echo.sequence = ping->seq++;
	icmp->checksum = 0;
	icmp->checksum = checksum(icmp, sizeof(packet));
	setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ping->ttl, sizeof(ping->ttl));
	gettimeofday(&ping->time_of_send, NULL);
	if (sendto(sockfd, packet, sizeof(packet), 0, (struct sockaddr *) &ping->ip, sizeof(ping->ip)) <= 0)
	{
		perror("sendto");
		return (1);
	}
	print_stats(ping, ADD_SENDPACKETS);
	return (0);
}

void	pingloop(t_ping *ping)
{
	int	ret;

	while (1)
	{
		if (send_icmp(ping->sockfd, ping))
			break ;
		ret = recv_icmp(ping->sockfd, ping);
		if (ret < 0)
			break ;
		gettimeofday(&ping->program_end, NULL);
		if (ping->arg->is_c[0] > 0 && ping->arg->is_c[1] == ping->seq)
		{
			print_stats(ping, PRINT);
			break ;
		}
		if (!ping->arg->is_f)
		{
			if (ping->arg->is_w[0] > 0)
				usleep(750000);
			else
				sleep(ping->arg->is_i[1]);
		}
	}
}

int	main(int ac, char **av)
{
	t_ping			*ping;

	if (ac < 2)
		return (printf("ft_ping: usage error: Adresse de destination requise\n"));
	ping = calloc(sizeof(t_ping), sizeof(t_ping));
	if (!ping)
	{
		perror("malloc");
		return (1);
	}
	init_ping_struct(ping);
	free_ping_struct(ping, 0);
	if (defined_allarg(ping, av, ac))
		return (free_ping_struct(NULL, 1));
	if (defined_allopt(ping))
		return (free_ping_struct(NULL, 1));
	if (checkip(ping))
		return (free_ping_struct(ping, 1));
	ping->sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (ping->sockfd < 0)
	{
		perror("socket");
		return (free_ping_struct(NULL, 1));
	}
	signal_handler();
	printf("FT_PING %s (%s) %d data bytes", ping->ipstr, inet_ntoa(ping->ip.sin_addr), ping->size);
	if (ping->arg->is_w[0] > 0)
		alarm(ping->arg->is_w[1]);
	if (ping->arg->is_v)
		printf(", id %p = %d\n", &ping->pid, ping->pid);
	else
		printf("\n");
	pingloop(ping);
	return (free_ping_struct(NULL, 0));
}
