#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void find(char* path, char* filename) {
    
    char buffer[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if((fd = open(path, 0)) < 0) {
	fprintf(2, "find: can't open %s\n", path);
	return;
    }

    if((fstat(fd, &st)) < 0) {
	fprintf(2, "find: can't stat %s\n", path);
	return;
    }

    strcpy(buffer, path);
    p = buffer + strlen(buffer);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)) {
	if(de.inum == 0)
	    continue;
	memmove(p, de.name, DIRSIZ);
	p[DIRSIZ] = 0;
	if(stat(buffer, &st) < 0) {
	    fprintf(2, "find: can't stat %s\n", buffer);
	    continue;
	}

	switch(st.type) {
	    case T_FILE:
		    if(!strcmp(de.name, filename))
			    printf("%s\n", buffer);
		    break;
	    case T_DIR:
		    if(strcmp(de.name, ".") && strcmp(de.name, ".."))
			    find(buffer, filename);
		    break;
	}
    }

    return;
}

int main(int argc, char* argv[]) {
    
    if(argc == 2) {
	find(".", argv[1]);
	exit(0);
    }
    
    find(argv[1], argv[2]);
    exit(0);
}
