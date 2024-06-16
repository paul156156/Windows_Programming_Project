#include <windows.h>
#include <gdiplus.h>
#include <tchar.h>
#include <mmsystem.h>
#include <vector>
#include <algorithm>
#include "Fighter.h"
#include "Enemy.h"
#include "Bullet.h"

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "winmm.lib")

using namespace Gdiplus;

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"RAIDEN";

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

const int winWidth = 700;
const int winHeight = 800;

ULONG_PTR gdiplusToken;
Fighter* playerFighter = nullptr;
std::vector<Bullet*> bullets; // �Ѿ˵��� ������ ����
std::vector<Enemy*> enemies; // ������ ������ ����
Image* lifeImage = nullptr; // ���� �� �̹���
int score = 0; // ���� ����

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
    if (playerFighter == nullptr) return; // ��ü�� nullptr���� Ȯ��

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
    playerFighter->SetBoundary(0, 0, winWidth - 200, winHeight); // ���� �÷��� ������ ����
}

void FireBullet()
{
    if (playerFighter == nullptr) return; // ��ü�� nullptr���� Ȯ��

    int x = playerFighter->GetX() + playerFighter->GetWidth() / 2 - 10;
    int y = playerFighter->GetY() - 10;

    // ������ 1000�� �̻��� �� �Ѿ��� �� �� �߻�
    if (score >= 1000)
    {
        bullets.push_back(new Bullet(x - 40, y, -1, L"resource\\image\\bullet.png")); // ���� �Ѿ�
        bullets.push_back(new Bullet(x, y, -1, L"resource\\image\\bullet.png")); // ������ �Ѿ�
    }
    else
    {
        bullets.push_back(new Bullet(x, y, -1, L"resource\\image\\bullet.png"));
    }
}

void CreateEnemy()
{
    int x = rand() % (winWidth - 250); // ���� x ��ġ�� �����ϰ� ����
    enemies.push_back(new Enemy(x, 0, L"resource\\image\\enemy.png"));
}

bool CheckCollision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
    return !(x1 > x2 + w2 || x1 + w1 < x2 || y1 > y2 + h2 || y1 + h1 < y2);
}

void CheckCollisions()
{
    if (playerFighter == nullptr) return; // ��ü�� nullptr���� Ȯ��

    for (auto bullet : bullets)
    {
        // �÷��̾�� ���� �Ѿ� �浹
        if (bullet->GetDirection() == 1 && CheckCollision(playerFighter->GetX(), playerFighter->GetY(), playerFighter->GetWidth(), playerFighter->GetHeight(),
            bullet->GetX(), bullet->GetY(), bullet->GetWidth(), bullet->GetHeight()))
        {
            playerFighter->TakeDamage();
            bullet->Destroy();
            if (playerFighter->GetLives() <= 0)
            {
                PostQuitMessage(0); // �÷��̾ ����
            }
        }
    }

    for (auto enemy : enemies)
    {
        for (auto bullet : bullets)
        {
            if (bullet->GetDirection() == -1 && CheckCollision(enemy->GetX(), enemy->GetY(), enemy->GetWidth(), enemy->GetHeight(),
                bullet->GetX(), bullet->GetY(), bullet->GetWidth(), bullet->GetHeight()))
            {
                enemy->TakeDamage();
                bullet->Destroy();
                if (enemy->IsDestroyed())
                {
                    score += 10; // ���� �׿��� �� ���� ����
                }
            }
        }

        // ���� �÷��̾� �浹
        if (CheckCollision(playerFighter->GetX(), playerFighter->GetY(), playerFighter->GetWidth(), playerFighter->GetHeight(),
            enemy->GetX(), enemy->GetY(), enemy->GetWidth(), enemy->GetHeight()))
        {
            PostQuitMessage(0); // �÷��̾ ����
        }
    }

    // ȭ���� ��� �Ѿ� ����
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](Bullet* bullet) {
        if (bullet->IsOffScreen() || bullet->IsDestroyed())
        {
            delete bullet;
            return true;
        }
        return false;
        }), bullets.end());

    // �ı��� �� ����
    enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](Enemy* enemy) {
        if (enemy->IsDestroyed())
        {
            delete enemy;
            return true;
        }
        return false;
        }), enemies.end());
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

    // ��� ���� ���
    PlayBGM(L"resource\\sound\\terran.mp3");

    // WinMain �Լ� �� ������ ��ü ���� �� ��� ���� �߰�
    playerFighter = new Fighter(225, 700, L"resource\\image\\fighter.png");
    if (!playerFighter) {
        MessageBox(hWnd, L"Player fighter initialization failed!", L"Error", MB_OK);
        PostQuitMessage(0);
    }
    playerFighter->SetBoundary(0, 0, winWidth, winHeight); // â ũ�⿡ �°� ��� ����

    // �� ��ü �ʱ� ����
    CreateEnemy();

    SetTimer(hWnd, 1, 50, NULL);
    SetTimer(hWnd, 2, 1000, NULL); // 1�ʸ��� ���ο� �� ����

    while (GetMessage(&Message, NULL, 0, 0))
    {
        TranslateMessage(&Message);
        DispatchMessage(&Message);
    }

    KillTimer(hWnd, 1);
    KillTimer(hWnd, 2);
    GdiplusShutdown(gdiplusToken);

    delete playerFighter;

    // �Ѿ� ��ü�� ����
    for (auto bullet : bullets)
    {
        delete bullet;
    }

    // �� ��ü�� ����
    for (auto enemy : enemies)
    {
        delete enemy;
    }

    // ��� ���� ���� �� �ݱ�
    mciSendString(L"stop bgm", NULL, 0, NULL);
    mciSendString(L"close bgm", NULL, 0, NULL);

    return (int)Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    static Image* pBackgroundImage = nullptr;
    static int bgY = 0;
    const int bgSpeed = 4;
    static LPCWSTR imagePath = L"resource\\image\\bg.png"; // �̹��� ���� ���

    switch (iMessage)
    {
    case WM_CREATE:
        pBackgroundImage = LoadPNG(imagePath);
        lifeImage = LoadPNG(L"resource\\image\\life.png"); // ���� �� �̹��� �ε�
        if (!pBackgroundImage || !lifeImage) {
            MessageBox(hWnd, L"Image load failed!", L"Error", MB_OK);
            PostQuitMessage(0);
        }
        break;

    case WM_TIMER:
        if (wParam == 1) // ���� ������Ʈ Ÿ�̸�
        {
            score += 1; // ���� ����
            bgY += bgSpeed;
            if (bgY >= 3000) bgY = 0; // 3000�� �̹����� ����

            UpdatePlayerFighter();

            for (auto enemy : enemies)
            {
                enemy->Move();
                enemy->Attack(bullets);
            }

            CheckCollisions();

            // �Ѿ� ������Ʈ
            for (auto bullet : bullets)
            {
                bullet->Update();
            }

            // ȭ���� ��� �Ѿ� ����
            bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](Bullet* bullet) {
                if (bullet->IsOffScreen())
                {
                    delete bullet;
                    return true;
                }
                return false;
                }), bullets.end());

            InvalidateRect(hWnd, NULL, FALSE);
        }
        else if (wParam == 2) // �� ���� Ÿ�̸�
        {
            CreateEnemy();
        }
        break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hDC = BeginPaint(hWnd, &ps);
        HDC hMemDC = CreateCompatibleDC(hDC);
        HBITMAP hMemBitmap = CreateCompatibleBitmap(hDC, winWidth, winHeight); // ������ ũ�⿡ �°� ����
        HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hMemBitmap);

        if (pBackgroundImage)
        {
            Graphics graphics(hMemDC);
            int imgWidth = pBackgroundImage->GetWidth();
            int imgHeight = pBackgroundImage->GetHeight();

            // �̹����� ��� �κ�
            graphics.DrawImage(pBackgroundImage, 0, bgY - imgHeight, imgWidth, imgHeight);
            // �̹����� �ϴ� �κ�
            graphics.DrawImage(pBackgroundImage, 0, bgY, imgWidth, imgHeight);
        }

        // ������ �׸���
        if (playerFighter)
        {
            playerFighter->Draw(hMemDC);
        }

        // �� �׸���
        for (auto enemy : enemies)
        {
            enemy->Draw(hMemDC);
        }

        // �Ѿ� �׸���
        for (auto bullet : bullets)
        {
            bullet->Draw(hMemDC);
        }

        // ���� ǥ��
        SetBkMode(hMemDC, TRANSPARENT);
        SetTextColor(hMemDC, RGB(255, 255, 255));
        HFONT hFont = CreateFont(24, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Arial");
        HFONT hOldFont = (HFONT)SelectObject(hMemDC, hFont);
        wchar_t scoreText[50];
        swprintf_s(scoreText, L"Score: %d", score);
        TextOut(hMemDC, 520, 20, scoreText, wcslen(scoreText)); // ������ �����ʿ� ǥ��
        SelectObject(hMemDC, hOldFont);
        DeleteObject(hFont);

        // ���� �� ǥ��
        if (playerFighter)
        {
            for (int i = 0; i < playerFighter->GetLives(); ++i)
            {
                Graphics graphics(hMemDC);
                graphics.DrawImage(lifeImage, 520 + i * 40, 60, lifeImage->GetWidth(), lifeImage->GetHeight());
            }
        }

        BitBlt(hDC, 0, 0, winWidth, winHeight, hMemDC, 0, 0, SRCCOPY); // ������ ũ�⿡ �°� ����

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
        delete lifeImage; // ���� �� �̹��� ����
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, iMessage, wParam, lParam);
    }

    return 0;
}
