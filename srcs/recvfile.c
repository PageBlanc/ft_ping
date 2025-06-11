#include "../Includes/ping.h"
#include <netinet/ip.h>
#include <netdb.h>

void	print_verbose_data(t_ping *ping, int bytes_received, struct icmphdr *icmp_header, struct iphdr *ip_header, struct sockaddr_in *src_addr, const void *buffer)
{
	(void) bytes_received; // Unused parameter
	(void) src_addr; // Unused parameter
	
	struct iphdr *orig_ip_header = ip_header;
	struct icmphdr *orig_icmp_header = icmp_header;
	const unsigned char *dump_data = (const unsigned char *)buffer;
	if (icmp_header->type == ICMP_TIME_EXCEEDED || icmp_header->type == ICMP_DEST_UNREACH) {
		orig_ip_header = (struct iphdr *)((char *)icmp_header + 8);
		orig_icmp_header = (struct icmphdr *)((char *)orig_ip_header + (orig_ip_header->ihl * 4));
		dump_data = (const unsigned char *)orig_ip_header;
	}
	printf("IP Hdr Dump:\n ");
	for (int i = 0; i < 20; i++)
	{
		printf("%02x", dump_data[i]);
		if ((i + 1) % 2 == 0) printf(" ");
	}
	printf("\n");
	printf("Vr HL TOS  Len   ID Flg  off TTL Pro  cks      Src\tDst\tData\n");
	char src_addr_str[INET_ADDRSTRLEN];
	char dst_addr_str[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &orig_ip_header->saddr, src_addr_str, INET_ADDRSTRLEN);
	inet_ntop(AF_INET, &orig_ip_header->daddr, dst_addr_str, INET_ADDRSTRLEN);
	
	printf(" %1x  %1x  %02x %04x %04x   %1x %04x  %02x  %02x %04x %-15s %-15s\n",
		orig_ip_header->version,
		orig_ip_header->ihl,
		orig_ip_header->tos,
		ntohs(orig_ip_header->tot_len),
		ntohs(orig_ip_header->id),
		(ntohs(orig_ip_header->frag_off) & 0xE000) >> 13,
		ntohs(orig_ip_header->frag_off) & 0x1FFF,
		orig_ip_header->ttl,
		orig_ip_header->protocol,
		ntohs(orig_ip_header->check),
		src_addr_str,
		dst_addr_str
	);
	printf("ICMP: type %d, code %d, size %d, id 0x%04x, seq 0x%04x\n",
		orig_icmp_header->type,
		orig_icmp_header->code,
		ping->size,
		ntohs(orig_icmp_header->un.echo.id),
		ntohs(orig_icmp_header->un.echo.sequence)
	);
}

void	print_error(int bytes_received, t_ping *ping, struct icmphdr *icmp_header, struct sockaddr_in *src_addr, struct iphdr *ip_header, const void *buffer)
{
	char ip_str[INET_ADDRSTRLEN];
	char hostname[NI_MAXHOST];

	print_stats(ping, SUB_RECVPACKETS);
	inet_ntop(AF_INET, &src_addr->sin_addr, ip_str, sizeof(ip_str));
	if (getnameinfo((struct sockaddr *)src_addr, sizeof(*src_addr), hostname, sizeof(hostname), NULL, 0, 0) != 0)
	{
		perror("getnameinfo");
		return ;
	}
	switch (icmp_header->type)
	{
		case ICMP_TIME_EXCEEDED:
		printf("%d bytes from %s (%s): Time exceeded\n", bytes_received - 20, hostname, ip_str);
		break ;
		case ICMP_DEST_UNREACH:
		printf("%d bytes from %s: Destination unreachable\n", bytes_received, inet_ntoa(ping->ip.sin_addr));
		break ;
		default:
		printf("%d bytes from %s: Unknown ICMP type %d\n", bytes_received, inet_ntoa(ping->ip.sin_addr), icmp_header->type);
		break ;
	}
	if (ping->arg->is_v)
	print_verbose_data(ping, bytes_received, icmp_header, ip_header, src_addr, buffer);
}

int recv_timeout(int sockfd, t_ping *ping)
{
	struct sockaddr_in	src_addr;
	socklen_t		addr_len;
	struct msghdr		msg;
	struct iovec		iov;
	struct iphdr		*ip_header;
	struct icmphdr		*icmp_header;
	char			buffer[1024];
	int			bytes_received;

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
	ip_header = (struct iphdr *) buffer;
	icmp_header = (struct icmphdr *)(buffer + (ip_header->ihl * 4));
	
	if (icmp_header->type == ICMP_ECHOREPLY)
	{
		if (icmp_header->un.echo.id == (getpid() & 0xFFFF))
		{
			print_stats(ping, ADD_RECVPACKETS);
			if (!ping->arg->is_f)
			{
				printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%.2f ms\n", 
					bytes_received - 12, inet_ntoa(ping->ip.sin_addr), 
					ntohs(icmp_header->un.echo.sequence), ip_header->ttl, 
					((ping->time_of_recv.tv_sec - ping->time_of_send.tv_sec) * 1000.0 + 
					(ping->time_of_recv.tv_usec - ping->time_of_send.tv_usec) / 1000.0));
			}
			if (ping->arg->is_v)
				print_verbose_data(ping, bytes_received, icmp_header, ip_header, &src_addr, buffer);
			return (bytes_received);
		}
		else
		{
			return (0); // Continue à chercher
		}
	}
	else if (icmp_header->type == ICMP_TIME_EXCEEDED || icmp_header->type == ICMP_DEST_UNREACH)
	{
		print_error(bytes_received, ping, icmp_header, &src_addr, ip_header, buffer);
		return (bytes_received);
	}
	else if (icmp_header->type == ICMP_ECHO)
	{
		// Ignorer nos propres requêtes ECHO
		return (0); // Continue à chercher
	}
	else
	{
		return (0); // Continue à chercher
	}
	return (bytes_received);
}

int	recv_icmp(int sockfd, t_ping *ping)
{
	fd_set				read_fds;
	struct timeval		timeout;
	int					ret;
	int					bytes_received;
	int					attempts = 0;
	const int			max_attempts = 10;

	while (attempts < max_attempts)
	{
		FD_ZERO(&read_fds);
		FD_SET(sockfd, &read_fds);

		if (ping->arg->is_f)
		{
			timeout.tv_sec = 0;
			timeout.tv_usec = 200000; // 200ms
		}
		else if (ping->arg->is_w[0] > 0)
		{
			timeout.tv_sec = 0;
			timeout.tv_usec = 100000; // 100ms
		}
		else
		{
			timeout.tv_sec = 1; // 1 seconde max par tentative
			timeout.tv_usec = 0;
		}

		ret = select(sockfd + 1, &read_fds, NULL, NULL, &timeout);
		if (ret == -1)
		{
			perror("select");
			return (-1);
		}
		else if (ret == 0)
		{
			// Timeout - on n'a pas reçu de réponse
			return (0);
		}
		else
		{
			bytes_received = recv_timeout(sockfd, ping);
			if (bytes_received < 0)
				return (-1);
			else if (bytes_received > 0)
				return (bytes_received);
		}
		attempts++;
	}
	return (0);
}
