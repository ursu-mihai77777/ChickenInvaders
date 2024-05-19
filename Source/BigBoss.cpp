#include "BigBoss.h"
#include "CBullet.h"

BigBoss::BigBoss()
{

}

BigBoss::BigBoss(const BackBuffer* pBackBuffer)
{
	m_pSprite = new Sprite("data/BigBossImgAndMask.bmp", RGB(0xff, 0x00, 0xff));
	m_pSprite->setBackBuffer(pBackBuffer);
}

BigBoss::BigBoss(const BackBuffer* pBackBuffer, Vec2 pos, Vec2 speed)
{
	m_pSprite = new Sprite("data/BigBossImgAndMask.bmp", RGB(0xff, 0x00, 0xff));
	m_pSprite->setBackBuffer(pBackBuffer);
	m_pSprite->mPosition = pos;
	m_pSpeed = speed;
}

BigBoss::~BigBoss()
{
	delete m_pSprite;
}

void BigBoss::Draw()
{
	m_pSprite->draw();
}

void BigBoss::Tick(float delta)
{
	if (m_pSprite->mPosition.x > 700 && m_pSpeed.x > 0) m_pSpeed.x = -m_pSpeed.x;
	else if (m_pSprite->mPosition.x < 100 && m_pSpeed.x < 0) m_pSpeed.x = -m_pSpeed.x;
	m_pSprite->mPosition.x += m_pSpeed.x;
	m_pSprite->mPosition.y += m_pSpeed.y;
}



CBullet* BigBoss::CreateBullet(BackBuffer* buffer)
{
	float posx = 0;
	float posy = 0;
	BigBossBullet* bullet = new BigBossBullet(buffer, Vec2(0.0f, 1.0f));
	posx = m_pSprite->mPosition.x;
	posy = m_pSprite->mPosition.y + (round(bullet->m_pSprite->height() * 1.0f) / 2);
	bullet->m_pSprite->setScale(0.1f, 0.1f);
	bullet->SetPosition(posx, posy);
	return bullet;
}


void BigBoss::SetPosition(float x, float y)
{
	m_pSprite->mPosition.x = x;
	m_pSprite->mPosition.y = y;
}

void BigBoss::SetScale(float scaleX, float scaleY)
{
	m_pSprite->setScale(scaleX, scaleY);
}

