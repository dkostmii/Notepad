#pragma once

#include "resource.h"
#include "assert.h"

HWND hWndEdit;

VOID SetupWindow(HWND hWnd)
{
    hWndEdit = CreateWindowEx(
        0, L"EDIT",
        NULL,
        WS_CHILD | WS_VISIBLE | WS_VSCROLL |
        ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
        0, 0, 0, 0,
        hWnd,
        NULL,
        NULL,
        NULL
    );

    // Add text
    SendMessage(hWndEdit, WM_SETTEXT, 0, (LPARAM)L"Hello, World!");
}

VOID ResizeEdit(LPARAM size)
{
    WORD width = LOWORD(size);
    WORD height = HIWORD(size);
    MoveWindow(hWndEdit, 0, 0, width, height, TRUE);
}

VOID ClearEdit()
{
    if (!hWndEdit)
    {
        assert(false);
    }
    // Select all text
    SendMessage(hWndEdit, EM_SETSEL, 0, -1);
    LPCWSTR empty = L"";
    // Replace all text with empty string
    SendMessage(hWndEdit, EM_REPLACESEL, FALSE, (LPARAM)empty);

    // Example below is making trouble in msg loop
    // SetWindowText(hWndEdit, L"");
}

VOID PaintEdit(HDC hdc)
{
    if (!hdc)
    {
        assert(false);
    }

    // Change the font

    int nHeight = -MulDiv(11, GetDeviceCaps(hdc, LOGPIXELSY), 72);
    int nWidth = -MulDiv(7, GetDeviceCaps(hdc, LOGPIXELSX), 72);


    HFONT hFont = CreateFont(nHeight, nWidth, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
        CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, L"Consolas");
    SendMessage(hWndEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
}

// Synchronous way to save the file
BOOL SaveToFile()
{
    HANDLE hFile;
    BOOL bSuccess = FALSE;

    hFile = CreateFileA(
        (LPCSTR)"mynote.txt", 
        GENERIC_WRITE, 
        0, 
        NULL, 
        CREATE_ALWAYS, 
        FILE_ATTRIBUTE_NORMAL, 
        NULL);

    if (hFile != INVALID_HANDLE_VALUE)
    {
        DWORD dwTextLength = GetWindowTextLength(hWndEdit);

        if (dwTextLength > 0)
        {
            DWORD dwBufferSize = dwTextLength + 1;
            LPWSTR pszText = (LPWSTR)GlobalAlloc(GPTR, dwBufferSize);

            if (pszText != NULL)
            {
                if (GetWindowText(hWndEdit, pszText, dwBufferSize))
                {
                    DWORD dwWritten;
                    unsigned char Header[2]; //unicode text file header
                    Header[0] = 0xFF;
                    Header[1] = 0xFE;
                    if (WriteFile(hFile, Header, sizeof(Header), &dwWritten, NULL) && WriteFile(hFile, (LPCTSTR)pszText, dwTextLength * 2, &dwWritten, NULL))
                        bSuccess = TRUE;
                }
                GlobalFree(pszText);
            }
        }
        else
        {
            bSuccess = TRUE;
        }
        CloseHandle(hFile);
    }
    return bSuccess;
}

BOOL OpenFile()
{
    HANDLE hFile;
    BOOL bSuccess = FALSE;

    hFile = CreateFileA(
        (LPCSTR)"mynote.txt",
        GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        0
        );

    if (hFile != INVALID_HANDLE_VALUE)
    {
        DWORD dwBufferSize = GetFileSize(hFile, NULL);

        LPWSTR pszText = (LPWSTR)GlobalAlloc(GPTR, dwBufferSize);

        if (pszText != NULL)
        {
            DWORD dwRead;
            if (ReadFile(hFile, pszText, dwBufferSize, &dwRead, NULL))
            {
                SetWindowText(hWndEdit, pszText);
                bSuccess = TRUE;
            }
            GlobalFree(pszText);
        }
        CloseHandle(hFile);
    }

    return bSuccess;
}