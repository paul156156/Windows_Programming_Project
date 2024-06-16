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
    // �̵� ���� ������Ʈ
    moveTimer += 50;

    // ���� y ��ǥ�� ȭ���� 1/2 �������� ���Ʒ��� �����̰� ����
    if (moveTimer >= 2000) // 2�ʸ��� ���� ��ȯ
    {
        movingDown = !movingDown;
        moveTimer = 0;
    }

    if (movingDown)
    {
        y += 5; // �Ʒ��� �̵�
    }
    else
    {
        y -= 5; // ���� �̵�
    }

    // �¿�� ������� �̵�
    x += 20 * sin(moveTimer * 3.14 / 180); // �ֱ⸦ �����Ͽ� �¿� ������ ����
}

void Enemy::Attack(std::vector<Bullet*>& bullets)
{
    // ���� �ֱ⸶�� �Ѿ��� �߻�
    attackTimer += 50; // Ÿ�̸� ���ݿ� �°� ����
    if (attackTimer >= 2000) // 2�ʸ��� �Ѿ� �߻�
    {
        int bulletX = x + image->GetWidth() / 2 - 10;
        int bulletY = y + image->GetHeight();
        bullets.push_back(new Bullet(bulletX, bulletY, 1, L"resource\\image\\enemy_bullet.png")); // �Ʒ��� �߻�
        attackTimer = 0;
    }
}
