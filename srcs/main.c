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
	print_stats(ping, 1);
	return (0);
}

int	recv_icmp(int sockfd, t_ping *ping)
{
	struct msghdr	msg;
	struct iovec	iov;
	struct iphdr	*ip_header;
	struct icmphdr	*icmp_header;
	char			buffer[1024];
	int				bytes_received;

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
	gettimeofday(&ping->time_of_recv, NULL);
	print_stats(ping, 2);
	ip_header = (struct iphdr *) buffer;
	icmp_header = (struct icmphdr *)(buffer + (ip_header->ihl * 4));
	if (icmp_header->type == ICMP_ECHOREPLY)
	{
		if (icmp_header->un.echo.id == (getpid() & 0xFFFF))
		{
			if (ping->arg->is_f)
				return (0);
			printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%.2f ms\n",
				bytes_received - 12, inet_ntoa(ping->ip.sin_addr),
				icmp_header->un.echo.sequence, ping->ttl,
				((ping->time_of_recv.tv_sec - ping->time_of_send.tv_sec) * 1000.0 +
				(ping->time_of_recv.tv_usec - ping->time_of_send.tv_usec) / 1000.0));
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
	struct addrinfo	hints;
	struct addrinfo	*res;
	t_ping			*ping;
	int				sockfd;

	if (ac < 2)
		return (printf("ft_ping: usage error: Adresse de destination requise\n"));
	ping = calloc(sizeof(t_ping), sizeof(t_ping));
	if (!ping)
	{
		perror("malloc");
		return (1);
	}
	init_ping_struct(ping);
	free_ping_struct(ping);
	if (defined_allarg(ping, av, ac))
	{
		free_ping_struct(NULL);
		return (1);
	}
	if (defined_allopt(ping))
	{
		free_ping_struct(NULL);
		return (1);
	}
	if (inet_pton(AF_INET, ping->ipstr, &ping->ip.sin_addr) <= 0)
	{
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_RAW;
		if (getaddrinfo(ping->ipstr, NULL, &hints, &res) != 0)
		{
			printf("ft_ping: %s: Unknown host\n", ping->ipstr);
			free_ping_struct(NULL);
			return (1);
		}
		memcpy(&ping->ip.sin_addr, &((struct sockaddr_in *)res->ai_addr)->sin_addr, sizeof(struct in_addr));
		freeaddrinfo(res);
	}
	sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sockfd < 0)
	{
		free_ping_struct(NULL);
		perror("socket");
		return (1);
	}
	signal_handler();
	printf("FT_PING %s (%s) %d data bytes", ping->ipstr, inet_ntoa(ping->ip.sin_addr), ping->size);
	if (ping->arg->is_w[0] > 0)
		alarm(ping->arg->is_w[1]);
	if (ping->arg->is_v)
	printf(", id %p = %d\n", &ping->pid, ping->pid);
	else
		printf("\n");
	while (1)
	{
		if (send_icmp(sockfd, ping))
			break ;
		if (recv_icmp(sockfd, ping))
			break ;
		gettimeofday(&ping->program_end, NULL);
		if (ping->arg->is_c[0] > 0 && ping->arg->is_c[1] == ping->seq)
		{
			print_stats(ping, 0);
			break ;
		}
		if (ping->arg->is_f)
			continue ;
		sleep(ping->arg->is_i[1]);
	}
	close(sockfd);
	free_ping_struct(NULL);
	return (0);
}
