#pragma once
#include <windows.h>
#include <gdiplus.h>

using namespace Gdiplus;

class Bullet
{
public:
    Bullet(int x, int y, LPCWSTR imagePath);
    ~Bullet();
    void Draw(HDC hdc);
    void Update();
    bool IsOffScreen() const;

private:
    int x, y;
    Image* image;
};