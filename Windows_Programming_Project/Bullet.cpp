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
    y -= 10; // 총알이 위로 이동
}

bool Bullet::IsOffScreen() const
{
    return y + image->GetHeight() < 0; // 총알이 화면 위로 나가면 true 반환
}
