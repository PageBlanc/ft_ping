#include "../Includes/ping.h"

void	handler(int signum)
{
	(void) signum;
	print_stats(NULL, 0);
	g_run = 0;
}

void	signal_handler(void)
{
	signal(SIGINT, handler);
	signal(SIGTERM, handler);
}
