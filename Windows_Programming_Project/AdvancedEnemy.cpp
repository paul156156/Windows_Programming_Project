#include "AdvancedEnemy.h"
#include <cmath>

AdvancedEnemy::AdvancedEnemy(int x, int y, LPCWSTR imagePath)
    : Enemy(x, y, imagePath)
{
}

void AdvancedEnemy::Move()
{
    // 고급 적의 이동 패턴을 정의합니다.
    moveTimer += 50;

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
    x += 30 * sin(moveTimer * 3.14 / 180); // 주기를 조정하여 좌우 움직임 변경
}

void AdvancedEnemy::Attack(std::vector<Bullet*>& bullets)
{
    attackTimer += 50;
    if (attackTimer >= 1000) // 1초마다 총알 발사
    {
        int bulletX = x + image->GetWidth() / 2 - 10;
        int bulletY = y + image->GetHeight();
        bullets.push_back(new Bullet(bulletX, bulletY, 1, L"resource\\image\\advanced_enemy_bullet.png")); // 고급 적의 총알 이미지
        attackTimer = 0;
    }
}
