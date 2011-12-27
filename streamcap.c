#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <inttypes.h>
#include <sys/stat.h>
#include <time.h>


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
    double     lastruntime;
    time_t     last_update;
    time_t     first_time;
    char *     filename;
    uint64_t   highwatermark;
    CURL *curl;
};


/* Must be free'd */
char * get_time_string(void)
{
    char      * datetime;
    time_t      mclock;
    struct tm * tmp;

    time(&mclock);
    /* Log in GMT, no exceptions */
    /* tmp = gmtime(&mclock); */
    tmp = localtime(&mclock);
    if (!tmp) {
        return NULL;
    }


    /* This string will always return 19 characters and \0 */
    /*
    datetime = malloc(sizeof(char) * 21);
    snprintf(datetime, 21, "%04d-%02d-%02d.%02d:%02d:%02dZ",
                       tmp->tm_year + 1900, tmp->tm_mon + 1, tmp->tm_mday,
                       tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
    */

    datetime = malloc(sizeof(char) * 16);
    snprintf(datetime, 16, "%04d%02d%02d.%02d%02d%02d",
                       tmp->tm_year + 1900, tmp->tm_mon + 1, tmp->tm_mday,
                       tmp->tm_hour, tmp->tm_min, tmp->tm_sec);

    return datetime;
}


static int progress_cb(void *p,
                       double dltotal, double dlnow,
                       double ultotal, double ulnow)
{
    struct myprogress *myp = (struct myprogress *)p;
    CURL *curl = myp->curl;
    double curtime = 0;
    struct stat s;

    /* Detect waterlevel on file size */
    stat(myp -> filename, &s);
    printf("File size is: %llu\n", s.st_size);

    if (s.st_size > myp->highwatermark) {
        return 1;
    }

    if (myp->first_time == 0){
        myp->first_time = time(NULL);
    }

    myp->last_update = time(NULL);

    curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &curtime);

    /* under certain circumstances it may be desirable for certain functionality
     to only run every N seconds, in order to do this the transaction time can
     be used */ 
    if((curtime - myp->lastruntime) >= MINIMAL_PROGRESS_FUNCTIONALITY_INTERVAL) {
        myp->lastruntime = curtime;
        fprintf(stderr, "TOTAL TIME: %f \r\n", curtime);
    }

    fprintf(stderr, "UP: %g of %g  DOWN: %g of %g\r\n", ulnow, ultotal, dlnow, dltotal);

    /*
    if(dlnow > STOP_DOWNLOAD_AFTER_THIS_MANY_BYTES)
        return 1;
    */
    return 0;
}


void get_page(const char* url, const char* name, const char* extention, const char* highwatermark)
{
    CURL* easyhandle = curl_easy_init();
    char * file_name = NULL;
    struct myprogress prog;
    char * tmp_parse = NULL;
    char * datetime_code = get_time_string();


    file_name = malloc(PATH_MAX);
    sprintf (file_name, "%s_%s.%s", datetime_code, name, extention);
    free(datetime_code);

    errno = 0;
    FILE* file = fopen(file_name, "a");
    if (!file) {
        fprintf(stderr, "StreamCap: Error: Couldn't open file: %s\n", strerror(errno));
        exit(1);
    }

    printf ("(Re)starting\n");

    prog.filename = file_name;
    if (highwatermark) {
        prog.highwatermark = strtoll(highwatermark, &tmp_parse, 10);
        if (tmp_parse && strlen(tmp_parse) > 0) {
            switch (tmp_parse[0]) {
                case 'k' :
                case 'K' :  prog.highwatermark *= 1024;
                            break;
                case 'm' :
                case 'M' :  prog.highwatermark *= 1024 * 1024;
                            break;
                case 'g' :
                case 'G' :  prog.highwatermark *= 1024 * 1024 * 1024;
                            break;
                default  :
                    fprintf(stderr, "Error: what the unit \"%s\" mean? - Understood are 'k', 'M' and 'G'\n", tmp_parse);
                    exit(1);
            }
        }
    } else {
        prog.highwatermark = 0;
    }


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

    free(file_name);

    return;
}


void usage(void) {
    printf("streamcap\n" \
           "              -url <url>\n" \
           "             -name <name>\n" \
           "        -extention <ext>\n" \
           "    -highwatermark <bytes>\n"
           );
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
        fprintf(stderr, "StreamCap: Error: No URL supplied\n");
        usage();
        return 1;
    }


    /* signal(SIGHUP, siginthandler); */

    while (1) {
        get_page(url, name, extention, highwatermark);
    }

    return 0;
}
