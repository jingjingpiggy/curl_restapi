#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


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
}

