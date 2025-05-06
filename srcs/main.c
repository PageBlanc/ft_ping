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
	ping->time_of_send = clock();
	if (sendto(sockfd, packet, sizeof(packet), 0, (struct sockaddr *) &ping->ip, sizeof(ping->ip)) <= 0)
	{
		perror("sendto");
		return (1);
	}
	print_stats(ping, 1);
	return (0);
}

int	recv_icmp(int sockfd, t_ping *ping)
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
		return (1);
	}
	ping->time_of_recv = clock();
	ping->time_of_wait = ping->time_of_recv - ping->time_of_send;
	print_stats(ping, 2);
	ip_header = (struct iphdr *) buffer;
	icmp_header = (struct icmphdr *)(buffer + (ip_header->ihl * 4));
	if (icmp_header->type == ICMP_ECHOREPLY)
	{
		if (icmp_header->un.echo.id == (getpid() & 0xFFFF))
		{
			printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%.2f ms\n",
				bytes_received, ping->ipstr, icmp_header->un.echo.sequence,
				ping->ttl, ((double)(ping->time_of_wait) / CLOCKS_PER_SEC) * 1000);
		}
		else
			printf("ICMP ECHO REPLY from %s: icmp_seq=%d\n", ping->ipstr, icmp_header->un.echo.sequence);
	}
	else
		printf("ICMP ERROR from %s: type=%d code=%d\n", ping->ipstr, icmp_header->type, icmp_header->code);
	return (0);
}

int	main(int ac, char **av)
{
	t_ping	*ping;
	struct addrinfo	hints;
	struct addrinfo	*res;
	int		sockfd;

	if (ac < 2)
		return (printf("ft_ping: usage error: Adresse de destination requise\n"));

	ping = malloc(sizeof(t_ping));
	if (!ping)
	{
		perror("malloc");
		return (1);
	}
	memset(ping, 0, sizeof(t_ping));
	init_ping_struct(ping);
	if (defined_allarg(ping, av, ac))
	{
		free_ping_struct(ping);
		return (1);
	}
	defined_allopt(ping);
	if (inet_pton(AF_INET, ping->ipstr, &ping->ip.sin_addr) <= 0)
	{
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_RAW;
		if (getaddrinfo(ping->ipstr, NULL, &hints, &res) != 0)
		{
			printf("ft_ping: %s: Unknown host\n", ping->ipstr);
			free_ping_struct(ping);
			return (1);
		}
		memcpy(&ping->ip.sin_addr, &((struct sockaddr_in *)res->ai_addr)->sin_addr, sizeof(struct in_addr));
		freeaddrinfo(res);
	}

	sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sockfd < 0)
	{
		free_ping_struct(ping);
		perror("socket");
		return (1);
	}

	printf("PING %s (%s) %d bytes of data\n", ping->ipstr, ping->ipstr, ping->size);
	signal_handler();
	print_stats(ping, 3);
	while (g_run)
	{
		if (ping->arg->is_c[0] > 0 && ping->arg->is_c[1] == ping->seq)
		{
			print_stats(ping, 0);
			break;
		}
		if (send_icmp(sockfd, ping))
			break;
		if (recv_icmp(sockfd, ping))
			break;
		sleep(1);
	}
	close(sockfd);
	free_ping_struct(ping);
	return (0);
}
