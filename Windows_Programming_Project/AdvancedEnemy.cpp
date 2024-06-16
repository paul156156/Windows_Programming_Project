#include "AdvancedEnemy.h"
#include <cmath>

AdvancedEnemy::AdvancedEnemy(int x, int y, LPCWSTR imagePath)
    : Enemy(x, y, imagePath)
{
}

void AdvancedEnemy::Move()
{
    // ��� ���� �̵� ������ �����մϴ�.
    moveTimer += 50;

    if (moveTimer >= 2000) // 2�ʸ��� ���� ��ȯ
    {
        movingDown = !movingDown;
        moveTimer = 0;
    }

    if (movingDown)
    {
        x -= 5; // ���������� �̵�
        y += 5; // �Ʒ��� �̵�
    }
    else
    {
        x += 5; // �������� �̵�
        y -= 5; // ���� �̵�
    }

    // �¿�� ������� �̵�
    //x += 30 * sin(moveTimer * 3.14 / 180); // �ֱ⸦ �����Ͽ� �¿� ������ ����

    // ���� �̵��� ȭ�� ���� ���� (���� ũ�⿡ �°� ����)
    if (x < 0) x = 0;
    if (x + image->GetWidth() > 500) x = 500 - image->GetWidth(); // ȭ�� �ʺ� �°� ����
    if (y < 0) y = 0;
    if (y + image->GetHeight() > 400) y = 400 - image->GetHeight(); // ȭ�� ������ 1/2 ������ �°� ����
}

void AdvancedEnemy::Attack(std::vector<Bullet*>& bullets)
{
    attackTimer += 50;
    if (attackTimer >= 1000) // 1�ʸ��� �Ѿ� �߻�
    {
        int bulletX = x + image->GetWidth() / 2 - 10;
        int bulletY = y + image->GetHeight();
        bullets.push_back(new Bullet(bulletX, bulletY, 1, L"resource\\image\\advanced_enemy_bullet.png")); // ��� ���� �Ѿ� �̹���
        attackTimer = 0;
    }
}
