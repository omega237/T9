// Dictionary.h
// The main file that holds the T9 functionality

#include <string.h>
#include <windows.h>

enum Priority_Class{ // different priority classes for guessing
  pc_none = 0, pc_dictionary = 1, pc_user = 2
};

struct Wordlist{
  Wordlist *wlprev;
  char *word; // holds the t9 word
  char *code; // holds the created code
  Wordlist *wlnext;
  Wordlist *wlareanext; // for next word in current t9 code area, last element points to first one in list
  Priority_Class pc; // the priority class for guessing
  int times_used; // times this word has been typed
};

Wordlist *wlfirst = NULL; // pointer to first element in list
Wordlist *wllast = NULL; // pointer to last element

Wordlist *ptactualword = NULL; // points to actually selected word for code, used by GetNextWord

Wordlist *dictionary = NULL; // this is the dictionary
int dictionary_size = 0;

// CreateT9Code
// return values
// true -> everything ok
// false -> something went wrong
// parameters
// [in] szString -> the string to be converted
// [out] szCode -> holds the t9 code

bool CreateT9Code(char *szString, char *szCode){
  int len = 0, idx = 0;
  char *code = NULL;

  if(szString == NULL || szCode == NULL) return false;

  len = (int)strlen(szString);
  code = new char[len+1];
  if(code == NULL) return false;

  while(idx < len){
    switch(szString[idx]){
      case 'a': case 'A':
      case 'b': case 'B':
      case 'c': case 'C':
        code[idx] = '2';
        break;
      case 'd': case 'D':
      case 'e': case 'E':
      case 'f': case 'F':
        code[idx] = '3';
        break;
      case 'g': case 'G':
      case 'h': case 'H':
      case 'i': case 'I':
        code[idx] = '4';
        break;
      case 'j': case 'J':
      case 'k': case 'K':
      case 'l': case 'L':
        code[idx] = '5';
        break;
      case 'm': case 'M':
      case 'n': case 'N':
      case 'o': case 'O':
        code[idx] = '6';
        break;
      case 'p': case 'P':
      case 'q': case 'Q':
      case 'r': case 'R':
      case 's': case 'S':
        code[idx] = '7';
        break;
      case 't': case 'T':
      case 'u': case 'U':
      case 'v': case 'V':
        code[idx] = '8';
        break;
      case 'w': case 'W':
      case 'x': case 'X':
      case 'y': case 'Y':
      case 'z': case 'Z':
        code[idx] = '9';
        break;
    }
    idx++;
  }
  code[idx] = '\0';
  strcpy(szCode, code);
  return true;
}

bool FillStruct(Wordlist *wl, char *szWord, Priority_Class pc, Wordlist *prev = NULL, Wordlist *next = NULL){
  char *t9code = NULL;
  int len = 0;
  bool res = false;

  len = (int)strlen(szWord);
  t9code = new char[len+1];
  if(t9code == NULL) return false;
  res = CreateT9Code(szWord, t9code);
  if(res == false) return res;
  
  wl->code = new char[len+1];
  wl->word = new char[len+1];
  if(wl->code == NULL || wl->word == NULL) return false;
  wl->pc = pc;
  wl->times_used = 0;
  wl->wlprev = prev;
  wl->wlnext = next;
  strcpy(wl->code, t9code);
  strcpy(wl->word, szWord);
  return true;
}

bool AddWordToDictionary(char *szWord, Priority_Class pc = pc_dictionary){
  int len = 0;
  bool retval = false;

  if(szWord == NULL) return false;
  len = strlen(szWord);

  if(dictionary_size == 0){ // special case, just insert
    dictionary = new Wordlist[dictionary_size+1];
    if(dictionary == NULL) return false;
    dictionary_size++;

    retval =  FillStruct(&dictionary[0], szWord, pc);
    wlfirst = &dictionary[0];
    wllast = &dictionary[0];
    return retval;
  }

  else{ // check if word to be added is already there!!
    // search through linked list for first element bigger or equal to current t9code
    Wordlist *ptactual = wlfirst;
    bool run = true;
    while((run == true) && (ptactual != NULL)){
      if(stricmp(ptactual->word, t9code) > 0)
        run = false;
      else
          ptactual = ptactual->wlnext;
      }

      dictionary = (Wordlist*) realloc (dictionary, (dictionary_size+1)*sizeof(Wordlist));
      if(dictionary == NULL) return false;
      dictionary_size++;

      if(ptactual == wlfirst){ // new element will be first in list
        retval = FillStruct(&dictionary[dictionary_size-1], szWord, pc, NULL, wlfirst);
        wlfirst = &dictionary[dictionary_size-1];
        return retval;
      }

      else if(ptactual == NULL){ // new element will be last in list
        retval = FillStruct(&dictionary[dictionary_size-1], szWord, pc, wllast);
        wllast->wlnext = &dictionary[dictionary_size-1];
        wllast = &dictionary[dictionary_size-1];
        return retval;
      }

      else{ // insert element before of ptactual
        retval = FillStruct(dictionary[dictionary_size-1], szWord, pc, ptactual->wlprev, ptactual);
        return retval;
      }
    }
  return true;
}

bool GetNextWord(char *t9code, char *szWord){
  if(t9Code == NULL || szWord == NULL) return false;

  if(ptactualword == NULL){ // if ptactualword not set then get first element of t9code area
    ptactualword = GetFirstElement(t9code);
    if(ptactualword == NULL) return false;
    strcpy(szWord, ptactualword->word);
  }
  else{ // else simply get wlareanext
    ptactualword = ptactualword->wlareanext;
    strcpy(szWord, ptactualword->word);
  }
  return true;
}

// let ptactualword point to word, to enable list cycling from actual word
bool GetPointerToWord(char *t9code, char *word){
  bool run = true;
  if(t9code == NULL  || word == NULL) return false;
  ptactualword = GetFirstElement(t9code); // get pointer to first element for code
  while(run == true){ // run through list to find word
    if(stricmp(ptactualword->word, word) == 0)
      run = false;
    ptactualword = ptactualword->wlareanext;
  }
  return true;
}

void DoSpace(){ // to enable GetNextWord again
  if(ptactualword != NULL) ptactualword->times_used++;
  ptactualword = NULL;
}

bool FillDictionary(char *szFileName){ // read a wordfile
  HANDLE wf = NULL;
  DWORD  dwBytesRead; 
  char   buff[4096]; 

  if(szFileName == NULL) return false;

  wf = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  
  if(wf == INVALID_HANDLE_VALUE) return false;

  do{ // read file
    if (ReadFile(wf, buff, 4096, &dwBytesRead, NULL)){
      // give buf to processing function AnalyzeBuffer() to return array of words and to know how much to turn file pointer back
    }
    // rewind file pointer
  }while(dwBytesRead = 4096);


  CloseHandle(wf);
  return true;
}