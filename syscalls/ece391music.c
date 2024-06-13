#include <stdint.h>
#include "ece391support.h"
#include "ece391syscall.h"

int main(){
    if (ece391_song()) {
            ece391_fdputs (1, (uint8_t*)"The music is gone.\n");
            return 3;
        }
    return 0;
}
