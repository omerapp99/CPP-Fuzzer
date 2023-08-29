#include <iostream>
#include <windows.h>
#include <string>
#include <shobjidl.h>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <curl/curl.h>
#include <thread>
#include "func.h"

using namespace std;

#define NUM_THREADS 5
string fUrl;


int PwdCheck(char* username, char* password) {
    if (strcmp(username, "Admin") == 0 && strcmp(password, "Admin") == 0)
    {
        return(1);
    }
    else { return(0); }
}



int CenterWindow(HWND hWnd) {
    RECT rc;
    GetWindowRect(hWnd, &rc);

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    int windowWidth = rc.right - rc.left;
    int windowHeight = rc.bottom - rc.top;

    int x = (screenWidth - windowWidth) / 2;
    int y = (screenHeight - windowHeight) / 2;

    SetWindowPos(hWnd, HWND_TOPMOST, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    return(0);
}

string GetWordlist(HWND hwnd, HWND hWordlist) {
    OPENFILENAME ofn = { 0 };
    char szFileName[MAX_PATH] = "";
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0";
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn))
    {
        SetWindowText(hWordlist, TEXT(szFileName));
    }
    return string(szFileName);
}

void fuzzer(HWND hListView){
    CURL* curl = curl_easy_init();
    CURLcode res;
    curl_easy_setopt(curl, CURLOPT_URL, fUrl.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 1L);
    res = curl_easy_perform(curl);
    long response_code;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    string StrResponse = std::to_string(response_code);
    //MessageBoxA(NULL, StrResponse.c_str(), "testx", MB_OK);
    curl_easy_cleanup(curl);
    if (StrResponse != "0") {
        PopulateListView(hListView, StrResponse, fUrl.c_str());
    }
    else {
        MessageBoxA(NULL, "Invalid Port / URL", "Error", MB_OK | MB_ICONERROR);
    }
}

void UrlBuild(HWND hUrl, HWND hPort, string path, HWND hListView) {
    // Get the size of the URL text
    int cTxtLen = GetWindowTextLength(hUrl);

    // Allocate memory for the URL
    char* lUrl = new char[cTxtLen + 1];
    GetWindowTextA(hUrl, lUrl, cTxtLen + 1);
    string sUrl(lUrl);
    delete[] lUrl; // Free the allocated memory

    // Get the size of the port text
    int cPortLen = GetWindowTextLength(hPort);

    // Allocate memory for the port
    char* lPort = new char[cPortLen + 1];
    GetWindowTextA(hPort, lPort, cPortLen + 1);
    string sPort(lPort);
    delete[] lPort; // Free the allocated memory

    // Build the complete URL
    //Read the Wordlist and start fuzzing
    string line;
    const char* cpath = path.c_str();
    ifstream myfile(cpath);
    if (myfile.is_open())
    {
        while (getline(myfile, line))
        {
            const char* cstr = line.c_str();
            //Here should the fuzz start, the built Url from UrlBuild function with the fuzz word.
            //Checking if there is "/" before the fuzz word.
            if (sUrl.at(sUrl.length() - 1) == '/') {
                sUrl.pop_back();
                fUrl = sUrl + ":" + sPort + "/" + cstr;
            }
            else {
                fUrl = sUrl + ":" + sPort + "/" + cstr;
            }
            CURL* curl = curl_easy_init();
            CURLcode res;
            curl_easy_setopt(curl, CURLOPT_URL, fUrl.c_str());
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 1L);
            res = curl_easy_perform(curl);
            long response_code;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
            string StrResponse = std::to_string(response_code);
            //MessageBoxA(NULL, StrResponse.c_str(), "testx", MB_OK);
            curl_easy_cleanup(curl);
            if (StrResponse != "0") {
                PopulateListView(hListView, StrResponse, fUrl.c_str());
            }
            else {
                MessageBoxA(NULL, "Invalid Port / URL", "Error", MB_OK | MB_ICONERROR);
                break;
            }
        }
        myfile.close();
    }
    else MessageBox(NULL, "Unable to open the file", "Error", MB_OK | MB_ICONERROR);
}


struct ListViewItem
{
    const char* url;
    const char* response;
};
vector<ListViewItem> itemsVector;

void PopulateListView(HWND hListView, string httpResp, string fUrl)
{
    
    ListViewItem newItem;
    newItem.url = fUrl.c_str(); // Convert string to const char*
    newItem.response = httpResp.c_str(); // Convert int to const char*
    itemsVector.push_back(newItem);


    int existingItemCount = ListView_GetItemCount(hListView);

    for (int i = existingItemCount; i < itemsVector.size(); ++i)
    {
        LVITEM lvItem = { 0 };
        lvItem.mask = LVIF_TEXT;
        lvItem.iItem = i;
        lvItem.pszText = const_cast<LPSTR>(itemsVector[i].url);
        ListView_InsertItem(hListView, &lvItem);
        ListView_SetItemText(hListView, i, 1, const_cast<LPSTR>(itemsVector[i].response));
    }
}