#pragma once
#include "Main.h"
#include "Sprite.h"

class CHealth
{
public:
	CHealth();
	CHealth(const BackBuffer* pBackBuffer);
	CHealth(const BackBuffer* pBackBuffer, Vec2);
	virtual ~CHealth();

	Vec2& Position();
	Vec2& Velocity();
	Vec2  Size();

	void Draw();
	void Tick(float);
	void SetPosition(float, float);
	/*	void SaveObject(FILE* file);
	void LoadObject(FILE* file);*/
	bool IsOutside();

	Sprite* m_pSprite;
	Vec2 m_pSpeed;
	int m_iFrame;
};