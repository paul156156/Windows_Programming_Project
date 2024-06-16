#include "Bullet.h"

Bullet::Bullet(int x, int y, LPCWSTR imagePath)
    : x(x), y(y)
{
    image = new Image(imagePath);
}

Bullet::~Bullet()
{
    delete image;
}

void Bullet::Draw(HDC hdc)
{
    Graphics graphics(hdc);
    graphics.DrawImage(image, x, y, image->GetWidth(), image->GetHeight());
}

void Bullet::Update()
{
    y -= 10; // �Ѿ��� ���� �̵�
}

bool Bullet::IsOffScreen() const
{
    return y + image->GetHeight() < 0; // �Ѿ��� ȭ�� ���� ������ true ��ȯ
}
