#include <stdint.h>

#include "ece391support.h"
#include "ece391syscall.h"

int main ()
{
    int32_t fd, cnt, i, j;
    uint8_t buf[1024];
    uint8_t filename[32];
    uint8_t string[1024 - 32 - 1];

    if (0 != ece391_getargs (buf, 1024)) {
        ece391_fdputs (1, (uint8_t*)"could not read arguments\n");
	return 3;
    }

    i = 0;
    while(buf[i] != 0x20 && i < 32) {
        filename[i] = buf[i];
        i++;
    }
    filename[i] = 0x00;
    i++;

    j = 0;
    while(buf[i] != 0x00 && i < 1024-32-1) {
        string[j] = buf[i];
        i++;
        j++;
    }


    if (-1 == (fd = ece391_open (filename))) {
        ece391_fdputs (1, (uint8_t*)"file not found\n");
	return 2;
    }

    while (0 != (cnt = ece391_write (fd, string, ece391_strlen(string)))) {
        if (-1 == cnt) {
	    ece391_fdputs (1, (uint8_t*)"file write failed\n");
	    return 3;
	    }
    }
	ece391_fdputs (1, (uint8_t*)"file write success\n");

    for (i = 0; i < 1024; i++) {
        buf[i] = 0x00;
        if (i < 32) {
            filename[i] = 0x00;
        }
        if (i < 1024 - 32 - 1){
            string[i] = 0x00;
        }
    }

    return 0;
}

