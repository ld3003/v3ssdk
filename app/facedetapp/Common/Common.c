#include "Common.h"
#include <sys/time.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>

int get_mac(char * mac, int len_limit , long long *devid , char *ethname)
{
    char *tmp;
    struct ifreq ifreq;
    int sock;

    if ((sock = socket (AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror ("socket");
        return -1;
    }
    strcpy (ifreq.ifr_name, ethname);    //Currently, only get eth0

    if (ioctl (sock, SIOCGIFHWADDR, &ifreq) < 0)
    {
        perror ("ioctl");
        return -1;
    }

    tmp = (char *)devid;
    tmp[0]=ifreq.ifr_hwaddr.sa_data[0];
    tmp[1]=ifreq.ifr_hwaddr.sa_data[1];
    tmp[2]=ifreq.ifr_hwaddr.sa_data[2];
    tmp[3]=ifreq.ifr_hwaddr.sa_data[3];
    tmp[4]=ifreq.ifr_hwaddr.sa_data[4];
    tmp[5]=ifreq.ifr_hwaddr.sa_data[5];

    printf("mac addr id ： %lld \n",*devid);

    return snprintf(mac,len_limit,"%lld",*devid);

    //return snprintf (mac, len_limit, "%X:%X:%X:%X:%X:%X", (unsigned char) ifreq.ifr_hwaddr.sa_data[0], (unsigned char) ifreq.ifr_hwaddr.sa_data[1], (unsigned char) ifreq.ifr_hwaddr.sa_data[2], (unsigned char) ifreq.ifr_hwaddr.sa_data[3], (unsigned char) ifreq.ifr_hwaddr.sa_data[4], (unsigned char) ifreq.ifr_hwaddr.sa_data[5]);
}

