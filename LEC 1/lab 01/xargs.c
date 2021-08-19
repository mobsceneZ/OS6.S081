#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char* argv[]) {
    char buffer[512], *temp;

    while(1) {
	temp = buffer;
	memset(buffer, 0, 512);
	
	while(read(0, temp, 1)) {
	    if(*temp == '\n' || *temp == '\0')
		break;
	    temp++;
	}

	if(*temp == '\0')
	    break;
	else
	    *temp = '\0';

	if(fork() == 0) {
	    char* new_argv[MAXARG];
	    int i = 1;
	    
	    while(i < argc) {
		new_argv[i-1] = argv[i];
		i++;
	    }

	    new_argv[i-1] = buffer;
	    new_argv[i] = 0;

	    exec(new_argv[0], new_argv);
	}
	wait(0);
    }

    exit(0);
}
