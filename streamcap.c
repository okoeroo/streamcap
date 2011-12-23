#include <curl/curl.h>
#include <stdio.h>

void get_page(const char* url, const char* file_name)
{
    CURL* easyhandle = curl_easy_init();
    FILE* file = fopen(file_name, "a");

    curl_easy_setopt(easyhandle, CURLOPT_URL, url);


    curl_easy_setopt(easyhandle, CURLOPT_USERAGENT, "");
    curl_easy_setopt(easyhandle, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(easyhandle, CURLOPT_AUTOREFERER, 1);
    curl_easy_setopt(easyhandle, CURLOPT_WRITEDATA, file);

    curl_easy_perform(easyhandle);

    curl_easy_cleanup(easyhandle);
}


int main()
{
    while (1) {
        get_page("http://194.171.96.102:8000/room2", "room.ogv");
    }

    return 0;
}
