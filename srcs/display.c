/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   display.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pageblanche <pageblanche@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/28 10:00:00 by pageblanche       #+#    #+#             */
/*   Updated: 2025/05/28 10:00:00 by pageblanche      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Includes/ping.h"
#include <netinet/ip.h>
#include <netdb.h>

static void	print_ip_header_dump(const unsigned char *dump_data)
{
	int	i;

	printf("IP Hdr Dump:\n ");
	i = 0;
	while (i < 20)
	{
		printf("%02x", dump_data[i]);
		if ((i + 1) % 2 == 0)
			printf(" ");
		i++;
	}
	printf("\n");
}

static void	print_ip_header_info(struct iphdr *orig_ip_header)
{
	char	src_addr_str[INET_ADDRSTRLEN];
	char	dst_addr_str[INET_ADDRSTRLEN];

	printf("Vr HL TOS  Len   ID Flg  off TTL Pro  cks      "
		"Src\tDst\tData\n");
	inet_ntop(AF_INET, &orig_ip_header->saddr, src_addr_str,
		INET_ADDRSTRLEN);
	inet_ntop(AF_INET, &orig_ip_header->daddr, dst_addr_str,
		INET_ADDRSTRLEN);
	printf(" %1x  %1x  %02x %04x %04x   %1x %04x  %02x  %02x %04x "
		"%-15s %-15s\n", orig_ip_header->version, orig_ip_header->ihl,
		orig_ip_header->tos, ntohs(orig_ip_header->tot_len),
		ntohs(orig_ip_header->id),
		(ntohs(orig_ip_header->frag_off) & 0xE000) >> 13,
		ntohs(orig_ip_header->frag_off) & 0x1FFF, orig_ip_header->ttl,
		orig_ip_header->protocol, ntohs(orig_ip_header->check),
		src_addr_str, dst_addr_str);
}

void	print_verbose_data(t_ping *ping, int bytes_received,
	struct icmphdr *icmp_header, const void *buffer)
{
	struct iphdr	*orig_ip_header;
	struct icmphdr	*orig_icmp_header;

	(void)bytes_received;
	orig_ip_header = (struct iphdr *)buffer;
	orig_icmp_header = icmp_header;
	if (icmp_header->type == ICMP_TIME_EXCEEDED
		|| icmp_header->type == ICMP_DEST_UNREACH)
	{
		orig_ip_header = (struct iphdr *)((char *)icmp_header + 8);
		orig_icmp_header = (struct icmphdr *)((char *)orig_ip_header
			+ (orig_ip_header->ihl * 4));
	}
	print_ip_header_dump((const unsigned char *)orig_ip_header);
	print_ip_header_info(orig_ip_header);
	printf("ICMP: type %d, code %d, size %d, id 0x%04x, seq 0x%04x\n",
		orig_icmp_header->type, orig_icmp_header->code, ping->size,
		ntohs(orig_icmp_header->un.echo.id),
		ntohs(orig_icmp_header->un.echo.sequence));
}

void	print_error(int bytes_received, t_ping *ping,
	struct icmphdr *icmp_header, struct sockaddr_in *src_addr)
{
	char	ip_str[INET_ADDRSTRLEN];
	char	hostname[NI_MAXHOST];

	print_stats(ping, SUB_RECVPACKETS);
	inet_ntop(AF_INET, &src_addr->sin_addr, ip_str, sizeof(ip_str));
	if (getnameinfo((struct sockaddr *)src_addr, sizeof(*src_addr),
			hostname, sizeof(hostname), NULL, 0, 0) != 0)
	{
		perror("getnameinfo");
		return ;
	}
	if (icmp_header->type == ICMP_TIME_EXCEEDED)
		printf("%d bytes from %s (%s): Time exceeded\n",
			bytes_received - 12, hostname, ip_str);
	else if (icmp_header->type == ICMP_DEST_UNREACH)
		printf("%d bytes from %s: Destination unreachable\n",
			bytes_received, inet_ntoa(ping->ip.sin_addr));
	else if (icmp_header->type == ICMP_ECHO)
		printf("%d bytes from %s: Echo request\n",
			bytes_received, inet_ntoa(ping->ip.sin_addr));
	else if (icmp_header->type == ICMP_ECHOREPLY)
		printf("%d bytes from %s: Echo reply\n",
			bytes_received, inet_ntoa(ping->ip.sin_addr));
	else
		printf("%d bytes from %s: Unknown ICMP type %d\n",
			bytes_received, inet_ntoa(ping->ip.sin_addr), icmp_header->type);
}
