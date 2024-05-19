#include "CHealth.h"

CHealth::CHealth()
{

}

CHealth::~CHealth()
{
	delete m_pSprite;
}

CHealth::CHealth(const BackBuffer* pBackBuffer)
{
	m_pSprite = new Sprite("data/Health.bmp", RGB(0xff, 0xff, 0xff));
	m_pSprite->setBackBuffer(pBackBuffer);
}

CHealth::CHealth(const BackBuffer* pBackBuffer, Vec2 pos)
{
	m_pSprite = new Sprite("data/Health.bmp", RGB(0xff, 0xff, 0xff));
	m_pSprite->setBackBuffer(pBackBuffer);
	m_pSprite->mPosition = pos;
}

Vec2& CHealth::Position()
{
	return m_pSprite->mPosition;
}

Vec2& CHealth::Velocity()
{
	return m_pSprite->mVelocity;
}

Vec2 CHealth::Size()
{
	return Vec2(m_pSprite->width(), m_pSprite->height());
}

void CHealth::Draw()
{
	m_pSprite->draw();
}

void CHealth::Tick(float delta)
{
	m_pSprite->mPosition.x += m_pSpeed.x;
	m_pSprite->mPosition.y += m_pSpeed.y;
}

void CHealth::SetPosition(float x, float y)
{
	m_pSprite->mPosition.x = x;
	m_pSprite->mPosition.y = y;
}