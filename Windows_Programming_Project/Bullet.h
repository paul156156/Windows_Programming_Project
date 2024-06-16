#pragma once
#include <windows.h>
#include <gdiplus.h>

using namespace Gdiplus;

class Bullet
{
public:
    Bullet(int x, int y, int direction, LPCWSTR imagePath); // direction �߰�
    ~Bullet();
    void Draw(HDC hdc);
    void Update();
    bool IsOffScreen() const;
    int GetDirection() const { return direction; }
    void Destroy() { destroyed = true; }
    bool IsDestroyed() const { return destroyed; }

    int GetX() const { return x; }
    int GetY() const { return y; }
    int GetWidth() const { return image->GetWidth(); }
    int GetHeight() const { return image->GetHeight(); }

private:
    int x, y;
    int direction; // ����: -1�� ��, 1�� �Ʒ�
    bool destroyed;
    Image* image;
};
