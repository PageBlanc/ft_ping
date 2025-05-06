#include "../Includes/ping.h"

void print_stats(t_ping *ping, int set)
{
	static char *ip_name = NULL;
	static double start_time = 0;
	static double end_time = 0;
	static int packets_sent = 0;
	static int packets_received = 0;

	if (set == 0)
	{
		end_time = clock();
		printf("\n--- %s ping statistics ---\n", ip_name);
		printf("%d packets transmitted, %d received, %d%% packet loss, time %.2f ms\n",
			packets_sent, packets_received,
			(packets_sent - packets_received) * 100 / packets_sent,
			(double)(end_time - start_time) / CLOCKS_PER_SEC * 1000);
		printf("rtt min/avg/max = %.2f/%.2f/%.2f ms\n",
			(double)(end_time - start_time) / CLOCKS_PER_SEC * 1000 / packets_received,
			(double)(end_time - start_time) / CLOCKS_PER_SEC * 1000 / packets_sent,
			(double)(end_time - start_time) / CLOCKS_PER_SEC * 1000);
	}
	else if (set == 1)
	{
		ip_name = ping->ip_name;
		packets_sent++;
	}
	else if (set == 2)
		packets_received++;
	else if (set == 3)
		start_time = clock();
}