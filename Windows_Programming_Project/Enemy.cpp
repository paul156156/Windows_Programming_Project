#include "Enemy.h"
#include <cmath>

Enemy::Enemy(int x, int y, LPCWSTR imagePath)
    : x(x), y(y), health(3), destroyed(false), attackTimer(0), moveTimer(0), movingDown(true)
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
    // 이동 패턴 업데이트
    moveTimer += 50;

    // 적의 y 좌표가 화면의 1/2 지점에서 위아래로 움직이게 설정
    if (moveTimer >= 2000) // 2초마다 방향 전환
    {
        movingDown = !movingDown;
        moveTimer = 0;
    }

    if (movingDown)
    {
        y += 5; // 아래로 이동
    }
    else
    {
        y -= 5; // 위로 이동
    }

    // 좌우로 지그재그 이동
    x += 20 * sin(moveTimer * 3.14 / 180); // 주기를 조정하여 좌우 움직임 변경
}

void Enemy::Attack(std::vector<Bullet*>& bullets)
{
    // 일정 주기마다 총알을 발사
    attackTimer += 50; // 타이머 간격에 맞게 설정
    if (attackTimer >= 2000) // 2초마다 총알 발사
    {
        int bulletX = x + image->GetWidth() / 2 - 10;
        int bulletY = y + image->GetHeight();
        bullets.push_back(new Bullet(bulletX, bulletY, 1, L"resource\\image\\enemy_bullet.png")); // 아래로 발사
        attackTimer = 0;
    }
}
