#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(void) {
	int fds[2];
	char buf[10]="Hello!";
	pipe(fds);

	if(fork() == 0) {
		read(fds[0], buf, 10);
		fprintf(1, "%d: received ping\n", getpid());
		write(fds[1], buf, 10);
	} else {
		write(fds[1], buf, 10);
		sleep(10);
		read(fds[0], buf, 10);
		fprintf(1, "%d: received pong\n", getpid());
	}

	close(fds[0]);
	close(fds[1]);
	exit(0);
}
