#include <stdio.h>
#include "scanner.h"
#include "token.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>


//FSA table value for tokens
int fsaTable[16][26] = 
// WS    EOF    a-z    A-Z     0-9      +       -       *       /       %      =      >       <       .     (        )       {        }       [        ]      ,       :       #       ;       /n    otherChars
{ {0,    1016,  1,     -1,     2,      7,      8,      3,      3,      3,     11,    4,      5,      3,    6,       6,      6,       6,      6,       6,     6,      6,      3,      6,       15,   -2},//s0
  {1001, 1001,  1,     1,      1,      1001,   1001,   1001,   1001,   1001,  1001,  1001,   1001,   1001,  1001,   1001,   1001,    1001,   1001,    1001,  1001,   1001,   1001,   1001,    1001, -2},//idKw token s1
  {1002, 1002,  1002,  1002,   2,      1002,   1002,   1002,   1002,   1002,  1002,  1002,   1002,   8,     1002,   1002,   1002,    1002,   1002,    1002,  1002,   1002,   1002,   1002,    1002, -2},//intg token s2
  {1003, 1003,  1003,  1003,   1003,   1003,   1003,   1003,   1003,   1003,  1003,  1003,   1003,   1003,  1003,   1003,   1003,    1003,   1003,    1003,  1003,   1003,   1003,   1003,    1003, -2},//op token s3
  {1004, 1004,  1004,  1004,   1004,   1004,   1004,   1004,   1004,   1004,  9,     1004,   1004,   1004,  1004,   1004,   1004,    1004,   1004,    1004,  1004,   1004,   1004,   1004,    1004, -2},//gthan token s4
  {1005, 1005,  1005,  1005,   1005,   1005,   1005,   1005,   1005,   1005,  10,    1005,   1005,   1005,  1005,   1005,   1005,    1005,   1005,    1005,  1005,   1005,   1005,   1005,    1005, -2},//lthan token s5
  {1006, 1006,  1006,  1006,   1006,   1006,   1006,   1006,   1006,   1006,  1006,  1006,   1006,   1006,  1006,   1006,   1006,    1006,   1006,    1006,  1006,   1006,   1006,   1006,    1006, -2},//punc token s6
  {1007, 1007,  1007,  1007,   1007,   13,     1007,   1007,   1007,   1007,  1007,  1007,   1007,   1007,  1007,   1007,   1007,    1007,   1007,    1007,  1007,   1007,   1007,   1007,    1007, -2},//plus token s7
  {1008, 1007,  1008,  1008,   1008,   1008,   14,     1008,   1008,   1008,  1008,  1008,   1008,   1008,  1008,   1008,   1008,    1008,   1008,    1008,  1008,   1008,   1008,   1008,    1008, -2},//minus token s8
  {1009, 1009,  1009,  1009,   1009,   1009,   1009,   1009,   1009,   1009,  1009,  1009,   1009,   1009,  1009,   1009,   1009,    1009,   1009,    1009,  1009,   1009,   1009,   1009,    1009, -2},//gthaneq token s9
  {1010, 1010,  1010,  1010,   1010,   1010,   1010,   1010,   1010,   1010,  1010,  1010,   1010,   1010,  1010,   1010,   1010,    1010,   1010,    1010,  1010,   1010,   1010,   1010,    1010, -2},//lthaneq token s10
  {1011, 1011,  1011,  1011,   1011,   1011,   1011,   1011,   1011,   1011,  12,    1011,   1011,   1011,  1011,   1011,   1011,    1011,   1011,    1011,  1011,   1011,   1011,   1011,    1011, -2},//assign token s11
  {1012, 1012,  1012,  1012,   1012,   1012,   1012,   1012,   1012,   1012,  1012,  1012,   1012,   1012,  1012,   1012,   1012,    1012,   1012,    1012,  1012,   1012,   1012,   1012,    1012, -2},//equality token s12
  {1013, 1013,  1013,  1013,   1013,   1013,   1013,   1013,   1013,   1013,  1013,  1013,   1013,   1013,  1013,   1013,   1013,    1013,   1013,    1013,  1013,   1013,   1013,   1013,    1013, -2},//increm token s13
  {1014, 1014,  1014,  1014,   1014,   1014,   1014,   1014,   1014,   1014,  1014,  1014,   1014,   1014,  1014,   1014,   1014,    1014,   1014,    1014,  1014,   1014,   1014,   1014,    1014, -2},//decrem token s14
  {1015, 1015,  1015,  1015,   1015,   1015,   1015,   1015,   1015,   1015,  1015,  1015,   1015,   1015,  1015,   1015,   1015,    1015,   1015,    1015,  1015,   1015,   1015,   1015,    1015, -2}//newline token s15

};
//variables
int keyStatus;
int nextChar;
int i = 0;
char thisChar = 32;
int linenum = 1;
char keyInput[100];
char *keyString;
FILE *input;

token scanner(){
	token tokey;
  if(thisChar == 32)
    processCharacter();
	tokey = FADriver();
  if(tokey.tokenInstance[0] == '\0'){
		tokey = FADriver();
	return tokey;	
}
return tokey;
}

//finds token in table
token FADriver()
{
  int state = 0;
  int nextState;
  token toke;
  toke.tokenInstance = "";
  char *string = "";
  //assigning token to lexeme via switch statement
  //each case represents a token in the FSA table
  while(state >-1 && state<1001)
  {
      nextState = fsaTable[state][nextChar];
      switch(nextState)
      {
        case -2:
          toke = assignToken(string);
          toke.tkn = unrecognizedCharacterError;
          toke.tokenInstance = addChar(string, thisChar);
          //gets rest of string before whitespace or newline, if char is unrecognized loop will terminate
          while(thisChar >= 33 && thisChar <=64){
            processCharacter();
            toke.tokenInstance = addChar(toke.tokenInstance, thisChar);
          }
          toke = setTokenName(toke);
          return toke;
          
        case -1:
          toke = assignToken(string);
          toke.tkn = capLetterError;
          toke.tokenInstance = addChar(string, thisChar);
          while(isalpha(thisChar) || isdigit(thisChar) && thisChar != EOF){
					processCharacter();
          toke.tokenInstance = addChar(toke.tokenInstance, thisChar);
				}
          toke = setTokenName(toke);
          return toke;

        case 1001:
          toke = assignToken(string);
          toke.tkn = idKw;
          toke = setTokenName(toke);
          return toke;
        
        case 1002:
          toke = assignToken(string);
          toke.tkn = intg;
          toke = setTokenName(toke);
          return toke;
          
        case 1003:
          toke = assignToken(string);
          toke.tkn = op;
          toke = setTokenName(toke);
          return toke;

        case 1004:
          toke = assignToken(string);
          toke.tkn = gthan;
          toke = setTokenName(toke);
          return toke;

        case 1005:
          toke = assignToken(string);
          toke.tkn = lthan;
          toke = setTokenName(toke);
          return toke;

        case 1006:
          toke = assignToken(string);
          toke.tkn = punc;
          toke = setTokenName(toke);
          return toke;

        case 1007:
          toke = assignToken(string);
          toke.tkn = add;
          toke = setTokenName(toke);
          return toke;
          
        case 1008:
          toke = assignToken(string);
          toke.tkn = sub;
          toke = setTokenName(toke);
          return toke;
          
        case 1009:
          toke = assignToken(string);
          toke.tkn = gthaneq;
          toke = setTokenName(toke);
          return toke;
          
        case 1010:
          toke = assignToken(string);
          toke.tkn = lthaneq;
          toke = setTokenName(toke);
          return toke;
          
        case 1011:
          toke = assignToken(string);
          toke.tkn = assign;
          toke = setTokenName(toke);
          return toke;
          
        case 1012:
          toke = assignToken(string);
          toke.tkn = equality;
          toke = setTokenName(toke);
          return toke;
          
        case 1013:
          toke = assignToken(string);
          toke.tkn = increm;
          toke = setTokenName(toke);
          return toke;

        case 1014:
          toke = assignToken(string);
          toke.tkn = decrem;
          toke = setTokenName(toke);
          return toke;

        case 1015:
          toke = assignToken(string);
          toke.tkn = newLine;
          toke = setTokenName(toke);
          return toke;

        case 1016:
          toke = assignToken(string);
          toke.tkn = eof;
          toke = setTokenName(toke);

        default:
          if(strlen(toke.tokenInstance) > 8)
          {
              while(thisChar != 32 && thisChar != 9 && thisChar != 10)
              {
                string = addChar(string, thisChar);
                if (keyStatus == 0)
                  thisChar = fgetc(input);
                else if(keyStatus == 1)
                {
                  thisChar = keyString[i];
                  i++;
                }
                else if(keyStatus ==2 )
                  thisChar = getchar();
              }
              toke = assignToken(string);
              toke.tkn = tooManyError;
              toke = setTokenName(toke);
              return toke;
          }
          state = nextState;
          if(thisChar != 32)
          {
            string = addChar(string, thisChar);
          }
          processCharacter();
      }
  }
}

//finds index in fsa table for each character
void processCharacter(){
	if(thisChar == '\n'){
		linenum++;
	}
  //keyboard or file input
	if(keyStatus == 0)
    thisChar = fgetc(input);
  else if(keyStatus == 1)
  {
    thisChar = keyString[i];
    i++;
  }
  else if(keyStatus == 2)
    thisChar = getchar();
  //index for FSAtable
	switch (thisChar)
  {
     case 32:
     nextChar = 0;
     break;
     
     case EOF:
     nextChar = 1;
     break;

     case '+':
     nextChar = 5;
     break;

     case '-':
     nextChar = 6;
     break;

     case '*':
     nextChar = 7;
     break;

     case '/':
     nextChar = 8;
     break;

     case '%':
     nextChar = 9;
     break;

     case '=':
     nextChar = 10;
     break;

     case '>':
     nextChar = 11;
     break;

     case '<':
     nextChar = 12;
     break;

     case '.':
     nextChar = 13;
     break;

     case '(':
     nextChar = 14;
     break;

     case ')':
     nextChar = 15;
     break;

     case '{':
     nextChar = 16;
     break;

     case '}':
     nextChar = 17;
     break;

     case '[':
     nextChar = 18;
     break;

     case ']':
     nextChar = 19;
     break;

     case ',':
     nextChar = 20;
     break;

     case ':':
     nextChar = 21;
     break;

     case '#':
     nextChar = 22;
     break;

     case ';':
     nextChar = 23;
     break;

     case '\n':
     nextChar = 24;
     break;

     default:
     if(thisChar <= 122 && thisChar >= 97)
     nextChar = 2;
     else if(thisChar <= 90 && thisChar >= 65)
     nextChar = 3;
     else if(thisChar <= 57 && thisChar >= 48)
     nextChar = 4;
     else if(thisChar == 0)
     nextChar = 1;
     else if(thisChar >= 33 && thisChar <=64)
     nextChar = 25;
     break;
  }
}

//adds new character to current lexeme
char* addChar(char *string, char ch)
{
  int size = strlen(string);
  char *newString = (char*) malloc(size+2);
  strncpy(newString, string, size+1);
  strncat(newString, &ch, 1);
  return newString;
}

//initialize token
token assignToken(char *literal)
{
  token instance;
  instance.lineNum = linenum;
  if(*literal == '\n')
    instance.tokenInstance = "newline";
  else if(literal == 0)
    instance.tokenInstance = "EOF";
  else
    instance.tokenInstance = literal;
  return instance;
}

//initilize string for token descriptor
token setTokenName(token toke)
{
    switch(toke.tkn)
    {
      case 1001:
        toke.tokenName = "identifier/keyword";
        break;
      case 1002:
        toke.tokenName = "integer";
        break;
      case 1003:
        toke.tokenName = "unambiguous operator";
        break;
      case 1004:
        toke.tokenName = "greater than operator";
        break;
      case 1005:
        toke.tokenName = "less than operator";
        break;
      case 1006:
        toke.tokenName = "delimiter";
        break;
      case 1007:
        toke.tokenName = "addition operator";
        break;
      case 1008:
        toke.tokenName = "subtraction operator";
        break;
      case 1009:
        toke.tokenName = "greater than or equal to operator";
        break;
      case 1010:
        toke.tokenName = "less than or equal to operator";
      case 1011:
        toke.tokenName = "assignment operator";
        break;
      case 1012:
        toke.tokenName = "equality operator";
        break;
      case 1013:
        toke.tokenName = "increment operator";
        break;
      case 1014:
        toke.tokenName = "decrement operator";
        break;
      case 1015:
        toke.tokenName = "newline";
        break;
      case 1016:
        toke.tokenName = "eof";
        break;
      case -1:
        toke.tokenName = "ERROR - Variable starting with capital letter";
        break;
      case -2:
        toke.tokenName = "ERROR - unrecognized character";
        break;
      case -3:
        toke.tokenInstance = "ERROR - too many characters in identifier";
        break;
    }
    return toke;
}

//process keyboard input
void keyboardScan()
{
    scanf("%[^\n]", keyInput);
    int size = strlen(keyInput);
    printf("%d", size);
    keyString = (char *) malloc(size +1);
    strncpy(keyString, keyInput, size);
}