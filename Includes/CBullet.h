#pragma once

#include "Main.h"
#include "Sprite.h"
#include "CBoundingBox.inl"

class CBullet {
public:
	CBullet();
	CBullet(const BackBuffer* pBackBuffer);
	CBullet(const BackBuffer* pBackBuffer, Vec2);
	virtual ~CBullet();
	virtual int GetType() { return 0; }

	void Draw();
	void Tick(float);
	void SetPosition(float, float);
	bool IsOutside();
	bool Intersects(Sprite*);

	Sprite* m_pSprite;
	Vec2 m_pSpeed;

};

class CChickenBullet : public CBullet {
public:
	CChickenBullet(const BackBuffer* pBackBuffer, Vec2);
	int GetType() override { return 1; }
};

class BigBossBullet : public CBullet {
public:
	BigBossBullet(const BackBuffer* pBackBuffer, Vec2);
	int GetType() override { return 1; }
};