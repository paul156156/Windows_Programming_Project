#include <windows.h>
#include <gdiplus.h>
#include <tchar.h>
#include <mmsystem.h>
#include <vector>
#include <algorithm>
#include "Fighter.h"
#include "Bullet.h"

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "winmm.lib")

using namespace Gdiplus;

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"RAIDEN";

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

const int winWidth = 500;
const int winHeight = 800;

ULONG_PTR gdiplusToken;
Fighter* playerFighter;
std::vector<Bullet*> bullets; // 총알들을 저장할 벡터

Image* LoadPNG(LPCWSTR filePath)
{
    return Image::FromFile(filePath);
}

void PlayBGM(LPCWSTR bgmFilePath)
{
    wchar_t command[256];
    wsprintf(command, L"open \"%s\" type mpegvideo alias bgm", bgmFilePath);
    mciSendString(command, NULL, 0, NULL);
    mciSendString(L"play bgm repeat", NULL, 0, NULL);
}

void UpdatePlayerFighter()
{
    if (GetAsyncKeyState(VK_LEFT) & 0x8000)
    {
        playerFighter->Move(-10, 0);
    }
    if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
    {
        playerFighter->Move(10, 0);
    }
    if (GetAsyncKeyState(VK_UP) & 0x8000)
    {
        playerFighter->Move(0, -10);
    }
    if (GetAsyncKeyState(VK_DOWN) & 0x8000)
    {
        playerFighter->Move(0, 10);
    }
    playerFighter->SetBoundary(0, 0, winWidth, winHeight);
}

void FireBullet()
{
    int x = playerFighter->GetX() + playerFighter->GetWidth() / 2 - 10;
    int y = playerFighter->GetY() - 10;
    bullets.push_back(new Bullet(x, y, L"resource\\image\\bullet.png"));
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
    HWND hWnd;
    MSG Message;
    WNDCLASSEX WndClass;
    g_hInst = hInstance;

    GdiplusStartupInput gdiplusStartupInput;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    WndClass.cbSize = sizeof(WndClass);
    WndClass.style = CS_HREDRAW | CS_VREDRAW;
    WndClass.lpfnWndProc = WndProc;
    WndClass.cbClsExtra = 0;
    WndClass.cbWndExtra = 0;
    WndClass.hInstance = hInstance;
    WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    WndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    WndClass.lpszMenuName = NULL;
    WndClass.lpszClassName = lpszClass;
    WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    RegisterClassEx(&WndClass);

    hWnd = CreateWindow(lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW, 500, 0, winWidth, winHeight, NULL, (HMENU)NULL, hInstance, NULL);
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // 배경 음악 재생
    PlayBGM(L"resource\\sound\\terran.mp3");

    // WinMain 함수 내 전투기 객체 생성 후 경계 설정 추가
    playerFighter = new Fighter(225, 700, L"resource\\image\\fighter.png");
    playerFighter->SetBoundary(0, 0, winWidth, winHeight); // 창 크기에 맞게 경계 설정

    SetTimer(hWnd, 1, 50, NULL);

    while (GetMessage(&Message, NULL, 0, 0))
    {
        TranslateMessage(&Message);
        DispatchMessage(&Message);
    }

    KillTimer(hWnd, 1);
    GdiplusShutdown(gdiplusToken);

    delete playerFighter;

    // 총알 객체들 삭제
    for (auto bullet : bullets)
    {
        delete bullet;
    }

    // 배경 음악 중지 및 닫기
    mciSendString(L"stop bgm", NULL, 0, NULL);
    mciSendString(L"close bgm", NULL, 0, NULL);

    return (int)Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    static Image* pBackgroundImage = nullptr;
    static int bgY = 0;
    const int bgSpeed = 4;
    static LPCWSTR imagePath = L"resource\\image\\bg.png"; // 이미지 파일 경로

    switch (iMessage)
    {
    case WM_CREATE:
        pBackgroundImage = LoadPNG(imagePath);
        break;

    case WM_TIMER:
        bgY += bgSpeed;
        if (bgY >= 3000) bgY = 0; // 3000은 이미지의 높이

        UpdatePlayerFighter();

        // 총알 업데이트
        for (auto bullet : bullets)
        {
            bullet->Update();
        }

        // 화면을 벗어난 총알 삭제
        bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](Bullet* bullet) {
            if (bullet->IsOffScreen())
            {
                delete bullet;
                return true;
            }
            return false;
            }), bullets.end());

        InvalidateRect(hWnd, NULL, FALSE);
        break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hDC = BeginPaint(hWnd, &ps);
        HDC hMemDC = CreateCompatibleDC(hDC);
        HBITMAP hMemBitmap = CreateCompatibleBitmap(hDC, winWidth, winHeight); // 윈도우 크기에 맞게 수정
        HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hMemBitmap);

        if (pBackgroundImage)
        {
            Graphics graphics(hMemDC);
            int imgWidth = pBackgroundImage->GetWidth();
            int imgHeight = pBackgroundImage->GetHeight();

            // 이미지의 상단 부분
            graphics.DrawImage(pBackgroundImage, 0, bgY - imgHeight, imgWidth, imgHeight);
            // 이미지의 하단 부분
            graphics.DrawImage(pBackgroundImage, 0, bgY, imgWidth, imgHeight);
        }

        // 전투기 그리기
        if (playerFighter)
        {
            playerFighter->Draw(hMemDC);
        }

        // 총알 그리기
        for (auto bullet : bullets)
        {
            bullet->Draw(hMemDC);
        }

        BitBlt(hDC, 0, 0, winWidth, winHeight, hMemDC, 0, 0, SRCCOPY); // 윈도우 크기에 맞게 수정

        SelectObject(hMemDC, hOldBitmap);
        DeleteObject(hMemBitmap);
        DeleteDC(hMemDC);

        EndPaint(hWnd, &ps);
    }
    break;

    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_SPACE:
            FireBullet();
            break;
        }
        InvalidateRect(hWnd, NULL, FALSE);
        break;

    case WM_DESTROY:
        delete pBackgroundImage;
        delete playerFighter;
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, iMessage, wParam, lParam);
    }

    return 0;
}
