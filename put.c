#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stddef.h>

#include <curl/curl.h>
#include "cJSON/cJSON.h"



int put(char* mac, char* status)
{
  CURL *curl;
  CURLcode res;
  char body[100];

  curl_global_init(CURL_GLOBAL_ALL);

  curl = curl_easy_init();
  if(curl)
  {
    struct curl_slist *headers = NULL;

    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers); 
   
    snprintf(body, sizeof(body), "{\"machine_mac\":\"%s\",\"deployment_status\":\"%s\"}", mac, status);

    printf("%s\r\n", body);

    curl_easy_setopt(curl, CURLOPT_URL, "http://hawkeye-dev.cmit.local:38888/api/v1.0/machine/deployment_status");

    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");

    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body);

    res = curl_easy_perform(curl);

    if(res != CURLE_OK) {
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));

      return -1;
    }
    curl_easy_cleanup(curl);
  }
  curl_global_cleanup();
  return 0;
}

/*
int main(void)
{
	CURL *curl;
	CURLcode res;
	curl = curl_easy_init();
	if(curl)
	{
		curl_easy_setopt(curl, CURLOPT_URL,
                           "http://hawkeye-dev.cmit.local:38888/api/v1.0/os");
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	}
	return 0;
}
*/

int main()
{
    char* mac = "48:4d:7e:bc:4e:06";
    char* deployment_status = "deploying";
    int statuspost = put(mac, deployment_status);
    printf("%d", statuspost);

}
