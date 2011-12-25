#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>


/*
 * Signal handler
 * This signal handler will stop the service gracefully
 */
/* void siginthandler(int sig) */
/* { */
   /* switch (sig) */
   /* { */
       /* case SIGHUP : */
   /* } */
/* } */

#define STOP_DOWNLOAD_AFTER_THIS_MANY_BYTES         6000
#define MINIMAL_PROGRESS_FUNCTIONALITY_INTERVAL     3
 
struct myprogress {
  double lastruntime;
  CURL *curl;
};
 

static int progress_cb(void *p,
                       double dltotal, double dlnow,
                       double ultotal, double ulnow)
{
  struct myprogress *myp = (struct myprogress *)p;
  CURL *curl = myp->curl;
  double curtime = 0;

  curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &curtime);

  /* under certain circumstances it may be desirable for certain functionality
     to only run every N seconds, in order to do this the transaction time can
     be used */ 
  if((curtime - myp->lastruntime) >= MINIMAL_PROGRESS_FUNCTIONALITY_INTERVAL) {
    myp->lastruntime = curtime;
    fprintf(stderr, "TOTAL TIME: %f \r\n", curtime);
  }

  fprintf(stderr, "UP: %g of %g  DOWN: %g of %g\r\n",
          ulnow, ultotal, dlnow, dltotal);

  if(dlnow > STOP_DOWNLOAD_AFTER_THIS_MANY_BYTES)
    return 1;
  return 0;
}


void get_page(const char* url, const char* name, const char* extention, const char* highwatermark)
{
    CURL* easyhandle = curl_easy_init();
    char * file_name = NULL;
    struct myprogress prog;

    file_name = malloc(PATH_MAX);
    sprintf (file_name, "%s.%s", name, extention);

    FILE* file = fopen(file_name, "a");

    curl_easy_setopt(easyhandle, CURLOPT_URL, url);

    /* curl_easy_setopt(easyhandle, CURLOPT_USERAGENT, "curly"); */
    curl_easy_setopt(easyhandle, CURLOPT_PROGRESSFUNCTION, progress_cb);
    curl_easy_setopt(easyhandle, CURLOPT_NOPROGRESS, 0);
    curl_easy_setopt(easyhandle, CURLOPT_PROGRESSDATA, &prog);

    curl_easy_setopt(easyhandle, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(easyhandle, CURLOPT_AUTOREFERER, 1);
    curl_easy_setopt(easyhandle, CURLOPT_WRITEDATA, file);

    curl_easy_perform(easyhandle);

    curl_easy_cleanup(easyhandle);
}


int main(int argc, char *argv[])
{
    int i = 0;
    char * url              = NULL;
    char * name             = NULL;
    char * extention        = NULL;
    char * highwatermark    = NULL;

    for (i = 1; i < argc; i++)
    {
        if ((strncasecmp(argv[i], "-url", 4) == 0) && (i + 1 < argc)) {
            if ((argv[i + 1] != NULL) && (strlen(argv[i + 1]) > 0)) {
                url = argv[i + 1];
            }
            i++;
        }
        else if ((strncasecmp(argv[i], "-name", 4) == 0) && (i + 1 < argc)) {
            if ((argv[i + 1] != NULL) && (strlen(argv[i + 1]) > 0)) {
                name = argv[i + 1];
            }
            i++;
        }
        else if ((strncasecmp(argv[i], "-extention", 4) == 0) && (i + 1 < argc)) {
            if ((argv[i + 1] != NULL) && (strlen(argv[i + 1]) > 0)) {
                extention = argv[i + 1];
            }
            i++;
        }
        else if ((strncasecmp(argv[i], "-highwatermark", 4) == 0) && (i + 1 < argc)) {
            if ((argv[i + 1] != NULL) && (strlen(argv[i + 1]) > 0)) {
                highwatermark = argv[i + 1];
            }
            i++;
        }
        else
        {
            fprintf(stderr, "%s: Error: I don't understand argument: %s\n", argv[0], argv[i]);
            return 1;
        }
    }

    if (!url) {
        fprintf(stderr, "%s: Error: No URL supplied\n", argv[0]);
        return 1;
    }


    /* signal(SIGHUP, siginthandler); */

    while (1) {
        get_page(url, name, extention, highwatermark);
    }

    return 0;
}
