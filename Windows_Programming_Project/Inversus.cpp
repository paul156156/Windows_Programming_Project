#include <windows.h>
#include <vector>
#include <cmath>
#include "resource.h"

#define PI 3.14159265358979323846

// 전역 변수
HINSTANCE g_hInstance;
LPCTSTR IpszClass = L"Inversus Class";
LPCTSTR IpszWindowName = L"Inversus";
const int CELL_SIZE = 40;
const int PLAYER_SIZE = CELL_SIZE;  // 플레이어의 크기
const int BULLET_RADIUS = 5;  // 총알(원)의 반지름

bool gamePaused = false;

// Board 클래스 정의
class Board {
public:
    int width;
    int height;
    std::vector<std::vector<int>> cells;

    Board(int w, int h) : width(w), height(h) {
        InitializeBoard();
    }

    void InitializeBoard() {
        cells.clear();
        cells.resize(height, std::vector<int>(width, 0));
    }

    void Render(HDC hdc, HDC hdcMem, RECT clientRect) {
        int boardPixelWidth = width * CELL_SIZE;
        int boardPixelHeight = height * CELL_SIZE;
        int startX = (clientRect.right - boardPixelWidth) / 2;
        int startY = (clientRect.bottom - boardPixelHeight) / 2;

        HBRUSH hBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdcMem, hBrush);
        HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
        HPEN hOldPen = (HPEN)SelectObject(hdcMem, hPen);

        // Clear the background
        PatBlt(hdcMem, 0, 0, clientRect.right, clientRect.bottom, WHITENESS);

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                RECT rect;
                rect.left = startX + x * CELL_SIZE;
                rect.top = startY + y * CELL_SIZE;
                rect.right = rect.left + CELL_SIZE;
                rect.bottom = rect.top + CELL_SIZE;

                Rectangle(hdcMem, rect.left, rect.top, rect.right, rect.bottom);
            }
        }

        SelectObject(hdcMem, hOldBrush);
        SelectObject(hdcMem, hOldPen);
        DeleteObject(hBrush);
        DeleteObject(hPen);
    }

};

// Player 클래스 정의
class Player {
public:
    int x, y;  // 플레이어의 보드 위치
    int centerX, centerY;  // 플레이어의 중심 좌표
    double angle;  // 총알 회전 각도
    std::vector<bool> bullets;  // 총알 사용 여부 (true = 사용됨, false = 사용 안 됨)

    Player(int startX, int startY) : x(startX), y(startY), angle(0.0) {
        centerX = x * CELL_SIZE + CELL_SIZE / 2;
        centerY = y * CELL_SIZE + CELL_SIZE / 2;
        bullets = std::vector<bool>(6, false);
    }

    void Render(HDC hdc) {
        HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

        RECT playerRect;
        playerRect.left = centerX - PLAYER_SIZE / 2;
        playerRect.top = centerY - PLAYER_SIZE / 2;
        playerRect.right = centerX + PLAYER_SIZE / 2;
        playerRect.bottom = centerY + PLAYER_SIZE / 2;

        Rectangle(hdc, playerRect.left, playerRect.top, playerRect.right, playerRect.bottom);

        // Draw the inner circle
        DrawCircle(hdc, centerX, centerY, PLAYER_SIZE / 2 - 5);

        // Draw the rotating bullets
        double angleStep = 2 * PI / bullets.size();
        for (int i = 0; i < bullets.size(); ++i) {
            double angle = this->angle + i * angleStep;
            int bulletX = centerX + (int)((PLAYER_SIZE / 2 - 10) * cos(angle));
            int bulletY = centerY + (int)((PLAYER_SIZE / 2 - 10) * sin(angle));
            if (bullets[i]) {
                HBRUSH hBulletBrush = CreateSolidBrush(RGB(255, 255, 255));
                SelectObject(hdc, hBulletBrush);
                DrawCircle(hdc, bulletX, bulletY, BULLET_RADIUS);
                DeleteObject(hBulletBrush);
            }
            else {
                DrawCircle(hdc, bulletX, bulletY, BULLET_RADIUS);
            }
        }

        SelectObject(hdc, hOldBrush);
        DeleteObject(hBrush);
    }

    void Update() {
        angle += 0.1;  // 회전 속도 조절
        if (angle >= 2 * PI) {
            angle -= 2 * PI;
        }
    }

private:
    void DrawCircle(HDC hdc, int x, int y, int radius) {
        Ellipse(hdc, x - radius, y - radius, x + radius, y + radius);
    }
};

// 함수 선언
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void HandleCommand(WPARAM wParam, HWND hWnd);

// 전역 객체
Board* board = nullptr;
Player* player = nullptr;

// WinMain: 프로그램의 진입점
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR IpszCmdParam, int nCmdShow) {
    HWND hWnd;
    MSG Message;
    WNDCLASSEX WndClass;
    g_hInstance = hInstance;

    WndClass.cbSize = sizeof(WndClass);
    WndClass.style = CS_HREDRAW | CS_VREDRAW;
    WndClass.lpfnWndProc = (WNDPROC)WndProc;
    WndClass.cbClsExtra = 0;
    WndClass.cbWndExtra = 0;
    WndClass.hInstance = hInstance;
    WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    WndClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
    WndClass.lpszClassName = IpszClass;
    WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    RegisterClassEx(&WndClass);

    hWnd = CreateWindow(IpszClass, IpszWindowName, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 800, NULL, NULL, hInstance, NULL);
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // 초기 보드 및 플레이어 설정
    //board = new Board(9, 9);
    //player = new Player(board->width / 2, board->height / 2);

    // 타이머 설정
    SetTimer(hWnd, 1, 50, NULL);  // 50ms마다 타이머 이벤트 발생

    while (GetMessage(&Message, NULL, 0, 0)) {
        TranslateMessage(&Message);
        DispatchMessage(&Message);
    }

    delete board;
    delete player;

    return (int)Message.wParam;
}

// WM_COMMAND 메시지 처리
void HandleCommand(WPARAM wParam, HWND hWnd) {
    switch (LOWORD(wParam)) {
    case ID_GAME_START:
        gamePaused = false;
        break;
    case ID_GAME_PAUSE:
        gamePaused = true;
        break;
    case ID_GAME_QUIT:
        PostQuitMessage(0);
        break;
    case ID_DIFFICULTY_EASY:
        delete board;
        board = new Board(9, 9);
        delete player;
        player = new Player(board->width / 2, board->height / 2);
        InvalidateRect(hWnd, NULL, TRUE);
        UpdateWindow(hWnd);
        break;
    case ID_DIFFICULTY_NORMAL:
        delete board;
        board = new Board(13, 13);
        delete player;
        player = new Player(board->width / 2, board->height / 2);
        InvalidateRect(hWnd, NULL, TRUE);
        UpdateWindow(hWnd);
        break;
    case ID_DIFFICULTY_HARD:
        delete board;
        board = new Board(17, 17);
        delete player;
        player = new Player(board->width / 2, board->height / 2);
        InvalidateRect(hWnd, NULL, TRUE);
        UpdateWindow(hWnd);
        break;
    }
}

// 윈도우 프로시저
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static HBITMAP hBitmap;
    static HDC hdcMem;
    static HBITMAP hOldBitmap;
    PAINTSTRUCT ps;
    HDC hdc;
    RECT clientRect;

    switch (uMsg) {
    case WM_CREATE:
        hdc = GetDC(hWnd);
        hdcMem = CreateCompatibleDC(hdc);
        hBitmap = CreateCompatibleBitmap(hdc, 800, 800);
        hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);
        // 초기 보드 및 플레이어 설정
        board = new Board(9, 9);
        player = new Player(board->width / 2, board->height / 2);
        ReleaseDC(hWnd, hdc);


        break;

    case WM_COMMAND:
        HandleCommand(wParam, hWnd);
        break;

    case WM_SIZE:
        GetClientRect(hWnd, &clientRect);
        hdc = GetDC(hWnd);
        SelectObject(hdcMem, hOldBitmap);
        DeleteObject(hBitmap);
        hBitmap = CreateCompatibleBitmap(hdc, clientRect.right, clientRect.bottom);
        hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);
        ReleaseDC(hWnd, hdc);
        InvalidateRect(hWnd, NULL, TRUE);
        break;

    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        GetClientRect(hWnd, &clientRect);
        board->Render(hdc, hdcMem, clientRect);
        player->Render(hdcMem);
        BitBlt(hdc, 0, 0, clientRect.right, clientRect.bottom, hdcMem, 0, 0, SRCCOPY);
        EndPaint(hWnd, &ps);
        break;

    case WM_TIMER:
        if (!gamePaused) {
            player->Update();
            hdc = GetDC(hWnd);
            GetClientRect(hWnd, &clientRect);
            board->Render(hdc, hdcMem, clientRect);
            player->Render(hdcMem);
            BitBlt(hdc, 0, 0, clientRect.right, clientRect.bottom, hdcMem, 0, 0, SRCCOPY);
            ReleaseDC(hWnd, hdc);
        }
        break;

    case WM_DESTROY:
        SelectObject(hdcMem, hOldBitmap);
        DeleteObject(hBitmap);
        DeleteDC(hdcMem);
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
