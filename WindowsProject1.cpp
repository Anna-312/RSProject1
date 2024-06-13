// WindowsProject1.cpp : Defines the entry point for the application.
//

#define _CRT_SECURE_NO_WARNINGS
#define TIMER1 1001
#include "framework.h"
#include "WindowsProject1.h"
#include <cstdio>
#include <locale.h>
#include <commdlg.h>
#include <process.h>
#include "RS.h"
#include <glut.h>

#define MAX_LOADSTRING 50
TCHAR FileName[200];
typedef struct
{
    HWND hWnd;
    HANDLE hEvent;
    BOOL bContinue;
    BOOL bKill;
} PARAM, *PPARAMS;
CRITICAL_SECTION cs;
float u[100][100];
float u_temp[100][100];


void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
    PIXELFORMATDESCRIPTOR pfd;

    int iFormat;

    /* get the device context (DC) */

    /* set the pixel format for the DC */
    ZeroMemory(&pfd, sizeof(pfd));

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
        PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    iFormat = ChoosePixelFormat(*hDC, &pfd);

    SetPixelFormat(*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext(*hDC);

    wglMakeCurrent(*hDC, *hRC);
}


void Thread(PVOID pvoid)
{
    PPARAMS pparams;
    pparams = (PPARAMS)pvoid;
    while (!pparams->bKill)
    {
        WaitForSingleObject(pparams->hEvent, INFINITE);
        //некоторые действия
        while (pparams->bContinue)
        {
            EnterCriticalSection(&cs);
            for (int i = 0; i < n1; i++)
                for (int j = 0; j < n2; j++)
                    u_temp[j][i] = u[j][i];
            LeaveCriticalSection(&cs);
            next_step(u_temp);
            EnterCriticalSection(&cs);
            for (int i = 0; i < n1; i++)
                for (int j = 0; j < n2; j++)
                    u[j][i] = u_temp[j][i];
            LeaveCriticalSection(&cs);
            Sleep(1000);
        }
    }
}

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT1));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT1);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}
//

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, L"Главное окно", WS_OVERLAPPEDWINDOW, 500, 100, 800, 600, nullptr, nullptr, hInst, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
}

int GetFileNameOpen(char name[200])
{
    OPENFILENAME ofn;
    TCHAR szFilter[100] = L"Текстовые файлы\0*.txt";
    memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFilter = szFilter;
    ofn.nFilterIndex = 1;
    ofn.lpstrFile = FileName;
    ofn.nMaxFile = 200;
    ofn.lpstrInitialDir = NULL;
    if (GetOpenFileName(&ofn))
    {
        wcstombs(name, FileName, sizeof(FileName));
        return 1;
    }
    else
        return 0;
}

INT_PTR CALLBACK Dialog1(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    TCHAR bufC1[5];
    TCHAR bufC2[5];
    TCHAR bufA[5];
    TCHAR bufB[5];
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
            GetDlgItemText(hDlg, IDC_EDIT1, bufC1, 4);
            GetDlgItemText(hDlg, IDC_EDIT2, bufC2, 4);
            GetDlgItemText(hDlg, IDC_EDIT3, bufA, 4);
            GetDlgItemText(hDlg, IDC_EDIT4, bufB, 4);
            C1 = _wtof(bufC1);
            C2 = _wtof(bufC1);
            a = _wtof(bufA);
            b = _wtof(bufB);
            init_RS(u);
            EndDialog(hDlg, 1);
            break;
        case IDCANCEL:
            EndDialog(hDlg, 0);
            break;
        }
        return (INT_PTR)TRUE;
    }
    return (INT_PTR)FALSE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    setlocale(LC_ALL, "rus");
    int PrFileName = 0;
    int PrInput = 0;
    static PARAM param;
    static HGLRC hRC;
    switch (message)
    {
    case WM_CREATE:
        InitializeCriticalSection(&cs);
        param.hEvent = CreateEvent(NULL, false, false, NULL);
        param.hWnd = hWnd;
        param.bKill = FALSE;
        _beginthread(Thread, 0, &param);
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            case ID_FAIL:
                char name[200];
                PrFileName = GetFileNameOpen(name);
                if (PrFileName)
                {
                    FILE* file;
                    size_t num;
                    file = fopen(name, "rt");
                    fscanf(file, "%f", &C1);
                    fscanf(file, "%f", &C2);
                    fscanf(file, "%f", &a);
                    fscanf(file, "%f", &b);
                    fclose(file);
                    init_RS(u);
                    SetTimer(hWnd, TIMER1, 1000, (TIMERPROC)NULL);
                }
                break;
            case ID_KLAV:
                PrInput = DialogBox(hInst, MAKEINTRESOURCE(DLG_INPUT), hWnd, Dialog1);
                SetTimer(hWnd, TIMER1, 1000, (TIMERPROC)NULL);
                break;
            case ID_START:
                param.bContinue = TRUE;
                SetEvent(param.hEvent);
                break;
            case ID_STOP:
                param.bContinue = FALSE;
                break;
            case ID_KILL:
                param.bKill = TRUE;
                param.bContinue = FALSE;
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_TIMER:
        if (wParam == TIMER1)
            InvalidateRect(hWnd, NULL, NULL);
        break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hDC = BeginPaint(hWnd, &ps);
        EnableOpenGL(hWnd, &hDC, &hRC);
        glEnable(GL_DEPTH_TEST);
        glClearColor(1.0, 1.0, 1.0, 1.1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glLoadIdentity();
        glOrtho(2, -2, -2, 2, -1, 1);
        glTranslatef(-0.5, 0.5, 0);
        glRotatef(30, 10, 5, 0);

        float vertices[7][3] = {
            {1, 1, -0.5},
            {1, -0.5, -0.5},
            {1, -0.5, 2},
            {-0.5, -0.5, -0.5},
            {-0.5, 1, -0.5},
            {-0.5, -0.5, 2},
            {1, 1, 2}
        };
        int edges[3][2] = {
            {0, 1},
            {1, 2},
            {1, 3}
        };
        int surfaces[3][4] = {
            {1, 3, 5, 2},
            {1, 0, 4, 3},
            {0, 1, 2, 6}
        };
        glColor3f(0.85, 0.85, 0.85);
        glBegin(GL_QUADS);
        for (int* surface : surfaces)
        {
            for (int i = 0; i < 4; i++)
                glVertex3f(vertices[surface[i]][0]+0.01, vertices[surface[i]][1] -0.01, vertices[surface[i]][2] - 0.01);
        }
        glEnd();
        float k1 = -1.4 / a;
        float k2 = 1.7 / b;
        float k3 = 1.5 / 100;
        glLineWidth(1);
        glColor3f(0.6, 0.6, 0.6);
        glBegin(GL_LINES);
        for (int i = 1; i < 10; i++)
        {
            glVertex3f(vertices[0][0] - i * 0.15, vertices[0][1], vertices[0][2]);
            glVertex3f(vertices[1][0] - i * 0.15, vertices[1][1], vertices[1][2]);
            glVertex3f(vertices[1][0] - i * 0.15, vertices[1][1], vertices[1][2]);
            glVertex3f(vertices[2][0] - i * 0.15, vertices[2][1], vertices[2][2]);
            glVertex3f(vertices[0][0], vertices[0][1], vertices[0][2] + i * 0.2);
            glVertex3f(vertices[1][0], vertices[1][1], vertices[1][2] + i * 0.2);
            glVertex3f(vertices[1][0], vertices[1][1] + i * 0.15, vertices[1][2]);
            glVertex3f(vertices[3][0], vertices[3][1] + i * 0.15, vertices[3][2]);
            glVertex3f(vertices[2][0], vertices[2][1] + i * 0.15, vertices[2][2]);
            glVertex3f(vertices[1][0], vertices[1][1] + i * 0.15, vertices[1][2]);
           glVertex3f(vertices[1][0], vertices[1][1], vertices[1][2] + i * 0.2);
            glVertex3f(vertices[3][0], vertices[3][1], vertices[3][2] + i * 0.2);
        }
        glEnd();
        glColor3f(0.0, 0.0, 0.0);
        glLineWidth(2);
        glBegin(GL_LINES);
        for (int* edge : edges)
            for (int i = 0; i < 2; i++)
                glVertex3fv(vertices[edge[i]]);
        glEnd();
        glLineWidth(2);
        glColor3f(1.0, 0.0, 0.0);
        EnterCriticalSection(&cs);        
        for (int j = 0; j < n2; j++)
        {
            glBegin(GL_LINE_STRIP);
            for (int i = 0; i < n1; i++)
                glVertex3f(x[i] * k1 + 1, u[j][i] * k3 -0.5, y[j] * k2 - 0.5);
            glEnd();
        }
        for (int j = 0; j < n1; j++)
        {
            glBegin(GL_LINE_STRIP);
            for (int i = 0; i < n2; i++)
                glVertex3f(x[j] * k1 +1, u[i][j] * k3 - 0.5, y[i] * k2 - 0.5);
            glEnd();
        }
        LeaveCriticalSection(&cs);
        SwapBuffers(hDC);

        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(hRC);
        ReleaseDC(hWnd, hDC);

        EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
