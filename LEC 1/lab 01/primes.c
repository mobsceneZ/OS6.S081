#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

/* Reference: https://www.cs.dartmouth.edu/~doug/sieve/sieve.pdf */
/* The implementation looks like a linked-list, everytime new process is */
/* inserted at the head. */

/* first iteration: screen <- parent <- child1 */
/* second iteration: screen <- parent <- child2 <- child1 */
/* and so on ... */

/* close unnecessary file descriptor (k = 0 or 1) */
void redirect(int k, int pd[2]) {
	close(k);
	dup(pd[k]);
	close(pd[0]);
	close(pd[1]);
}

void source() {
	int n;
	for(n = 2; n <= 35; n++)
		write(1, &n, 4);
}

void cull(int n) {
	int temp;
	while(read(0, &temp, 4)) {
		if(temp % n != 0)
			write(1, &temp, 4);
	}
}

void sink() {
	int pd[2];
	int prime;

	while(read(0, &prime, 4)) {

		fprintf(1, "prime %d\n", prime);
		pipe(pd);

		if(fork() == 0) {
			redirect(1, pd);
			cull(prime);
			close(0);
			close(1);
		} else {
			redirect(0, pd);
			continue;
		}

		exit(0);

	}
}

int main(void) {
	int pd[2];
	pipe(pd);

	if(fork() == 0) {	/* child process */
		redirect(1, pd);
		source();
		close(1);
	} else {		/* parent process */
		redirect(0, pd);
		sink();
		close(0);
	}
	
	exit(0);
}
