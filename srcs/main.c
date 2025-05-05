#include "../Includes/ping.h"

int	g_run = 1;

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

void	send_icmp(int sockfd, t_ping *ping)
{
	char			packet[64];
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
	if (sendto(sockfd, packet, sizeof(packet), 0, (struct sockaddr *) &ping->ip, sizeof(ping->ip)) <= 0)
		perror("sendto");
}

void	recv_icmp(int sockfd, t_ping *ping)
{
	struct msghdr	msg;
	struct iovec	iov;
	char			buffer[1024];
	int				bytes_received;
	struct iphdr	*ip_header;
	struct icmphdr	*icmp_header;

	iov.iov_base = buffer;
	iov.iov_len = sizeof(buffer);
	memset(&msg, 0, sizeof(msg));
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	bytes_received = recvmsg(sockfd, &msg, 0);
	if (bytes_received < 0)
	{
		perror("recvmsg");
		return ;
	}
	ip_header = (struct iphdr *) buffer;
	icmp_header = (struct icmphdr *)(buffer + (ip_header->ihl * 4));
	if (icmp_header->type == ICMP_ECHOREPLY)
	{
		printf("64 bytes from %s: icmp_seq=%d ttl=%d time=%.2f ms\n",
			ping->ip_name, icmp_header->un.echo.sequence, ping->ttl,
			(double)(clock() - ping->seq) / CLOCKS_PER_SEC * 1000);
	}
	else
		printf("Received unexpected ICMP packet\n");
}

int	main(int ac, char **av)
{
	t_ping	*ping;
	int		sockfd;

	if (ac < 2)
		return (printf("ft_ping: usage error: Adresse de destination requise\n"));
	if (check_ip(av[1]))
		return (1);
	signal_handler();
	ping = malloc(sizeof(t_ping));
	memset(ping, 0, sizeof(t_ping));
	if (!ping)
	{
		perror("malloc");
		return (1);
	}
	init_ping_struct(ping, av[1]);
	sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sockfd < 0)
	{
		free(ping);
		perror("socket");
		return (1);
	}
	printf("FT_PING: %s\n", ping->ip_name);
	while (g_run)
	{
		send_icmp(sockfd, ping);
		recv_icmp(sockfd, ping);
		sleep(1);
	}
	close(sockfd);
	free(ping);
	return (0);
}
