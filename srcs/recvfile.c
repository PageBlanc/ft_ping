#include "../Includes/ping.h"

void	print_error(int bytes_received, t_ping *ping, struct icmphdr *icmp_header, struct sockaddr_in *src_addr)
{
	char ip_str[INET_ADDRSTRLEN];
	char hostname[NI_MAXHOST];

	inet_ntop(AF_INET, &src_addr->sin_addr, ip_str, sizeof(ip_str));
	if (getnameinfo((struct sockaddr *)src_addr, sizeof(*src_addr), hostname, sizeof(hostname), NULL, 0, 0) != 0)
	{
		perror("getnameinfo");
		return ;
	}
	switch (icmp_header->type)
	{
		case ICMP_TIME_EXCEEDED:
			printf("%d bytes from %s (%s): Time exceeded\n", bytes_received -12, hostname, ip_str);
		break ;
		case ICMP_DEST_UNREACH:
			printf("%d bytes from %s: Destination unreachable\n", bytes_received, inet_ntoa(ping->ip.sin_addr));
		break ;
		case ICMP_ECHO:
			printf("%d bytes from %s: Echo request\n", bytes_received, inet_ntoa(ping->ip.sin_addr));
		break ;
		case ICMP_ECHOREPLY:
			printf("%d bytes from %s: Echo reply\n", bytes_received, inet_ntoa(ping->ip.sin_addr));
		break ;
		default:
			printf("%d bytes from %s: Unknown ICMP type %d\n", bytes_received, inet_ntoa(ping->ip.sin_addr), icmp_header->type);
		break ;
	}
}

int	recv_icmp(int sockfd, t_ping *ping)
{
	struct sockaddr_in	src_addr;
	socklen_t			addr_len;
	struct msghdr		msg;
	struct iovec		iov;
	struct iphdr		*ip_header;
	struct icmphdr		*icmp_header;
	char				buffer[1024];
	int					bytes_received;

	addr_len = sizeof(src_addr);
	iov.iov_base = buffer;
	iov.iov_len = sizeof(buffer);
	memset(&msg, 0, sizeof(msg));
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_name = &src_addr;
	msg.msg_namelen = addr_len;
	bytes_received = recvmsg(sockfd, &msg, 0);
	if (bytes_received < 0)
	{
		perror("recvmsg");
		return (-1);
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
			printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%.2f ms\n", bytes_received - 12, inet_ntoa(ping->ip.sin_addr), icmp_header->un.echo.sequence, ip_header->ttl, ((ping->time_of_recv.tv_sec - ping->time_of_send.tv_sec) * 1000.0 + (ping->time_of_recv.tv_usec - ping->time_of_send.tv_usec) / 1000.0));
		}
		else
			print_error(bytes_received, ping, icmp_header, &src_addr);
	}
	else
		print_error(bytes_received, ping, icmp_header, &src_addr);
	return (bytes_received);
}
