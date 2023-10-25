#include <Windows.h>
#include <iostream>
#include <stdio.h>
#include <WinUser.h>


#define IDM_MOUSELOGGER 1001
#define IDM_KEYLOGGER 1002
#define IDM_EXIT 1003
#define ID_MOUSE_PAUSE_BUTTON 1004
#define ID_MOUSE_EXIT_BUTTON 1005
#define ID_KEYBOARD_PAUSE_BUTTON 1006
#define ID_KEYBOARD_EXIT_BUTTON 1007



HWND hwndMouseLogger = NULL;
HWND hwndKeyLogger = NULL;
HWND hwndTextBox;
HWND hwndMain = NULL;

static HWND hwndMouseEdit = NULL;
static HWND hwndMousePauseButton = NULL;
static HWND hwndMouseExitButton = NULL;
static BOOL isPausedMouse = FALSE;

static HWND hwndKeyboardEdit = NULL;
static HWND hwndKeyboardPauseButton = NULL;
static HWND hwndKeyboardExitButton = NULL;
static BOOL isPausedKeyboard = FALSE;

HHOOK mouseHook;
HHOOK keyboardHook;

// Клас для вікна MouseLogger
const char* szMouseLoggerClass = "MouseLoggerClass";

// Клас для вікна KeyLogger
const char* szKeyLoggerClass = "KeyLoggerClass";

// Прототип функцій обробників вікон
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MouseLoggerProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK KeyLoggerProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Функції обробки глобального гука на мишу
LRESULT CALLBACK LeftMouseHookCallback(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0 && !isPausedMouse) {
        if (wParam == WM_LBUTTONDOWN) {
            // Лівий клік миші
            MOUSEHOOKSTRUCT* pMouseStruct = (MOUSEHOOKSTRUCT*)lParam;
            int x = pMouseStruct->pt.x;
            int y = pMouseStruct->pt.y;

           

            // Додаємо подію до текстового поля вікна MouseLogger
            char text[100];
            sprintf_s(text, sizeof(text), "Left mouse button down at (%d, %d)\r\n", x, y);
            HWND hwndEdit = FindWindowEx(hwndMouseLogger, NULL, "EDIT", NULL);
            SendMessageA(hwndEdit, EM_SETSEL, -1, -1); // Вибираємо весь текст
            SendMessageA(hwndEdit, EM_REPLACESEL, FALSE, (LPARAM)text); // Додаємо текст
        }
    }

    return CallNextHookEx(mouseHook, nCode, wParam, lParam);
}

LRESULT CALLBACK RightMouseHookCallback(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0 && !isPausedMouse) {
        if (wParam == WM_RBUTTONDOWN) {
            // Правий клік миші
            MOUSEHOOKSTRUCT* pMouseStruct = (MOUSEHOOKSTRUCT*)lParam;
            int x = pMouseStruct->pt.x;
            int y = pMouseStruct->pt.y;

            

            // Додаємо подію до текстового поля вікна MouseLogger
            char text[100];
            sprintf_s(text, sizeof(text), "Right mouse button down at (%d, %d)\r\n", x, y);
            HWND hwndEdit = FindWindowEx(hwndMouseLogger, NULL, "EDIT", NULL);
            SendMessageA(hwndEdit, EM_SETSEL, -1, -1); // Вибираємо весь текст
            SendMessageA(hwndEdit, EM_REPLACESEL, FALSE, (LPARAM)text); // Додаємо текст
        }
    }

    return CallNextHookEx(mouseHook, nCode, wParam, lParam);
}

LRESULT CALLBACK MiddleMouseHookCallback(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0 && !isPausedMouse) {
        if (wParam == WM_MBUTTONDOWN) {
            // Середній клік миші
            MOUSEHOOKSTRUCT* pMouseStruct = (MOUSEHOOKSTRUCT*)lParam;
            int x = pMouseStruct->pt.x;
            int y = pMouseStruct->pt.y;

            

            // Додаємо подію до текстового поля вікна MouseLogger
            char text[100];
            sprintf_s(text, sizeof(text), "Middle mouse button down at (%d, %d)\r\n", x, y);
            HWND hwndEdit = FindWindowEx(hwndMouseLogger, NULL, "EDIT", NULL);
            SendMessageA(hwndEdit, EM_SETSEL, -1, -1); // Вибираємо весь текст
            SendMessageA(hwndEdit, EM_REPLACESEL, FALSE, (LPARAM)text); // Додаємо текст
        }
    }

    return CallNextHookEx(mouseHook, nCode, wParam, lParam);
}

// Функція обробки глобального гука на клавіатуру
LRESULT CALLBACK KeyboardHookCallback(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0 && !isPausedKeyboard) {
        if (wParam == WM_KEYDOWN) {
            // Отримуємо код натиснутої клавіші
            KBDLLHOOKSTRUCT* pKeyStruct = (KBDLLHOOKSTRUCT*)lParam;
            DWORD scanCode = pKeyStruct->scanCode;

            // Конвертуємо код клавіші у рядок
            char keyText[16];
            GetKeyNameTextA(scanCode << 16, keyText, sizeof(keyText));

            // Додаємо натиснуту клавішу до текстового поля вікна KeyLogger
            HWND hwndEdit = FindWindowEx(hwndKeyLogger, NULL, "EDIT", NULL);
            char text[32];
            sprintf_s(text, sizeof(text), "Key pressed: %s\r\n", keyText);
            SendMessageA(hwndEdit, EM_SETSEL, -1, -1); 
            SendMessageA(hwndEdit, EM_REPLACESEL, FALSE, (LPARAM)text); 
        }
    }

    return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
}

void SetMouseHook() {
    mouseHook = SetWindowsHookEx(WH_MOUSE_LL, LeftMouseHookCallback, NULL, 0);
    SetWindowsHookEx(WH_MOUSE_LL, RightMouseHookCallback, NULL, 0);
    SetWindowsHookEx(WH_MOUSE_LL, MiddleMouseHookCallback, NULL, 0);
}

void UnhookMouse() {
    UnhookWindowsHookEx(mouseHook);
}

void SetKeyboardHook() {
    keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookCallback, NULL, 0);
}

void UnhookKeyboard() {
    UnhookWindowsHookEx(keyboardHook);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    // Ініціалізація структури WNDCLASSEX для головного вікна
    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = "MyWindowClass"; // Унікальне ім'я класу вікна

    // Реєстрація класу головного вікна
    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, "Window class registration failed!", "Error", MB_ICONERROR);
        return 1;
    }

    // Створення головного вікна
    hwndMain = CreateWindow("MyWindowClass", "Program", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL, hInstance, NULL);

    // Створення меню
    HMENU hMenu = CreateMenu();
    AppendMenu(hMenu, MF_STRING, IDM_MOUSELOGGER, "Mouse Logger");
    AppendMenu(hMenu, MF_STRING, IDM_KEYLOGGER, "Key Logger");
    AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hMenu, MF_STRING, IDM_EXIT, "Exit");

    
    

    if (!hwndMain) {
        MessageBox(NULL, "Window creation failed!", "Error", MB_ICONERROR);
        return 1;
    }
    

    SetMenu(hwndMain, hMenu);

    // Відображення головного вікна
    ShowWindow(hwndMain, nCmdShow);
    UpdateWindow(hwndMain);

    // Основний цикл обробки повідомлень
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Знімаємо глобальний гук на мишу перед виходом
    UnhookMouse();

    return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDM_MOUSELOGGER:

                    // Обробка натиснення пункту меню MouseLogger
                    if (hwndMouseLogger == NULL) {

                        // Створення та відображення вікна MouseLogger
                        WNDCLASSEX wcMouseLogger = {0};
                        wcMouseLogger.cbSize = sizeof(WNDCLASSEX);
                        wcMouseLogger.style = CS_HREDRAW | CS_VREDRAW;
                        wcMouseLogger.lpfnWndProc = MouseLoggerProc;
                        wcMouseLogger.hInstance = GetModuleHandle(NULL);
                        wcMouseLogger.hCursor = LoadCursor(NULL, IDC_ARROW);
                        wcMouseLogger.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
                        wcMouseLogger.lpszClassName = szMouseLoggerClass;
                        RegisterClassEx(&wcMouseLogger);


                        hwndMouseLogger = CreateWindow(szMouseLoggerClass, "Mouse Logger", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 600, 400, NULL, NULL, GetModuleHandle(NULL), NULL);
                        ShowWindow(hwndMouseLogger, SW_SHOW);
                    }   else {

                            // Вікно MouseLogger вже існує, активуємо його
                            ShowWindow(hwndMouseLogger, SW_RESTORE);
                            SetForegroundWindow(hwndMouseLogger);
                            SetMouseHook();
                    }

                    break;
                case IDM_KEYLOGGER:

                    // Обробка натиснення пункту меню Key Logger
                    if (hwndKeyLogger == NULL) {

                        // Створення та відображення вікна KeyLogger
                        WNDCLASSEX wcKeyLogger = {0};
                        wcKeyLogger.cbSize = sizeof(WNDCLASSEX);
                        wcKeyLogger.style = CS_HREDRAW | CS_VREDRAW;
                        wcKeyLogger.lpfnWndProc = KeyLoggerProc;
                        wcKeyLogger.hInstance = GetModuleHandle(NULL);
                        wcKeyLogger.hCursor = LoadCursor(NULL, IDC_ARROW);
                        wcKeyLogger.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
                        wcKeyLogger.lpszClassName = szKeyLoggerClass;
                        RegisterClassEx(&wcKeyLogger);

                        hwndKeyLogger = CreateWindow(szKeyLoggerClass, "Key Logger", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 600, 400, NULL, NULL, GetModuleHandle(NULL), NULL);
                        ShowWindow(hwndKeyLogger, SW_SHOW);
                    }   else {

                            // Вікно KeyboardLogger вже існує, активуємо його
                            ShowWindow(hwndKeyLogger, SW_RESTORE);
                            SetForegroundWindow(hwndKeyLogger);
                            SetKeyboardHook();
                    }
                    break;
                case IDM_EXIT:

                    // Обробка натиснення пункту меню Exit
                    DestroyWindow(hwnd);
                break;
            }
        break;       

        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK MouseLoggerProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    switch (msg) {
        case WM_CREATE:

            // Створення текстового поля з фіксованим розміром та автоматичною прокруткою
            hwndMouseEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_MULTILINE | WS_VSCROLL, 10, 30, 360, 230, hwnd, NULL, GetModuleHandle(NULL), NULL);
            hwndMousePauseButton = CreateWindow("BUTTON", "Pause", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 270, 100, 30, hwnd, (HMENU)ID_MOUSE_PAUSE_BUTTON, GetModuleHandle(NULL), NULL);
            hwndMouseExitButton = CreateWindow("BUTTON", "Exit", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 140, 270, 100, 30, hwnd, (HMENU)ID_MOUSE_EXIT_BUTTON, GetModuleHandle(NULL), NULL);

            SetMouseHook();
        break;

        case WM_COMMAND:
            if (LOWORD(wParam) == ID_MOUSE_PAUSE_BUTTON) {

                // Обробка натискання кнопки "Pause/Continue"
                if (isPausedMouse) {
                    isPausedMouse = FALSE;
                    SetWindowText(hwndMousePauseButton, "Pause"); 
                } else {
                    isPausedMouse = TRUE;
                    SetWindowText(hwndMousePauseButton, "Continue"); 
                }
            } else if (LOWORD(wParam) == ID_MOUSE_EXIT_BUTTON) {

                // Обробка натискання кнопки "Exit"
                UnhookMouse();
                isPausedMouse = FALSE;
                hwndMouseLogger = NULL;
                DestroyWindow(hwnd);
            }
        break;

        case WM_CLOSE:
            
            // Обробка повідомлення про закриття вікна MouseLogger
            UnhookMouse();
            isPausedMouse = FALSE;
            hwndMouseLogger = NULL;
            DestroyWindow(hwnd);
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK KeyLoggerProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE:

            // Створення текстового поля з фіксованим розміром та автоматичною прокруткою
            hwndKeyboardEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_MULTILINE | WS_VSCROLL, 10, 30, 360, 230, hwnd, NULL, GetModuleHandle(NULL), NULL);
            hwndKeyboardPauseButton = CreateWindow("BUTTON", "Pause", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 270, 100, 30, hwnd, (HMENU)ID_KEYBOARD_PAUSE_BUTTON, GetModuleHandle(NULL), NULL);
            hwndKeyboardExitButton = CreateWindow("BUTTON", "Exit", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 140, 270, 100, 30, hwnd, (HMENU)ID_KEYBOARD_EXIT_BUTTON, GetModuleHandle(NULL), NULL);

            SetKeyboardHook();
        break;

        case WM_COMMAND:
            if (LOWORD(wParam) == ID_KEYBOARD_PAUSE_BUTTON) {

                // Обробка натискання кнопки "Pause/Continue"
                if (isPausedKeyboard) {
                    isPausedKeyboard = FALSE;
                    SetWindowText(hwndKeyboardPauseButton, "Pause");
                } else {
                    isPausedKeyboard = TRUE;
                    SetWindowText(hwndKeyboardPauseButton, "Continue");
                }
            } else if (LOWORD(wParam) == ID_KEYBOARD_EXIT_BUTTON) {

                // Обробка натискання кнопки "Exit"
                hwndKeyLogger = NULL; 
                isPausedKeyboard = FALSE;
                UnhookKeyboard();
                DestroyWindow(hwnd);
            }
        break;

        case WM_CLOSE:    

            // Обробка повідомлення про закриття вікна MouseLogger      
            hwndKeyLogger = NULL;
            isPausedKeyboard = FALSE;
            UnhookKeyboard();            
            DestroyWindow(hwnd);
        break;


        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}
