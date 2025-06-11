/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   network.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pageblanche <pageblanche@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/28 10:00:00 by pageblanche       #+#    #+#             */
/*   Updated: 2025/05/28 10:00:00 by pageblanche      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Includes/ping.h"

unsigned short	checksum(void *b, int len)
{
	unsigned short	*buf;
	unsigned int	sum;

	buf = b;
	sum = 0;
	while (len > 1)
	{
		sum += *buf++;
		len -= 2;
	}
	if (len == 1)
		sum += *(unsigned char *)buf;
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	return (~sum);
}

static int	prepare_icmp_packet(struct icmphdr *icmp, t_ping *ping)
{
	icmp->type = ICMP_ECHO;
	icmp->code = 0;
	icmp->un.echo.id = getpid() & 0xFFFF;
	icmp->un.echo.sequence = ping->seq++;
	icmp->checksum = 0;
	icmp->checksum = checksum(icmp, ping->size);
	return (0);
}

int	send_icmp(int sockfd, t_ping *ping)
{
	char			*packet;
	struct icmphdr	*icmp;

	packet = malloc(ping->size);
	if (!packet)
		return (1);
	icmp = (struct icmphdr *)packet;
	memset(packet, 0, ping->size);
	prepare_icmp_packet(icmp, ping);
	setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ping->ttl, sizeof(ping->ttl));
	gettimeofday(&ping->time_of_send, NULL);
	if (sendto(sockfd, packet, ping->size, 0, (struct sockaddr *)&ping->ip,
			sizeof(ping->ip)) <= 0)
	{
		perror("sendto");
		free(packet);
		return (1);
	}
	print_stats(ping, ADD_SENDPACKETS);
	free(packet);
	return (0);
}

static void	handle_timing(t_ping *ping)
{
	if (!ping->arg->is_f)
	{
		if (ping->arg->is_w[0] > 0)
			usleep(750000);
		else
			sleep(ping->arg->is_i[1]);
	}
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
		handle_timing(ping);
	}
}
