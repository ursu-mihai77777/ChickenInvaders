#pragma once
#include "Main.h"
#include "Sprite.h"

class CBullet;

class CChicken
{
public:
	CChicken();
	CChicken(const BackBuffer* pBackBuffer);
	CChicken(const BackBuffer* pBackBuffer, Vec2, Vec2);
	virtual ~CChicken();

	void Draw();
	void Tick(float);
	void SetPosition(float, float);
	void SetScale(float, float);
	CBullet* CreateBullet(BackBuffer*);

	Sprite* m_pSprite;
	Vec2 m_pSpeed;
};
