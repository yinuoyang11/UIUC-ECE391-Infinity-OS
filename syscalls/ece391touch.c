#include <stdint.h>

#include "ece391support.h"
#include "ece391syscall.h"

int main ()
{
    uint8_t buf[1024];
    uint8_t filename[32];
    int i = 0;

    if (0 != ece391_getargs (buf, 1024)) {
        ece391_fdputs (1, (uint8_t*)"could not read arguments\n");
	return 3;
    }

    while(buf[i] != 0x20 && buf[i] != 0x00 && i < 32) {
        filename[i] = buf[i];
        i++;
    }
    filename[i] = 0x00;

    if (-1 == (ece391_touch (filename, i))) {
        ece391_fdputs (1, (uint8_t*)"touch file failed\n");
	    return 2;
    }

    for (i = 0; i < 1024; i++) {
        buf[i] = 0x00;
        if (i < 32) {
            filename[i] = 0x00;
        }
    }

	ece391_fdputs (1, (uint8_t*)"file creation success\n");


    return 0;
}

