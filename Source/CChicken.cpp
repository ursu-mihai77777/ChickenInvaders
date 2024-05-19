#include "CChicken.h"
#include "CBullet.h"

CChicken::CChicken()
{

}

CChicken::CChicken(const BackBuffer* pBackBuffer)
{
	m_pSprite = new Sprite("data/ChickenEnemy.bmp", RGB(0x00, 0x00, 0x00));
	m_pSprite->setBackBuffer(pBackBuffer);
}

CChicken::CChicken(const BackBuffer* pBackBuffer, Vec2 pos, Vec2 speed)
{
	m_pSprite = new Sprite("data/ChickenEnemy.bmp", RGB(0x00, 0x00, 0x00));
	m_pSprite->setBackBuffer(pBackBuffer);
	m_pSprite->mPosition = pos;
	m_pSpeed = speed;
}

CChicken::~CChicken()
{
	delete m_pSprite;
}

void CChicken::Draw()
{
	m_pSprite->draw();
}

void CChicken::Tick(float delta)
{
	if (m_pSprite->mPosition.x > 700 && m_pSpeed.x > 0) m_pSpeed.x = -m_pSpeed.x;
	else if (m_pSprite->mPosition.x < 100 && m_pSpeed.x < 0) m_pSpeed.x = -m_pSpeed.x;
	m_pSprite->mPosition.x += m_pSpeed.x;
	m_pSprite->mPosition.y += m_pSpeed.y;
}



CBullet* CChicken::CreateBullet(BackBuffer* buffer)
{
	float posx = 0;
	float posy = 0;
	CChickenBullet* bullet = new CChickenBullet(buffer, Vec2(0.0f, 1.0f));
	posx = m_pSprite->mPosition.x;
	posy = m_pSprite->mPosition.y + (round(bullet->m_pSprite->height() * 1.0f) / 2);
	bullet->m_pSprite->setScale(0.1f, 0.1f);
	bullet->SetPosition(posx, posy);
	return bullet;
}


void CChicken::SetPosition(float x, float y)
{
	m_pSprite->mPosition.x = x;
	m_pSprite->mPosition.y = y;
}

void CChicken::SetScale(float scaleX, float scaleY)
{
	m_pSprite->setScale(scaleX, scaleY);
}

