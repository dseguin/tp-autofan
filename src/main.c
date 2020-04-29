#include "fancontrol.h"
#include <signal.h>
#include <stddef.h>

int _running = 1;

int not_quitting(void)
{
	return _running;
}

void _sig_handler(int sig)
{
	_running = 0;
}

void _create_sigaction(void)
{
	struct sigaction sa;
	sa.sa_flags = 0;
	sa.sa_handler = _sig_handler;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);
}

int main(void)
{
	monitor_temp();
	return 0;
}
