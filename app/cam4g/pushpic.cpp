#include <stdio.h>
#include <curl/curl.h>
//linker options: -lcurl -lcurldll
static unsigned char reqbuffer[512];


size_t write_data(void *buff, size_t size, size_t nmemb, FILE *fp){
    //回调函数，下载的数据通过这里写入本地文件
    fwrite(buff, size, nmemb, fp);
    return size*nmemb;
}


#define  REQURL "http://39.98.235.120:18080/rest/face/faceDevice/distinguish"

int test(int argc, char* argv[]){
	char *url= REQURL;
	CURL *pCurl = NULL;
	CURLcode res;

	struct curl_slist *headerlist = NULL;

	struct curl_httppost *post = NULL ;
	struct curl_httppost *last = NULL;

	curl_formadd(&post, &last, CURLFORM_COPYNAME, "busiDeviceId",
			CURLFORM_COPYCONTENTS, "280941447020552",
			CURLFORM_END);

	curl_formadd(&post, &last,CURLFORM_COPYNAME, "file", //此处表示要传的参数名
			CURLFORM_FILE, "1.jpg",                               //此处表示图片文件的路径
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


