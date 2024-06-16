#pragma once
#include <windows.h>
#include <gdiplus.h>
#include <vector>
#include "Bullet.h"

using namespace Gdiplus;

class Enemy
{
public:
    Enemy(int x, int y, LPCWSTR imagePath);
    ~Enemy();
    void Draw(HDC hdc);
    void Move();
    void Attack(std::vector<Bullet*>& bullets, int playerX, int playerY);
    bool IsDestroyed() const { return destroyed; }
    int GetX() const { return x; }
    int GetY() const { return y; }
    void Destroy() { destroyed = true; }

private:
    int x, y;
    bool destroyed;
    Image* image;
};