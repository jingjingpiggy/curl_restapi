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
 
static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
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

int post(char* mac, char* status)
{
  CURL *curl;
  CURLcode res;
  char str[100];

  curl_global_init(CURL_GLOBAL_ALL);

  curl = curl_easy_init();
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, "http://hawkeye.cmit.local:38888/api/v1.0/machine/deployment_status");
    
    data = sprintf(str, "machine_mac=%s&deployment_status=%s", mac, status);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);

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

int getopts(int argc, char **argv, char* output, size_t size)
{
  int hasMac = 0;
  for(int i = 0; i < argc; i++)
  {
     if (!strncmp(argv[i], "--mac", 4)&&i+1<argc)
     {
         snprintf(output, size, "%s", argv[++i]);
         hasMac = 1;
        }
   }
  return hasMac;
}

 
int main(int argc, char* argv[])
{
  char mac[20];
  if (1!=(getopts(argc, argv, mac, sizeof(mac)))) {
     printf("e.g.\ntest.exe --mac aa:bb:cc:dd:ee:ff\n");
     return 0;
  } else {
     printf("mac = %s\n", mac);
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
 
  url = sprintf(mac, "http://hawkeye.cmit.local:38888/api/v1.0/machine?mac=%s", mac);
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
    //printf("%lu bytes retrieved\n", (unsigned long)chunk.size);
    strcpy(response, chunk.memory);
  }

  curl_easy_cleanup(curl_handle);
 
  free(chunk.memory);
  printf("%s", response);
 
  curl_global_cleanup();

  cJSON *cjson = cJSON_Parse(response);
  if (cjson == NULL){
	  printf("Json pack into cjson error...");
  } else {
	  /*
	   * {
		"code":	0,
		"res":	{
			"id":	3,
			"name":	"Quality-PC-3",
			"mac":	"48:4d:7e:bc:4e:06",
			"ip":	"10.0.23.115",
			"os_id":	1,
			"deployment_status":	"deploy",
			"deployment_starttime":	"",
			"os_name":	"CMGE V0-G",
			"os_path":	"\\\\10.0.23.113\\public\\V2020L_TempFix"
			}
		}

	   */ printf("%s\r\n", cJSON_Print(cjson)); }

  cJSON* jres = cJSON_GetObjectItem(cjson, "res"); 
  char* deploy_s = cJSON_GetObjectItem(jres, "deployment_status")->valuestring;
  /*
  //printf("%s\r\n", deploy_s);
  if (strcmp(deploy_s, "deploy") == 0) {
	  printf("The OS has been deployed, gonna exit!\n");
	  exit(0);
  }*/ 

  char* os_name = cJSON_GetObjectItem(jres, "os_name")->valuestring;
  char* os_path = cJSON_GetObjectItem(jres, "os_path")->valuestring;

  char* ps = (char*) malloc(strlen(os_name) + strlen(os_path) + 10);
  strcpy(ps, os_path);
  strcat(ps, "\\");
  strcat(ps, os_name);
  strcat(ps, "\\Setup.exe"); 

  int statuspost = post(mac, deployment_status)
  if (statuspost != 0) {
	  print("Post os deploy statement fail...");
  } else {
	print("Gonna deploy os...");
  } 

  // exec setup
  //printf("%s\n", ps);
  /*
  //system("ls");
  if(execl("/bin/ls","ls", "-a", NULL) < 0) {
	  printf(stderr, "execl failed:%s", strerror(errno));
	  return -1;
  }
  */

  /*
  printf("%s\r\n", deploy_s);
  printf("%s\r\n", os_name);
  printf("%s\r\n", os_path);
  */ 
  return 0;
}

