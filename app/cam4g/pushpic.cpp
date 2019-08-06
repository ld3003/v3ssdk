#include <stdio.h>
#include <curl/curl.h>
//linker options: -lcurl -lcurldll
static unsigned char reqbuffer[512];

#include <stdlib.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <string.h>

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
	*devid = 0;
    tmp = (char *)devid;
    tmp[0]=ifreq.ifr_hwaddr.sa_data[0];
    tmp[1]=ifreq.ifr_hwaddr.sa_data[1];
    tmp[2]=ifreq.ifr_hwaddr.sa_data[2];
    tmp[3]=ifreq.ifr_hwaddr.sa_data[3];
    tmp[4]=ifreq.ifr_hwaddr.sa_data[4];
    tmp[5]=ifreq.ifr_hwaddr.sa_data[5];

    printf("mac addr id ： %lld \n",*devid);

	printf ("%X:%X:%X:%X:%X:%X", (unsigned char) ifreq.ifr_hwaddr.sa_data[0], (unsigned char) ifreq.ifr_hwaddr.sa_data[1], (unsigned char) ifreq.ifr_hwaddr.sa_data[2], (unsigned char) ifreq.ifr_hwaddr.sa_data[3], (unsigned char) ifreq.ifr_hwaddr.sa_data[4], (unsigned char) ifreq.ifr_hwaddr.sa_data[5]);

    return snprintf(mac,len_limit,"%lld",*devid);

    //return snprintf (mac, len_limit, "%X:%X:%X:%X:%X:%X", (unsigned char) ifreq.ifr_hwaddr.sa_data[0], (unsigned char) ifreq.ifr_hwaddr.sa_data[1], (unsigned char) ifreq.ifr_hwaddr.sa_data[2], (unsigned char) ifreq.ifr_hwaddr.sa_data[3], (unsigned char) ifreq.ifr_hwaddr.sa_data[4], (unsigned char) ifreq.ifr_hwaddr.sa_data[5]);
}


size_t write_data(void *buff, size_t size, size_t nmemb, FILE *fp){
    //回调函数，下载的数据通过这里写入本地文件
    fwrite(buff, size, nmemb, fp);
    return size*nmemb;
}


#define  REQURL "http://demo.zxhtong.com/camera_upload.html"

int pushpic()
{
	char mac[32];
    long long devid;
	char *url= REQURL;
	CURL *pCurl = NULL;
	CURLcode res;

	struct curl_slist *headerlist = NULL;

	struct curl_httppost *post = NULL ;
	struct curl_httppost *last = NULL;

	get_mac(mac,sizeof(mac),&devid,"eth0");

	curl_formadd(&post, &last, CURLFORM_COPYNAME, "BATVOL",
			CURLFORM_COPYCONTENTS, "3.7",
			CURLFORM_END);

	curl_formadd(&post, &last, CURLFORM_COPYNAME, "SIG",
			CURLFORM_COPYCONTENTS, "27",
			CURLFORM_END);

	curl_formadd(&post, &last, CURLFORM_COPYNAME, "DEVID",
			CURLFORM_COPYCONTENTS, devid,
			CURLFORM_END);

	curl_formadd(&post, &last,CURLFORM_COPYNAME, "file", //此处表示要传的参数名
			CURLFORM_FILE, "/tmp/tmp.jpg",                               //此处表示图片文件的路径
			CURLFORM_CONTENTTYPE, "image/jpeg",
			CURLFORM_END);

	pCurl = curl_easy_init();

	if (NULL != pCurl)
	{
		curl_easy_setopt(pCurl, CURLOPT_TIMEOUT, 5);
		curl_easy_setopt(pCurl, CURLOPT_URL, url);
		curl_easy_setopt(pCurl, CURLOPT_HTTPPOST, post);

		//curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, write_data);

		res = curl_easy_perform(pCurl);

		if (res != CURLE_OK)
		{
			printf("curl_easy_perform() failed，error code is:%s\n", curl_easy_strerror(res));
		}
		printf("\n");

		curl_easy_cleanup(pCurl);

		printf("%s\n",reqbuffer);

	}

	return 0;



}


