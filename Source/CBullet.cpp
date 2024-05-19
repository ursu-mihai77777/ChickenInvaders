#include "CBullet.h"
#include "CGameApp.h"

extern CGameApp g_App;
CBullet::CBullet()
{

}

CBullet::CBullet(const BackBuffer* pBackBuffer)
{
	m_pSprite = new Sprite("data/bulletandmask.bmp", RGB(0xff, 0x00, 0xff));
	m_pSprite->setBackBuffer(pBackBuffer);
}

CBullet::CBullet(const BackBuffer* pBackBuffer, Vec2 speed)
{
	m_pSprite = new Sprite("data/bulletandmask.bmp", RGB(0xff, 0x00, 0xff));
	m_pSprite->setBackBuffer(pBackBuffer);
	m_pSpeed = speed;
}

CBullet::~CBullet()
{
	delete m_pSprite;
}

void CBullet::Draw()
{
	m_pSprite->draw();
}

void CBullet::Tick(float delta)
{
	m_pSprite->mPosition += m_pSpeed;
}

void CBullet::SetPosition(float x, float y)
{
	m_pSprite->mPosition.x = x;
	m_pSprite->mPosition.y = y;
}

bool CBullet::IsOutside()
{
	int width, height;
	g_App.GetWindowSize(width, height);
	Vec2 scale = m_pSprite->getScale();
	double pos = m_pSprite->mPosition.x - m_pSprite->width() * scale.x / 2;
	if (pos < 0)
		return true;

	pos = m_pSprite->mPosition.x + m_pSprite->width() * scale.x / 2;
	if (pos > (width + m_pSprite->width()))
		return true;

	pos = m_pSprite->mPosition.y - m_pSprite->height() * scale.y / 2;
	if (pos < 0)
		return true;

	pos = m_pSprite->mPosition.y + m_pSprite->height() * scale.y / 2;
	if (pos > (height + m_pSprite->width()))
		return true;

	return false;
}

bool CBullet::Intersects(Sprite* sprite)
{
	Vec2 pPosition = sprite->mPosition;
	Vec2 pSize = Vec2(sprite->width(), sprite->height());

	Vec2& position = m_pSprite->mPosition;
	Vec2 scale = m_pSprite->getScale();

	CBoundingBox box(m_pSprite->mPosition, m_pSprite->mPosition + Vec2(m_pSprite->width() * scale.x, m_pSprite->height() * scale.y));
	CBoundingBox chickenBox(pPosition, pPosition + pSize);

	if (chickenBox.Intersects(box))
		return true;
	return false;
}

CChickenBullet::CChickenBullet(const BackBuffer* pBackBuffer, Vec2 speed)
{
	m_pSprite = new Sprite("data/chickenbulletandmask.bmp", RGB(0xff, 0x00, 0xff));
	m_pSprite->setBackBuffer(pBackBuffer);
	m_pSpeed = speed;
}

BigBossBullet::BigBossBullet(const BackBuffer* pBackBuffer, Vec2 speed)
{
	m_pSprite = new Sprite("data/BulletBigBossAndMask.bmp", RGB(0xff, 0x00, 0xff));
	m_pSprite->setBackBuffer(pBackBuffer);
	m_pSpeed = speed;
}
