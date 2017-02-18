#include "helpers.h"


char e_buf[4096] = {0};

FILE *fp;
void e_proc(char *comm, char *out, int len)
{
    fp = popen(comm, "r");
    if (fp == NULL) {
	//printf("Failed to run command\n" );
	exit(1);
    }

    /* Read the output a line at a time - output it. */
    while (fgets(e_buf, sizeof(e_buf)-1, fp) != NULL) {
	strcat(out, e_buf);
    }
    /* close */
    pclose(fp);
}

void e_file(char *path, char out, int len)
{
    
}
