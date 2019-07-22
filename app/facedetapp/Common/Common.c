#include "Common.h"
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/*执行耗时计算参数*/
static struct timeval gTpstart,gTpend;
void time_consuming_start()
{
    memset(&gTpstart,0,sizeof(struct timeval));
    memset(&gTpend,0,sizeof(struct timeval));
    gettimeofday(&gTpstart,NULL); // 开始时间
}

void time_consuming_print(char *strPuts)
{
    float timeuse;

    gettimeofday(&gTpend,NULL); // 结束时间
    timeuse=1000000*(gTpend.tv_sec-gTpstart.tv_sec)+gTpend.tv_usec-gTpstart.tv_usec;
    timeuse/=1000000;
    printf("@ %s -----> Used Time:%f  S\n",strPuts,timeuse);
}
