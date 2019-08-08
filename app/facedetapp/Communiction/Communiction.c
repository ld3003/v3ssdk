#include "Communiction.h"
#include "curl/curl.h"
#include "Common.h"
#include "cJSON.h"

#include <stdio.h>
#include <curl/curl.h>

#define  REQURL "http://39.98.235.120:18080/rest/face/faceDevice/distinguish"

static unsigned char reqbuffer[512];


void json_2_data(char * in_json)
{
    cJSON * pSub;
    cJSON * pJson;
    //printf("json_2_data success 2222222222222222@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
    if(NULL == in_json){return;}
    pJson = cJSON_Parse(in_json);
    if(NULL == pJson){return;}

    printf("json_2_data success \n");

    pSub = cJSON_GetObjectItem(pJson, "MESSAGE");
    if(NULL != pSub)
    {
        printf("MESSAGE:%s\n", pSub->valuestring);
    }

    cJSON_Delete(pJson);

    return ;
    //
}




size_t  write_data(void *ptr, size_t size, size_t nmemb, void *stream) {

    printf("\n\n\n[%s]\n\n\n\n",ptr);
    snprintf(reqbuffer,sizeof(reqbuffer),"%s",ptr,nmemb);
    return nmemb;
}


NH_ERRCODE communiction_init(COMMUNICTION *comm)
{
    return NH_ERR_NOERR;
}


NH_ERRCODE communiction_pushpic(COMMUNICTION *comm, PICTURE *pic, COMMUNICTION_RESULT *result)
{

    char mac[32];
    long long devid;
    char *url= REQURL;
    CURL *pCurl = NULL;
    CURLcode res;

    struct curl_slist *headerlist = NULL;

    struct curl_httppost *post = NULL;
    struct curl_httppost *last = NULL;


    get_mac(mac,sizeof(mac),&devid,ETH_NAME);


    curl_formadd(&post, &last, CURLFORM_COPYNAME, "busiDeviceId",
                 CURLFORM_COPYCONTENTS, mac,
                 CURLFORM_END);


    curl_formadd(&post, &last,CURLFORM_COPYNAME, "file", //此处表示要传的参数名
                 CURLFORM_FILE, pic->path,                               //此处表示图片文件的路径
                 CURLFORM_CONTENTTYPE, "image/jpeg",
                 CURLFORM_END);

    pCurl = curl_easy_init();

    if (NULL != pCurl)
    {
        curl_easy_setopt(pCurl, CURLOPT_TIMEOUT, 5);
        curl_easy_setopt(pCurl, CURLOPT_URL, url);
        curl_easy_setopt(pCurl, CURLOPT_HTTPPOST, post);

        curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, write_data);

        res = curl_easy_perform(pCurl);

        if (res != CURLE_OK)
        {
            printf("curl_easy_perform() failed，error code is:%s\n", curl_easy_strerror(res));
        }


        curl_easy_cleanup(pCurl);

        //printf("%s\n",reqbuffer);

    }

    snprintf(comm->resp,512,"%s",reqbuffer);

    return NH_ERR_NOERR;

    if (strstr(reqbuffer,"MESSAGE"))
    {

        char *in_json = strstr(reqbuffer,"MESSAGE");
        cJSON * pSub;
        cJSON * pJson;
        //printf("json_2_data success 2222222222222222@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
        if(NULL == in_json){
            goto err;
        }
        pJson = cJSON_Parse(in_json);
        if(NULL == pJson){
            goto err;
        }

        printf("json_2_data success \n");

        pSub = cJSON_GetObjectItem(pJson, "MESSAGE");
        if(NULL != pSub)
        {
            printf("MESSAGE:%s\n", pSub->valuestring);
            snprintf(comm->resp,512,"%s",pSub->valuestring);

        }

        cJSON_Delete(pJson);


        //json_2_data(reqbuffer);
    }

    err:


    return NH_ERR_NOERR;
}
