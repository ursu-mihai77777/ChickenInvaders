//-----------------------------------------------------------------------------
// File: CGameApp.cpp
//
// Desc: Game Application class, this is the central hub for all app processing
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CGameApp Specific Includes
//-----------------------------------------------------------------------------
#include "CGameApp.h"


extern HINSTANCE g_hInst;

//-----------------------------------------------------------------------------
// CGameApp Member Functions
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Name : CGameApp () (Constructor)
// Desc : CGameApp Class Constructor
//-----------------------------------------------------------------------------
CGameApp::CGameApp()
{
	// Reset / Clear all required values
	m_hWnd			= NULL;
	m_hIcon			= NULL;
	m_hMenu			= NULL;
	m_pBBuffer		= NULL;
	m_pPlayer		= NULL;
	m_iScore		= 0;
	m_iKilledChickens = 0;
	m_iLastScore	= 0;
	m_LastFrameRate = 0;
	m_iLevel = 0;
	m_fBackgroundOffset = 0.0f;
}

//-----------------------------------------------------------------------------
// Name : ~CGameApp () (Destructor)
// Desc : CGameApp Class Destructor
//-----------------------------------------------------------------------------
CGameApp::~CGameApp()
{
	// Shut the engine down
	ShutDown();
}

//-----------------------------------------------------------------------------
// Name : InitInstance ()
// Desc : Initialises the entire Engine here.
//-----------------------------------------------------------------------------
bool CGameApp::InitInstance( LPCTSTR lpCmdLine, int iCmdShow )
{
	// Create the primary display device
	if (!CreateDisplay()) { ShutDown(); return false; }

	// Build Objects
	if (!BuildObjects()) 
	{ 
		MessageBox( 0, _T("Failed to initialize properly. Reinstalling the application may solve this problem.\nIf the problem persists, please contact technical support."), _T("Fatal Error"), MB_OK | MB_ICONSTOP);
		ShutDown(); 
		return false; 
	}

	// Set up all required game states
	SetupGameState();

	// Success!
	return true;
}

//-----------------------------------------------------------------------------
// Name : CreateDisplay ()
// Desc : Create the display windows, devices etc, ready for rendering.
//-----------------------------------------------------------------------------
bool CGameApp::CreateDisplay()
{
	LPTSTR			WindowTitle		= _T("GameFramework");
	LPCSTR			WindowClass		= _T("GameFramework_Class");
	USHORT			Width			= 800;
	USHORT			Height			= 600;
	RECT			rc;
	WNDCLASSEX		wcex;


	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= CGameApp::StaticWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= g_hInst;
	wcex.hIcon			= LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_ICON));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= WindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON));

	if(RegisterClassEx(&wcex)==0)
		return false;

	// Retrieve the final client size of the window
	::GetClientRect( m_hWnd, &rc );
	m_nViewX		= rc.left;
	m_nViewY		= rc.top;
	m_nViewWidth	= rc.right - rc.left;
	m_nViewHeight	= rc.bottom - rc.top;

	m_hWnd = CreateWindow(WindowClass, WindowTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, Width, Height, NULL, NULL, g_hInst, this);

	if (!m_hWnd)
		return false;

	// Show the window
	ShowWindow(m_hWnd, SW_SHOW);

	// Success!!
	return true;
}

//-----------------------------------------------------------------------------
// Name : BeginGame ()
// Desc : Signals the beginning of the physical post-initialisation stage.
//		From here on, the game engine has control over processing.
//-----------------------------------------------------------------------------
int CGameApp::BeginGame()
{
	MSG		msg;

	// Start main loop
	while(true) 
	{
		// Did we recieve a message, or are we idling ?
		if ( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) ) 
		{
			if (msg.message == WM_QUIT) break;
			TranslateMessage( &msg );
			DispatchMessage ( &msg );
		} 
		else 
		{
			// Advance Game Frame.
			FrameAdvance();

		} // End If messages waiting
	
	} // Until quit message is receieved

	return 0;
}

//-----------------------------------------------------------------------------
// Name : ShutDown ()
// Desc : Shuts down the game engine, and frees up all resources.
//-----------------------------------------------------------------------------
bool CGameApp::ShutDown()
{
	// Release any previously built objects
	ReleaseObjects ( );
	
	// Destroy menu, it may not be attached
	if ( m_hMenu ) DestroyMenu( m_hMenu );
	m_hMenu		 = NULL;

	// Destroy the render window
	SetMenu( m_hWnd, NULL );
	if ( m_hWnd ) DestroyWindow( m_hWnd );
	m_hWnd		  = NULL;
	
	// Shutdown Success
	return true;
}

void CGameApp::GetWindowSize(int& width, int& height)
{
	RECT rect;
	if (GetWindowRect(m_hWnd, &rect))
	{
		width = rect.right - rect.left - 16;
		height = rect.bottom - rect.top - 36;
	}
}

//-----------------------------------------------------------------------------
// Name : StaticWndProc () (Static Callback)
// Desc : This is the main messge pump for ALL display devices, it captures
//		the appropriate messages, and routes them through to the application
//		class for which it was intended, therefore giving full class access.
// Note : It is VITALLY important that you should pass your 'this' pointer to
//		the lpParam parameter of the CreateWindow function if you wish to be
//		able to pass messages back to that app object.
//-----------------------------------------------------------------------------
LRESULT CALLBACK CGameApp::StaticWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	// If this is a create message, trap the 'this' pointer passed in and store it within the window.
	if ( Message == WM_CREATE ) SetWindowLong( hWnd, GWL_USERDATA, (LONG)((CREATESTRUCT FAR *)lParam)->lpCreateParams);

	// Obtain the correct destination for this message
	CGameApp *Destination = (CGameApp*)GetWindowLong( hWnd, GWL_USERDATA );
	
	// If the hWnd has a related class, pass it through
	if (Destination) return Destination->DisplayWndProc( hWnd, Message, wParam, lParam );
	
	// No destination found, defer to system...
	return DefWindowProc( hWnd, Message, wParam, lParam );
}

//-----------------------------------------------------------------------------
// Name : DisplayWndProc ()
// Desc : The display devices internal WndProc function. All messages being
//		passed to this function are relative to the window it owns.
//-----------------------------------------------------------------------------
LRESULT CGameApp::DisplayWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	static UINT			fTimer;
	static UINT			fTimer2;

	// Determine message type
	switch (Message)
	{
	case WM_CREATE:
		break;

	case WM_CLOSE:
		PostQuitMessage(0);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_SIZE:
		if (wParam == SIZE_MINIMIZED)
		{
			// App is inactive
			m_bActive = false;

		} // App has been minimized
		else
		{
			// App is active
			m_bActive = true;

			// Store new viewport sizes
			m_nViewWidth = LOWORD(lParam);
			m_nViewHeight = HIWORD(lParam);


		} // End if !Minimized

		break;

	case WM_LBUTTONDOWN:
		// Capture the mouse
		SetCapture(m_hWnd);
		GetCursorPos(&m_OldCursorPos);
		break;

	case WM_LBUTTONUP:
		// Release the mouse
		ReleaseCapture();
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		case VK_SPACE:
		{
			if (m_pPlayer->IsExploding()) break;
			CBullet* bullet = m_pPlayer->CreateBullet(m_pBBuffer);
			m_bullets.push_back(bullet);
			break;
		}
		case VK_RETURN:
			SetTimer(m_hWnd, 4, 2500, NULL);
			break;

		}

	case WM_TIMER:
		switch (wParam)
		{
		case 1:
			if (!m_pPlayer->AdvanceExplosion())
				KillTimer(m_hWnd, 1);

		case 2:
			if (m_pChicken.size() < 0)
			{
				float pos = -100.0f;
				if (rand() % 100 < 51) pos += 100.0f;
				CChicken* m_chicken = new CChicken(m_pBBuffer, Vec2(pos, 100.0f), Vec2(FRand(0.1, 1.0), 0.0f));
				float chickenScale = 0.5f; // Adjust as needed
				m_chicken->SetScale(chickenScale, chickenScale);
				m_pChicken.push_back(m_chicken);

			}
			break;
		

	case 3:
		if (m_pHealth.empty()  && m_iLevel % 2 == 0)
		{
			CHealth* m_health = new CHealth(m_pBBuffer, Vec2(FRand(0, 860), FRand(0, 860)));
			m_pHealth.push_back(m_health);
		}
		break;

	

	

				 
	}

			break;

		case WM_COMMAND:
			break;

		default:
			return DefWindowProc(hWnd, Message, wParam, lParam);

	} // End Message Switch
	
	return 0;
}

//-----------------------------------------------------------------------------
// Name : BuildObjects ()
// Desc : Build our demonstration meshes, and the objects that instance them
//-----------------------------------------------------------------------------
bool CGameApp::BuildObjects()
{
	m_pBBuffer = new BackBuffer(m_hWnd, m_nViewWidth, m_nViewHeight);
	m_pPlayer = new CPlayer(m_pBBuffer);
	m_pPlayer->Init(m_pBBuffer);
	CChicken* m_chicken = new CChicken(m_pBBuffer, { 600, 100 }, { 1.0f, 0.0f });
	float chickenScale = 0.5f; // Adjust as needed
	m_chicken->SetScale(chickenScale, chickenScale);
	m_pChicken.push_back(m_chicken);
	

	CHealth* m_health = new CHealth(m_pBBuffer);
	m_health->SetPosition(300, 300);
	m_pHealth.push_back(m_health);


	if (!m_imgBackground.LoadBitmapFromFile("data/BackgroundBig.bmp", GetDC(m_hWnd)))
		return false;

	// Success!
	return true;
}

//-----------------------------------------------------------------------------
// Name : SetupGameState ()
// Desc : Sets up all the initial states required by the game.
//-----------------------------------------------------------------------------
void CGameApp::SetupGameState()
{
	m_pPlayer->Position() = Vec2(100, 400);
}

//-----------------------------------------------------------------------------
// Name : ReleaseObjects ()
// Desc : Releases our objects and their associated memory so that we can
//		rebuild them, if required, during our applications life-time.
//-----------------------------------------------------------------------------
void CGameApp::ReleaseObjects( )
{
	if(m_pPlayer != NULL)
	{
		delete m_pPlayer;
		m_pPlayer = NULL;
	}


	if(m_pBBuffer != NULL)
	{
		delete m_pBBuffer;
		m_pBBuffer = NULL;
	}
}

//-----------------------------------------------------------------------------
// Name : FrameAdvance () (Private)
// Desc : Called to signal that we are now rendering the next frame.
//-----------------------------------------------------------------------------
void CGameApp::FrameAdvance()
{
	static TCHAR FrameRate[ 50 ];
	static TCHAR TitleBuffer[ 255 ];

	// Advance the timer
	m_Timer.Tick(0.0f);

	// Skip if app is inactive
	if ( !m_bActive ) return;
	
	// Get / Display the framerate
	if (m_LastFrameRate != m_Timer.GetFrameRate() || m_iLastScore != m_iScore)
	{
		m_LastFrameRate = m_Timer.GetFrameRate(FrameRate, 50);
		m_iLastScore = m_iScore;
		sprintf_s(TitleBuffer, _T("Game : %s | Score : %i | Kills : %i | Level: %i"), FrameRate, m_iLastScore,m_iKilledChickens,m_iLevel);
		SetWindowText(m_hWnd, TitleBuffer);
	} // End if Frame Rate Altered

	if (m_pChicken.empty() && m_iLevel!=5) {
		for (int i = 0; i < 4; ++i) {
			float pos = 100.0f + i * 200.0f; // Adjust the spacing between chickens as needed
			CChicken* m_chicken = new CChicken(m_pBBuffer, { pos, 100.0f }, { 1.0f, 0.0f });
			float chickenScale = 0.5f; // Adjust as needed
			m_chicken->SetScale(chickenScale, chickenScale);
			m_pChicken.push_back(m_chicken);
			
		}
	}


	if (m_iLevel % 2 == 0 && m_pHealth.empty())
	{
		CHealth* m_health = new CHealth(m_pBBuffer, Vec2(FRand(0, 860), FRand(0, 860)));
		m_pHealth.push_back(m_health);
	}
	for (auto it = m_bullets.begin(); it != m_bullets.end(); )
	{
		CBullet* bullet = *it;
		bullet->Tick(0.0f);
		bool collided = false;

		if (bullet->GetType() == 0) { // Regular bullet
			// Check for collision with chickens
			for (auto jt = m_pChicken.begin(); jt != m_pChicken.end(); ++jt)
			{
				if (bullet->Intersects((*jt)->m_pSprite) )
				{
					delete* jt;
					jt = m_pChicken.erase(jt);
					m_iScore += 100;
					m_iKilledChickens++;
					collided = true;
					break;
				}
			}
			for (auto jt = m_pBigBoss.begin(); jt != m_pBigBoss.end(); ++jt)
			{
				if (bullet->Intersects((*jt)->m_pSprite))
				{
					delete* jt;
					jt = m_pBigBoss.erase(jt);
					m_iScore += 500;
					m_iKilledChickens++;
					collided = true;
					break;
				}
			}
		}
		else { // Chicken bullet
			// Check for collision with player
			if (m_pPlayer->Intersects(bullet->m_pSprite)  && !m_pPlayer->IsExploding())
			{
				SetTimer(m_hWnd, 1, 50, NULL);
				m_pPlayer->Explode();
				m_pPlayer->Position() = Vec2(100, 400);
				m_pPlayer->Velocity() = Vec2(0, 0);
				collided = true;
			}
			if (m_pPlayer->IntersectsBoss(bullet->m_pSprite) && !m_pPlayer->IsExploding())
			{
				SetTimer(m_hWnd, 1, 50, NULL);
				m_pPlayer->Explode();
				m_pPlayer->Position() = Vec2(100, 400);
				m_pPlayer->Velocity() = Vec2(0, 0);
				collided = true;
			}
		}


		if (bullet->IsOutside() || collided)
		{
			delete bullet;
			it = m_bullets.erase(it);
		}
		else
		{
			++it;
		}
	}


	for (auto it = m_pChicken.begin(); it != m_pChicken.end(); )
	{
		CChicken& chicken = **it;
		chicken.Tick(0.0f);
		if (rand() % 1000 < 0.1)
		{
			CBullet* bullet = chicken.CreateBullet(m_pBBuffer);
			m_bullets.push_back(bullet);
		}
		++it;
	}
	for (auto it = m_pBigBoss.begin(); it != m_pBigBoss.end(); )
	{
		BigBoss& bigboss = **it;
		bigboss.Tick(0.0f);
		if (rand() % 1000 < 0.1)
		{
			CBullet* bullet = bigboss.CreateBullet(m_pBBuffer);
			m_bullets.push_back(bullet);
		}
		++it;
	}
	for (auto it = m_pHealth.begin(); it != m_pHealth.end(); )
	{
		CHealth& health = **it;
		health.Tick(0.0f);
		if (m_pPlayer->Intersects(health.m_pSprite) && !m_pPlayer->IsExploding())
		{
			m_pPlayer->AddLife();
			delete* it;
			it = m_pHealth.erase(it);
		}
		else
			++it;
	}
	if (!m_pPlayer->IsExploding() && m_pPlayer->GetLives() < 1)
		ShutDown();
	if (m_iKilledChickens == 1)
		m_iLevel = 1;
	if(m_iKilledChickens > 1)
		m_iLevel = 1+ m_iKilledChickens / 4;
	if (m_pBigBoss.empty()  && m_iLevel == 5)
	{
		float pos = 00.0f;
		BigBoss* m_bigboss = new BigBoss(m_pBBuffer, { pos, 100.0f }, { 1.0f, 0.0f });
		
		m_pBigBoss.push_back(m_bigboss);


	}


	// Poll & Process input devices
	ProcessInput();

	// Animate the game objects
	AnimateObjects();

	// Drawing the game objects
	DrawObjects();
}

//-----------------------------------------------------------------------------
// Name : ProcessInput () (Private)
// Desc : Simply polls the input devices and performs basic input operations
//-----------------------------------------------------------------------------
void CGameApp::ProcessInput( )
{
	static UCHAR pKeyBuffer[ 256 ];
	ULONG		Direction = 0;
	POINT		CursorPos;
	float		X = 0.0f, Y = 0.0f;

	// Retrieve keyboard state
	if ( !GetKeyboardState( pKeyBuffer ) ) return;

	// Check the relevant keys
	if ( pKeyBuffer[ VK_UP	] & 0xF0 ) Direction |= CPlayer::DIR_FORWARD;
	if ( pKeyBuffer[ VK_DOWN  ] & 0xF0 ) Direction |= CPlayer::DIR_BACKWARD;
	if ( pKeyBuffer[ VK_LEFT  ] & 0xF0 ) Direction |= CPlayer::DIR_LEFT;
	if ( pKeyBuffer[ VK_RIGHT ] & 0xF0 ) Direction |= CPlayer::DIR_RIGHT;

	
	// Move the player
	m_pPlayer->Move(Direction);


	// Now process the mouse (if the button is pressed)
	if ( GetCapture() == m_hWnd )
	{
		// Hide the mouse pointer
		SetCursor( NULL );

		// Retrieve the cursor position
		GetCursorPos( &CursorPos );

		// Reset our cursor position so we can keep going forever :)
		SetCursorPos( m_OldCursorPos.x, m_OldCursorPos.y );

	} // End if Captured
}

//-----------------------------------------------------------------------------
// Name : AnimateObjects () (Private)
// Desc : Animates the objects we currently have loaded.
//-----------------------------------------------------------------------------
void CGameApp::AnimateObjects()
{
	m_pPlayer->Update(m_Timer.GetTimeElapsed());
}

//-----------------------------------------------------------------------------
// Name : DrawObjects () (Private)
// Desc : Draws the game objects
//-----------------------------------------------------------------------------
void CGameApp::DrawObjects()
{
	m_pBBuffer->reset();

	if (m_fBackgroundOffset <= -1154.0f) m_fBackgroundOffset = 0.2f;
	m_imgBackground.Paint(m_pBBuffer->getDC(), 0, m_fBackgroundOffset -= 0.2f);

	m_pPlayer->Draw();

	for (CBullet* bullet : m_bullets)
		bullet->Draw();

	for (CChicken* chicken : m_pChicken)
		chicken->Draw();

	for (CHealth* health : m_pHealth)
		health->Draw();

	for (BigBoss* bigboss : m_pBigBoss)
		bigboss->Draw();

	m_pBBuffer->present();
}
