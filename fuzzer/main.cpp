#include <windows.h>
#include "func.h"
#include <string>
#include <iostream>
#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")


HWND hUsername, hPassword, hButton;
HWND hUrl, hPort, hWordlist, hButtonSelect, hButtonFuzz, hThreads, hResults;
string hPath;
HWND hMainWindow, hSuccessWindow;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK SuccessWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASS wcMain = { 0 };
    wcMain.lpfnWndProc = WindowProc;
    wcMain.hInstance = hInstance;
    wcMain.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
    wcMain.lpszClassName = "MainWindowClass";

    if (!RegisterClass(&wcMain))
        return 0;

    hMainWindow = CreateWindowEx(0, "MainWindowClass", "Login",
        WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX), CW_USEDEFAULT, CW_USEDEFAULT,
        400, 200, NULL, NULL, hInstance, NULL);

    if (hMainWindow == NULL)
        return 0;

    hUsername = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "test",
        WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
        130, 20, 200, 25, hMainWindow, NULL, hInstance, NULL);

    hPassword = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "test",
        WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_PASSWORD,
        130, 60, 200, 25, hMainWindow, NULL, hInstance, NULL);

    hButton = CreateWindow("BUTTON", "Login",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        20, 100, 100, 30, hMainWindow, (HMENU)1, hInstance, NULL);

    ShowWindow(hMainWindow, nCmdShow);
    UpdateWindow(hMainWindow);
    CenterWindow(hMainWindow);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        CreateWindow("STATIC", "Username:",
            WS_CHILD | WS_VISIBLE,
            20, 22, 100, 20, hwnd, NULL, NULL, NULL);

        CreateWindow("STATIC", "Password:",
            WS_CHILD | WS_VISIBLE,
            20, 62, 100, 20, hwnd, NULL, NULL, NULL);

        break;

    case WM_COMMAND:
        if (LOWORD(wParam) == 1)
        {
            char username[100], password[100];
            GetWindowText(hUsername, username, sizeof(username));
            GetWindowText(hPassword, password, sizeof(password));

            int PwdStatus = PwdCheck(username, password);
            if (PwdStatus == 1)
            {
                ShowWindow(hMainWindow, SW_HIDE); // Hide the main window

                WNDCLASS wcSuccess = { 0 };
                wcSuccess.lpfnWndProc = SuccessWindowProc;
                wcSuccess.hInstance = GetModuleHandle(NULL);
                wcSuccess.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
                wcSuccess.lpszClassName = "SuccessWindowClass";

                RegisterClass(&wcSuccess);

                hSuccessWindow = CreateWindowEx(0, "SuccessWindowClass", "Url Fuzzing",
                    WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX), CW_USEDEFAULT, CW_USEDEFAULT,
                    400, 400, NULL, NULL, GetModuleHandle(NULL), NULL);
                if (hSuccessWindow == NULL)
                    return 0;

                ShowWindow(hSuccessWindow, SW_SHOW);
                UpdateWindow(hSuccessWindow);
                CenterWindow(hSuccessWindow);
            }
            else
            {
                MessageBox(hwnd, "Login failed. Please try again.", "Error", MB_OK | MB_ICONERROR);
            }

        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}



LRESULT CALLBACK SuccessWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static int nSortColumn = -1;
    static bool bSortAscending = true;
    switch (uMsg)
    {
    case WM_CREATE:
        //Titles in the Fuzz window
        CreateWindow("STATIC", "URL: ",
            WS_CHILD | WS_VISIBLE,
            20, 22, 50, 20, hwnd, NULL, NULL, NULL);
        CreateWindow("STATIC", "Port: ",
            WS_CHILD | WS_VISIBLE,
            20, 62, 50, 20, hwnd, NULL, NULL, NULL);
        CreateWindow("STATIC", "Word list: ",
            WS_CHILD | WS_VISIBLE,
            20, 98, 50, 30, hwnd, NULL, NULL, NULL);
        CreateWindow("STATIC", "Threads: ",
            WS_CHILD | WS_VISIBLE,
            329, 22, 56, 30, hwnd, NULL, NULL, NULL);

        //Fields in the Fuzz window
        hUrl = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "https://www.google.com/",
            WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
            80, 20, 240, 25, hwnd, NULL, NULL, NULL);
        hPort = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "80",
            WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
            80, 60, 240, 25, hwnd, NULL, NULL, NULL);
        hWordlist = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "",
            WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
            80, 100, 240, 25, hwnd, NULL, NULL, NULL);
        hThreads = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "10",
            WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
            327, 40, 60, 50, hwnd, NULL, NULL, NULL);

        //Buttons in the Fuzz window				                               
        hButtonSelect = CreateWindow("BUTTON", "Select",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            325, 98, 65, 30, hwnd, (HMENU)2, NULL, NULL);
        hButtonFuzz = CreateWindow("BUTTON", "Fuzz",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            147.5, 130, 105, 30, hwnd, (HMENU)3, NULL, NULL);

        //Results in the Fuzz window						                             
        // Create the ListView control with columns
        hResults = CreateWindow(WC_LISTVIEW, "",
            WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_SINGLESEL,
            5, 165, 382, 200, hwnd, NULL, NULL, NULL);

        // Define and insert the first column
        LVCOLUMN urlCol;
        urlCol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
        urlCol.fmt = LVCFMT_LEFT;
        urlCol.cx = 302;
        urlCol.pszText = LPSTR("URL");
        ListView_InsertColumn(hResults, 0, &urlCol);

        // Define and insert the second column
        LVCOLUMN responseCol;
        responseCol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
        responseCol.fmt = LVCFMT_LEFT;
        responseCol.cx = 63; // Adjusted width to ensure no empty column
        responseCol.pszText = LPSTR("Response");
        ListView_InsertColumn(hResults, 1, &responseCol);

        // Set additional ListView styles
        ListView_SetExtendedListViewStyle(hResults, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
        //PopulateListView(hResults);


        break;
    case WM_COMMAND:
        if (LOWORD(wParam) == 2) // Handle the "Select" button click
        {
            hPath = GetWordlist(hwnd, hWordlist);

        }
        if (LOWORD(wParam) == 3) // Handle the "Fuzz" button click
        {
            UrlBuild(hUrl, hPort, hPath, hResults);
        }
        break;

    case WM_CLOSE:
        DestroyWindow(hwnd);
        ShowWindow(hMainWindow, SW_SHOW);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}
