#include "../Includes/ping.h"

void	handler(int signum)
{
	if (signum == 2 || signum == 14)
		print_stats(NULL, PRINT);
	exit(free_ping_struct(NULL, 0));
}

void	signal_handler(void)
{
	signal(SIGALRM, handler);
	signal(SIGINT, handler);
	signal(SIGQUIT, handler);
}
