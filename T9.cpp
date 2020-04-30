#include <windows.h>
#include <string.h>

#include "dictionary.h"



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int){
  char str[]="1"; char str1[]="2";
  char msg[30];
  wsprintf(msg,"%i",stricmp(str, str1));
  MessageBox(NULL, msg, NULL, MB_OK);
  return 0;
}