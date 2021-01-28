#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
 
#include <curl/curl.h>
#include "cJSON/cJSON.h"


struct MemoryStruct {
  char *memory;
  size_t size;
};
 static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;
 
  char *ptr = realloc(mem->memory, mem->size + realsize + 1);
  if(ptr == NULL) {
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }
 
  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
 
  return realsize;
}

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

    curl_easy_setopt(curl, CURLOPT_URL, "http://hawkeye.cmit.local:38888/api/v1.0/machine/deployment_status");

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


int main(int argc, char* argv[])
{
  char mac[20];
  FILE *f;
  char ln[80];
  system("ipconfig /all | find \"Physical Address\" > ipconfig.txt");
  f = fopen("ipconfig.txt", "r");
  if (NULL != f) {
     fgets(ln, 80, f);
     fclose(f);
     printf("MAC:%s\n", ln+strlen(ln)-19);
     snprintf(mac, sizeof(mac), "%s", ln+strlen(ln)-19);

     //snprintf(mac, sizeof(mac), "%s", "48:4d:7e:bc:4e:06");
  }

  CURL *curl_handle;
  CURLcode res;
  char response[1000] = {0};
  char url[150];

  struct MemoryStruct chunk; 
  chunk.memory = malloc(1);
  chunk.size = 0;
 
  curl_global_init(CURL_GLOBAL_ALL);
 
  curl_handle = curl_easy_init();
 
  sprintf(url, "http://hawkeye.cmit.local:38888/api/v1.0/machine?mac=%s", mac);
  curl_easy_setopt(curl_handle, CURLOPT_URL, url);
 
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
 
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk); 
  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
 
  res = curl_easy_perform(curl_handle);
 
  if(res != CURLE_OK) {
    fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(res));
  }
  else {
    strcpy(response, chunk.memory);
  }

  curl_easy_cleanup(curl_handle);
 
  free(chunk.memory);
  printf("%s", response);
 
  curl_global_cleanup();

  cJSON *cjson = cJSON_Parse(response);
  if (cjson == NULL) {
    printf("Json pack into cjson error...\n");
  }
  else {
    printf("%s\r\n", cJSON_Print(cjson)); 
  }

  cJSON* jres = cJSON_GetObjectItem(cjson, "res"); 
  char* deploy_s = cJSON_GetObjectItem(jres, "deployment_status")->valuestring;
  printf("deploy_s: %s\r\n", deploy_s);

  system("bcdedit.exe /set {fwbootmgr} bootsequence {bootmgr}");

  if (strcmp(deploy_s, "scheduled") == 0) {
    printf("Gonna deploy windows os. \n");
  }
  else if (strcmp(deploy_s, "deploying") == 0) {
    printf("The OS has been deployed, update status...\n");
    char* status = "done";
    int statuspost = put(mac, status);
    if (statuspost != 0) { 
	printf("Update os deploy statement fail...\n");
    } else {
	printf("Update os deploy statement success...\n");
    }     
    system("shutdown /r /t 0");
  }
  else{
    printf("The OS has been deployed, gonna exit!\n");
    system("shutdown -r -t 0");
  } 

  char* os_name = cJSON_GetObjectItem(jres, "os_name")->valuestring;
  char* os_path = cJSON_GetObjectItem(jres, "os_path")->valuestring;

  char* ps = (char*) malloc(strlen(os_name) + strlen(os_path) + 10);
  strcpy(ps, os_path);
  strcat(ps, "\\");
  strcat(ps, os_name);
  strcat(ps, "\\Setup.exe"); 

  char* status = "deploying";
  int statuspost = put(mac, status);
  if (statuspost != 0) { 
	printf("Post os deploy statement fail...\n");
  } else {
	printf("Post os deploy statement success...\n");
  } 
  //
  // exec setup
  printf("%s\n", ps);
  system(ps);
  /*
  if(execl("/bin/ls","ls", "-a", NULL) < 0) {
	  printf(stderr, "execl failed:%s", strerror(errno));
	  return -1;
  }
  */
  return 0;
}

