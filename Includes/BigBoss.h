#pragma once
#include "Main.h"
#include "Sprite.h"

class CBullet;

class BigBoss
{
public:
	BigBoss();
	BigBoss(const BackBuffer* pBackBuffer);
	BigBoss(const BackBuffer* pBackBuffer, Vec2, Vec2);
	virtual ~BigBoss();

	void Draw();
	void Tick(float);
	void SetPosition(float, float);
	void SetScale(float, float);
	CBullet* CreateBullet(BackBuffer*);

	Sprite* m_pSprite;
	Vec2 m_pSpeed;
};
