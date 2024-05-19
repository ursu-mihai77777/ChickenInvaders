//-----------------------------------------------------------------------------
// File: CPlayer.cpp
//
// Desc: This file stores the player object class. This class performs tasks
//       such as player movement, some minor physics as well as rendering.
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CPlayer Specific Includes
//-----------------------------------------------------------------------------
#include "CPlayer.h"
#include "CGameApp.h"
#include "CBoundingBox.inl"

extern CGameApp g_App;

//-----------------------------------------------------------------------------
// Name : CPlayer () (Constructor)
// Desc : CPlayer Class Constructor
//-----------------------------------------------------------------------------
CPlayer::CPlayer(const BackBuffer *pBackBuffer)
{
	//m_pSprite = new Sprite("data/planeimg.bmp", "data/planemask.bmp");
	m_pSprite = new Sprite("data/planeimgandmask.bmp", RGB(0xff,0x00, 0xff));
	m_pSprite->setBackBuffer( pBackBuffer );
	m_eSpeedState = SPEED_STOP;
	m_fTimer = 0;
	m_iLives = 0;

	// Animation frame crop rectangle
	RECT r;
	r.left = 0;
	r.top = 0;
	r.right = 128;
	r.bottom = 128;

	m_pExplosionSprite	= new AnimatedSprite("data/explosion.bmp", "data/explosionmask.bmp", r, 16);
	m_pExplosionSprite->setBackBuffer( pBackBuffer );
	m_bExplosion		= false;
	m_iExplosionFrame	= 0;
}

//-----------------------------------------------------------------------------
// Name : ~CPlayer () (Destructor)
// Desc : CPlayer Class Destructor
//-----------------------------------------------------------------------------
CPlayer::~CPlayer()
{
	delete m_pSprite;
	delete m_pExplosionSprite;
}

void CPlayer::Update(float dt)
{
	// Update sprite
	m_pSprite->update(dt);


	// Get velocity
	double v = m_pSprite->mVelocity.Magnitude();

	// NOTE: for each async sound played Windows creates a thread for you
	// but only one, so you cannot play multiple sounds at once.
	// This creation/destruction of threads also leads to bad performance
	// so this method is not recommanded to be used in complex projects.

	// update internal time counter used in sound handling (not to overlap sounds)
	m_fTimer += dt;

	// A FSM is used for sound manager 
	switch(m_eSpeedState)
	{
	case SPEED_STOP:
		if(v > 35.0f)
		{
			m_eSpeedState = SPEED_START;
			PlaySound("data/jet-start.wav", NULL, SND_FILENAME | SND_ASYNC);
			m_fTimer = 0;
		}
		break;
	case SPEED_START:
		if(v < 25.0f)
		{
			m_eSpeedState = SPEED_STOP;
			PlaySound("data/jet-stop.wav", NULL, SND_FILENAME | SND_ASYNC);
			m_fTimer = 0;
		}
		else
			if(m_fTimer > 1.f)
			{
				PlaySound("data/jet-cabin.wav", NULL, SND_FILENAME | SND_ASYNC);
				m_fTimer = 0;
			}
		break;
	}

	// NOTE: For sound you also can use MIDI but it's Win32 API it is a bit hard
	// see msdn reference: http://msdn.microsoft.com/en-us/library/ms711640.aspx
	// In this case you can use a C++ wrapper for it. See the following article:
	// http://www.codeproject.com/KB/audio-video/midiwrapper.aspx (with code also)
}

void CPlayer::Init(const BackBuffer* pBackBuffer)
{
	m_iLives = 3;
	for (int i = 0; i < m_iLives; ++i) {
		m_pLifeImages[i] = new Sprite("data/Heart.bmp", RGB(0xff, 0xff, 0xff));
		m_pLifeImages[i]->setBackBuffer(pBackBuffer);
		m_pLifeImages[i]->mPosition.x = 135 + i * (m_pLifeImages[i]->width() * 0.1f);
		m_pLifeImages[i]->mPosition.y = 135;
		m_pLifeImages[i]->setScale(0.1f, 0.1f);
	}
}

void CPlayer::Draw()
{
	for (int i = 0; i < m_iLives; i++)
		m_pLifeImages[i]->draw();
	if (!m_bExplosion)
		m_pSprite->draw();
	else
		m_pExplosionSprite->draw();
}

void CPlayer::Move(ULONG ulDirection)
{
	int width, height;
	g_App.GetWindowSize(width, height);
	if( ulDirection & CPlayer::DIR_LEFT )
		m_pSprite->mVelocity.x -= 2.1;
	double pos = m_pSprite->mPosition.x - m_pSprite->width() / 2;
	if (pos < 0)
		m_pSprite->mPosition.x += 0 - pos;

	if( ulDirection & CPlayer::DIR_RIGHT )
		m_pSprite->mVelocity.x += 2.1;
	pos = m_pSprite->mPosition.x + m_pSprite->width() / 2;
	if (pos > width)
		m_pSprite->mPosition.x += width - pos;

	if( ulDirection & CPlayer::DIR_FORWARD )
		m_pSprite->mVelocity.y -= 2.1;
	pos = m_pSprite->mPosition.y - m_pSprite->height() / 2;
	if (pos < 0)
		m_pSprite->mPosition.y += 0 - pos;

	if( ulDirection & CPlayer::DIR_BACKWARD )
		m_pSprite->mVelocity.y += 2.1;
	pos = m_pSprite->mPosition.y + m_pSprite->height() / 2;
	if (pos > height)
		m_pSprite->mPosition.y += height - pos;
}

void CPlayer::Intersects(CPlayer* player)
{
	Vec2 pPosition = player->Position();
	Vec2 pSize = player->Size();

	Vec2& position = m_pSprite->mPosition;
	Vec2& velocity = m_pSprite->mVelocity;

	CBoundingBox box(m_pSprite->mPosition, m_pSprite->mPosition + Vec2(m_pSprite->width(), m_pSprite->height()));
	CBoundingBox playerBox(pPosition, pPosition + pSize);

	Vec2 center = box.GetCenter();
	Vec2 pCenter = playerBox.GetCenter();

	if (box.Intersects(playerBox))
	{
		Vec2 c = pCenter - center;
		Vec2 norm = c.Normalize();
		position -= norm;
	}
}

bool CPlayer::Intersects(Sprite* sprite)
{
	Vec2 pPosition = sprite->mPosition;
	Vec2 pSize = Vec2(sprite->width(), sprite->height());

	Vec2& position = m_pSprite->mPosition;
	Vec2& velocity = m_pSprite->mVelocity;

	CBoundingBox box(m_pSprite->mPosition, m_pSprite->mPosition + Vec2(m_pSprite->width(), m_pSprite->height()));
	CBoundingBox chickenBox(pPosition, pPosition + pSize);

	// Calculate the vertical distance between the player and the chicken bullet
	float verticalDistance = abs(m_pSprite->mPosition.y - pPosition.y);

	// Check if the player's horizontal position is within the chicken bullet's horizontal bounds
	if (box.Intersects(chickenBox) && verticalDistance < m_pSprite->height() / 2 )
		return true;


	return false;
}
bool CPlayer::IntersectsBoss(Sprite* bossSprite)
{
	Vec2 bossPosition = bossSprite->mPosition;
	Vec2 bossSize = Vec2(bossSprite->width(), bossSprite->height());

	Vec2& playerPosition = m_pSprite->mPosition;
	Vec2& playerVelocity = m_pSprite->mVelocity;

	CBoundingBox playerBox(m_pSprite->mPosition, m_pSprite->mPosition + Vec2(m_pSprite->width(), m_pSprite->height()));
	CBoundingBox bossBox(bossPosition, bossPosition + bossSize);

	// Calculate the vertical distance between the player and the boss
	float verticalDistance = abs(m_pSprite->mPosition.y - bossPosition.y);

	// Check if the player's horizontal position is within the boss's horizontal bounds
	if (playerBox.Intersects(bossBox) && verticalDistance < m_pSprite->height() / 2)
		return true;

	return false;
}



Vec2& CPlayer::Position()
{
	return m_pSprite->mPosition;
}

Vec2& CPlayer::Velocity()
{
	return m_pSprite->mVelocity;
}

Vec2 CPlayer::Size()
{
	return Vec2(m_pSprite->width(), m_pSprite->height());
}


void CPlayer::Explode()
{
	m_iLives -= 1;
	m_pExplosionSprite->mPosition = m_pSprite->mPosition;
	m_pExplosionSprite->SetFrame(0);
	PlaySound("data/explosion.wav", NULL, SND_FILENAME | SND_ASYNC);
	m_bExplosion = true;
}

bool CPlayer::AdvanceExplosion()
{
	if(m_bExplosion)
	{
		m_pExplosionSprite->SetFrame(m_iExplosionFrame++);
		if(m_iExplosionFrame==m_pExplosionSprite->GetFrameCount())
		{
			m_bExplosion = false;
			m_iExplosionFrame = 0;
			m_pSprite->mVelocity = Vec2(0,0);
			m_eSpeedState = SPEED_STOP;
			return false;
		}
	}

	return true;
}

bool CPlayer::IsExploding()
{
	return m_bExplosion;
}

int CPlayer::GetLives()
{
	return m_iLives;
}

void CPlayer::AddLife()
{
	if (m_iLives < 3)
		m_iLives++;
}


CBullet* CPlayer::CreateBullet(BackBuffer* buffer)
{
	CBullet* bullet = new CBullet(buffer, Vec2(0.0f, -1.0f));
	bullet->m_pSprite->setScale(0.1f, 0.1f);
	float posx = m_pSprite->mPosition.x + m_pSprite->width() / 2 + (round(bullet->m_pSprite->width() * 0.1f) / 2);
	bullet->SetPosition(posx, m_pSprite->mPosition.y);
	return bullet;
}