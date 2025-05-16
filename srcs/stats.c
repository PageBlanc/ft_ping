#include "../Includes/ping.h"

void	set_rtt(t_ping *ping, t_stats *stats)
{
	double	rtt;
	double	delta;

	rtt = (ping->time_of_recv.tv_sec - ping->time_of_send.tv_sec) * 1000.0;
	rtt += (ping->time_of_recv.tv_usec - ping->time_of_send.tv_usec) / 1000.0;
	if (stats->rtt_min == 0 || rtt < stats->rtt_min)
		stats->rtt_min = rtt;
	if (rtt > stats->rtt_max)
		stats->rtt_max = rtt;
	delta = rtt - stats->rtt_avg;
	stats->rtt_avg += delta / stats->packets_received;
	stats->rtt_mdev += delta * (rtt - stats->rtt_avg);
}

void	print_stats(t_ping *ping, int set)
{
	static t_stats	stats = {0};
	static int		flood = 0;

	switch (set)
	{
		case PRINT:
			if (flood == 1)
			{
				for (int i = 0; i < stats.packets_sent - stats.packets_received; i++)
					write(1, ".", 1);
			}
			printf("--- %s ping statistics ---\n", stats.ip_name);
			printf("%d packets transmitted, %d packets received, %d%% packet loss\n",
				stats.packets_sent, stats.packets_received,
				((stats.packets_sent - stats.packets_received) * 100) / stats.packets_sent);
			if (stats.packets_received == 0)
				return ;
			stats.rtt_mdev /= stats.packets_received;
			stats.rtt_mdev = sqrt(stats.rtt_mdev);
			printf("round-trip min/avg/max/stddev = %.2f/%.2f/%.2f/%.2f ms\n",
				stats.rtt_min, stats.rtt_avg,
				stats.rtt_max, stats.rtt_mdev);
			break;
		case ADD_SENDPACKETS:
			if (stats.ip_name == NULL)
				stats.ip_name = ping->ipstr;
			stats.packets_sent++;
			break;
		case ADD_RECVPACKETS:
			stats.packets_received++;
			set_rtt(ping, &stats);
			break;
		case SUB_RECVPACKETS:
			stats.packets_received--;
			set_rtt(ping, &stats);
			break;
		case 4:
			flood = 1;
			break;
	}
}
