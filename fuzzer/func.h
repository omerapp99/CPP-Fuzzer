#include <iostream>
#include <windows.h>
#include <string>
using namespace std;

int PwdCheck(char*, char*);
int CenterWindow(HWND);
string GetWordlist(HWND, HWND);
void PopulateListView(HWND, string, string);
string UrlBuild(HWND, HWND, string, HWND);

