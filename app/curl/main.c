#include <stdio.h>
#include <curl/curl.h>
//linker options: -lcurl -lcurldll

size_t write_function(void *buff, size_t size, size_t nmemb, FILE *fp){
    //回调函数，下载的数据通过这里写入本地文件
    fwrite(buff, size, nmemb, fp);
    return size*nmemb;
}

int main(int argc, char* argv[]){
    CURL *curl = NULL;
    CURLcode code = 0;
    char url[] = "http://www.lolhelper.cn/rank/rank.php";
    char formdata[] = "daqu=%E7%94%B5%E4%BF%A1%E4%B8%80&nickname=%E4%BC%A0%E5%A5%87%E8%8B%B1%E9%9B%84";
    char filename[] = "c:\\post.html";
    FILE *fp = fopen(filename, "w");


    curl = curl_easy_init();
    if(curl){
        //设置POST协议、URL和FORM_DATA
        curl_easy_setopt(curl, CURLOPT_POST, 1);
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, formdata);
        //设置数据回调
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_function);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        code = curl_easy_perform(curl);

        if(code == CURLE_OK){
            ;;
        }

        curl_easy_cleanup(curl);
    }

    fclose(fp);
    return 0;
}
