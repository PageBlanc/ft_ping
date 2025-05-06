#include "../Includes/ping.h"

void	set_rtt(t_ping *ping, t_stats *stats)
{
	double rtt;

	rtt = ((double)(ping->time_of_wait) / CLOCKS_PER_SEC) * 1000;
	if (stats->rtt_min == 0 || rtt < stats->rtt_min)
		stats->rtt_min = rtt;
	if (rtt > stats->rtt_max)
		stats->rtt_max = rtt;
	stats->rtt_avg += rtt;
	stats->rtt_mdev += (rtt - stats->rtt_avg) * (rtt - stats->rtt_avg);
}

void	print_stats(t_ping *ping, int set)
{
	static clock_t start_time = 0;
	static t_stats stats = {0};

	if (set == 0)
	{
		printf("\n--- %s ping statistics ---\n", stats.ip_name);
		printf("%d packets transmitted, %d received, %d%% packet loss, time %.2f ms\n",
			stats.packets_sent, stats.packets_received,
			((stats.packets_sent - stats.packets_received) * 100) / stats.packets_sent,
			((double)(clock() - start_time) / CLOCKS_PER_SEC) * 1000);
		printf("rtt min/avg/max/mdev = %.2f/%.2f/%.2f/%.2f ms\n",
			stats.rtt_min, stats.rtt_avg / stats.packets_received,
			stats.rtt_max, sqrt((double)(stats.rtt_mdev / stats.packets_received)));
	}
	else if (set == 1)
	{
		if (stats.ip_name == NULL)
			stats.ip_name = ping->ipstr;
		stats.packets_sent++;
	}
	else if (set == 2)
	{
		stats.packets_received++;
		set_rtt(ping, &stats);
	}
	else if (set == 3)
		start_time = clock();	
}