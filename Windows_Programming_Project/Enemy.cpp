#include "Enemy.h"

Enemy::Enemy(int x, int y, LPCWSTR imagePath)
    : x(x), y(y), destroyed(false)
{
    image = new Image(imagePath);
}

Enemy::~Enemy()
{
    delete image;
}

void Enemy::Draw(HDC hdc)
{
    Graphics graphics(hdc);
    graphics.DrawImage(image, x, y, image->GetWidth(), image->GetHeight());
}

void Enemy::Move()
{
    y += 5; // 적이 아래로 이동
}

void Enemy::Attack(std::vector<Bullet*>& bullets, int playerX, int playerY)
{
    int bulletX = x + image->GetWidth() / 2 - 5;
    int bulletY = y + image->GetHeight();
    bullets.push_back(new Bullet(bulletX, bulletY, L"resource\\image\\Ebullet.png"));
}