#include "../Includes/ping.h"

void	handler(int signum)
{
	(void) signum;
	print_stats(NULL, 0);
	free_ping_struct(NULL);
	exit(0);
}

void	signal_handler(void)
{
	signal(SIGINT, handler);
}
