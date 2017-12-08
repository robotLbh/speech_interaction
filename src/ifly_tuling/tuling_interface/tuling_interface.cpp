#include "../ifly_tuling_params.h"
#include <curl/curl.h>
#include <iostream>
#include <cstring>
#include <exception>
#include <vector>
#include "cJSON.h"
params_t params_;
using namespace std;
size_t http_data_writer(void* data,size_t size,size_t nmemb,void* content){
    long totalSize = size*nmemb;
    std::string* symbolBuffer=(std::string*)content;
    if (symbolBuffer){
        symbolBuffer->append((char *)data,((char*)data)+totalSize);
    }
    return totalSize;
}
int http_get(const std::string& url,const std::string& askJson,std::string& answerJson)
{
    int re=0;
    char szJsonData[1024];  
    strcpy(szJsonData, askJson.c_str());  
    try   
    {  
        CURL *pCurl = NULL;  
        CURLcode res;  
        curl_global_init(CURL_GLOBAL_ALL);  
        pCurl = curl_easy_init();  
        if (NULL != pCurl)   
        {  
            curl_easy_setopt(pCurl,CURLOPT_TIMEOUT,2);  
            curl_easy_setopt(pCurl,CURLOPT_URL,url.c_str());  
            curl_slist *plist=curl_slist_append(NULL,   
                    "Content-Type:application/json;charset=UTF-8");  
            curl_easy_setopt(pCurl,CURLOPT_HTTPHEADER,plist);  
            curl_easy_setopt(pCurl,CURLOPT_POSTFIELDS,szJsonData);  
            curl_easy_setopt(pCurl,CURLOPT_WRITEFUNCTION,http_data_writer);
            curl_easy_setopt(pCurl,CURLOPT_WRITEDATA,(void*)&answerJson);
            res = curl_easy_perform(pCurl);  
            long responseCode = 0;
            curl_easy_getinfo(pCurl,CURLINFO_RESPONSE_CODE,&responseCode);
            if (res!=CURLE_OK){  
                std::cout<<"curl_easy_perform() failed:"<<curl_easy_strerror(res)<<std::endl;  
                re=11;
            }  
            curl_easy_cleanup(pCurl);  
        }  
        curl_global_cleanup();  
        return re;
    }  
    catch (std::exception &ex){  
        std::cout<<"curl exception:"<<ex.what()<<std::endl;  
        return 10;
    }  
}
int languageUnderstandingCpp(const std::string& ask,std::vector<std::string>& answer)
{
    std::string url=params_.http_tuling;
    std::string key=params_.robotid_tuling;
    answer.resize(3);
    std::string answerJson;
    std::string askJson = "{";  
    askJson += "\"key\" : ";  
    askJson += "\""+key+"\",";  
    askJson += "\"info\" : ";  
    askJson += "\""+ask+"\",";  
    askJson += "\"userid\" : \"123456\"";  
    askJson += "}";  
    int i=1;
    while(i){
        int ret=http_get(url,askJson,answerJson);
        if(ret==0)
            break;
        if(i>=2)
            return ret;
        i++;
    }
    cJSON *root =cJSON_Parse(answerJson.c_str());
    cJSON *code=cJSON_GetObjectItem(root,"code");
    cJSON *text=cJSON_GetObjectItem(root,"text");
    answer[0]=text->valuestring;
    answer[1]=code->valueint;
    return 0;
}
