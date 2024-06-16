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
    virtual ~Enemy();
    virtual void Draw(HDC hdc);
    virtual void Move(); // virtual Ű���� �߰�
    virtual void Attack(std::vector<Bullet*>& bullets); // virtual Ű���� �߰�
    bool IsDestroyed() const { return destroyed; }
    void Destroy() { destroyed = true; }
    void TakeDamage() { health--; if (health <= 0) Destroy(); }

    int GetX() const { return x; }
    int GetY() const { return y; }
    int GetWidth() const { return image->GetWidth(); }
    int GetHeight() const { return image->GetHeight(); }

protected:
    int x, y;
    int health;
    bool destroyed;
    Image* image;
    int attackTimer; // �Ѿ� �߻� �ֱ⸦ ���� Ÿ�̸�
    int moveTimer;   // �̵� ������ ���� Ÿ�̸�
    bool movingDown; // ���Ʒ� ������ ���� �÷���
};
