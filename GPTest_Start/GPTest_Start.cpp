// GPTest_Basic.cpp : ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include "GPTest_Start.h"

#include "egl/egl.h"
#include "vg/openvg.h"
#include "vgLoadImage.h"

#include <stdlib.h>
#include <time.h>

#define MAX_LOADSTRING 100

#define	NO_ENEMY		5
#define	NO_FIRE			5
#define	NO_BALL			5

#define DIR_RIGHT		0
#define DIR_LEFT		1

#define GAME_PLAYING	0
#define GAME_OVER		1
#define GAME_DONE		2


#define KEY_DOWN(code)  ((GetAsyncKeyState(code)&0x8000)?1:0)

// global variables
EGLDisplay display;
EGLSurface surface;
EGLContext context;

VGImage bgImage, fallImage[4], enemyImage, enemyImage2[2], hitImage, textImage, textImage2, lifeImage, ballImage, fireImage, victoryImage;

int gameCount, gameState = GAME_PLAYING;
int client_width, client_height;
int enemy2width;
// �ʿ��� ������ �߰��մϴ�.
// ����ü�� �ʿ��� �ʵ带 �� �߰��մϴ�.

struct PlayerState{ // ������
	int x, y;
	int dir;
	int count;
	int life;
	int width;
}player;

struct EnemyState{	// ��
	int x, y;
} enemy[NO_ENEMY];

struct FireState{ // ��
	int x, y;
	int shown;
}fire[NO_FIRE];

struct EnemyState2{	// �ź���
	int x,y;
	int shown;
	int dir;
	int count;
}enemy2[NO_ENEMY];

struct BallState{	// ��
	int x,y;
	int vx,vy;
	int shown;
} ball[NO_BALL];



// ���� ����:
HINSTANCE hInst;								// ���� �ν��Ͻ��Դϴ�.
TCHAR szTitle[MAX_LOADSTRING];					// ���� ǥ���� �ؽ�Ʈ�Դϴ�.
TCHAR szWindowClass[MAX_LOADSTRING];			// �⺻ â Ŭ���� �̸��Դϴ�.

// �� �ڵ� ��⿡ ��� �ִ� �Լ��� ������ �����Դϴ�.
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

void TimerProc( void );
void Draw(void);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: ���⿡ �ڵ带 �Է��մϴ�.
	MSG msg;
	HACCEL hAccelTable;

	// ���� ���ڿ��� �ʱ�ȭ�մϴ�.
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_GPTEST_START, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// ���� ���α׷� �ʱ�ȭ�� �����մϴ�.
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GPTEST_START));

	// �⺻ �޽��� �����Դϴ�.
	DWORD last = GetTickCount();
	while(1){
	if( PeekMessage(&msg, NULL,0, 0, 1) ){
		if(!TranslateAccelerator(msg.hwnd,NULL,&msg)){ TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	if( msg.message == WM_QUIT ) break;
	}
	if( GetTickCount()-last>32 )
	{ TimerProc(); last+=33; }
	}

	return (int) msg.wParam;
}



//
//  �Լ�: MyRegisterClass()
//
//  ����: â Ŭ������ ����մϴ�.
//
//  ����:
//
//    Windows 95���� �߰��� 'RegisterClassEx' �Լ����� ����
//    �ش� �ڵ尡 Win32 �ý��۰� ȣȯ�ǵ���
//    �Ϸ��� ��쿡�� �� �Լ��� ����մϴ�. �� �Լ��� ȣ���ؾ�
//    �ش� ���� ���α׷��� �����
//    '�ùٸ� ������' ���� �������� ������ �� �ֽ��ϴ�.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GPTEST_START));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_GPTEST_START);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   �Լ�: InitInstance(HINSTANCE, int)
//
//   ����: �ν��Ͻ� �ڵ��� �����ϰ� �� â�� ����ϴ�.
//
//   ����:
//
//        �� �Լ��� ���� �ν��Ͻ� �ڵ��� ���� ������ �����ϰ�
//        �� ���α׷� â�� ���� ���� ǥ���մϴ�.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // �ν��Ͻ� �ڵ��� ���� ������ �����մϴ�.

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 720,480, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  �Լ�: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ����: �� â�� �޽����� ó���մϴ�.
//
//  WM_COMMAND	- ���� ���α׷� �޴��� ó���մϴ�.
//  WM_PAINT	- �� â�� �׸��ϴ�.
//  WM_DESTROY	- ���� �޽����� �Խ��ϰ� ��ȯ�մϴ�.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_CREATE:
		{
			display = eglGetDisplay( GetDC(hWnd));
			eglInitialize( display, NULL, NULL );
			eglBindAPI( EGL_OPENVG_API );
			EGLint conf_list[] = { EGL_RED_SIZE, 8,
									EGL_GREEN_SIZE, 8,
									EGL_BLUE_SIZE, 8,
									EGL_ALPHA_SIZE, 8,
									EGL_NONE };
			EGLConfig config;
			EGLint num_config;
			eglChooseConfig( display, conf_list, &config, 1, &num_config );
			surface = eglCreateWindowSurface( display, config, hWnd, NULL );
			context = eglCreateContext( display, 0, NULL, NULL );

			bgImage = vgLoadImage( TEXT("images/bg1.png") );
			fallImage[0] = vgLoadImage(TEXT("images/fall1.png"));
			fallImage[1] = vgLoadImage(TEXT("images/fall2.png"));
			fallImage[2] = vgLoadImage(TEXT("images/fall3.png"));
			fallImage[3] = vgLoadImage(TEXT("images/fall4.png"));

			hitImage = vgLoadImage(TEXT("images/hit.png"));
			victoryImage = vgLoadImage(TEXT("images/victory.png"));

			enemyImage = vgLoadImage(TEXT("images/enemy.png"));
			enemyImage2[0] = vgLoadImage(TEXT("images/enemyWalk1.png"));
			enemyImage2[1] = vgLoadImage(TEXT("images/enemyWalk2.png"));

			ballImage = vgLoadImage(TEXT("images/ball.png"));
			fireImage = vgLoadImage(TEXT("images/fire.png"));

			textImage = vgLoadImage(TEXT("images/game_over.png"));
			textImage2 = vgLoadImage(TEXT("images/game_done.png"));
			lifeImage = vgLoadImage(TEXT("images/tile.png"));

			player.x=130;
			player.y=350;
			player.count=0;
			player.life=3;
			player.width = vgGetParameteri(fallImage[0], VG_IMAGE_WIDTH);
			enemy2width=vgGetParameteri(enemyImage2[0], VG_IMAGE_WIDTH);

			for(int i = 0; i < NO_BALL; i++)
			{
				ball[i].shown=0;
				ball[i].vy=0;
				ball[i].x=player.x+20;
				ball[i].y=player.y-10;
			}

			
			for(int i = 0; i < 4; i++)
			{
				enemy[i].x=i*200+30;

			}
			for(int i = 0; i < NO_ENEMY; i++)
			{
				enemy2[i].shown=0;
				enemy2[i].dir=0;
			}

			for(int i = 0; i < NO_FIRE; i++)
			{
				fire[i].shown=0;
			}
			gameCount = 0;
			
			RECT rect;
			GetClientRect(hWnd, &rect);

			client_width = rect.right;
			client_height = rect.bottom;

			srand(time(NULL));

		}
		break;
	case WM_KEYDOWN:
		if(gameState == GAME_PLAYING && wParam == VK_SPACE)
		for(int i = 0; i < NO_BALL; i++){
				if(ball[i].shown==0){
					ball[i].shown = 1;
					ball[i].x=player.x+20;
					ball[i].y=player.y-10;
					
					break;
				}
			}
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// �޴� ������ ���� �м��մϴ�.
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: ���⿡ �׸��� �ڵ带 �߰��մϴ�.
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// ���� ��ȭ ������ �޽��� ó�����Դϴ�.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void Draw( void )
{
	float clearColor[4]={1,1,1,1};
	eglMakeCurrent(display,surface,surface,context);
	// Clearing Screen
	vgSetfv( VG_CLEAR_COLOR, 4, clearColor );
	vgClear(0, 0,720,480 );
	// Setting up OpenVG to draw
	vgSeti(VG_MATRIX_MODE,VG_MATRIX_IMAGE_USER_TO_SURFACE);

	vgLoadIdentity();
	vgDrawImage(bgImage);


	vgLoadIdentity();
	if(player.dir == DIR_RIGHT)
		{
			vgTranslate(player.x, player.y);
		}
	if(player.dir == DIR_LEFT)
		{
		vgTranslate(player.x + player.width - 1, player.y);
		vgScale(-1, 1);
		}
	vgDrawImage(fallImage[player.count]);

	for(int i = 0; i < NO_BALL; i++)
	{
		if(ball[i].shown==1){
		vgLoadIdentity();
		vgTranslate(ball[i].x,ball[i].y);
		vgDrawImage(ballImage);
		}
	}

	for(int i = 0; i < NO_FIRE; i++)
	{
		if(fire[i].shown==1){
		vgLoadIdentity();
		vgTranslate(fire[i].x,fire[i].y);
		vgDrawImage(fireImage);
		}
	}

	for(int i = 0; i < 4; i++)
	{
		vgLoadIdentity();
		vgTranslate(enemy[i].x,enemy[i].y);
		vgDrawImage(enemyImage);
	}

	for(int i = 0; i < NO_ENEMY; i++)
	{
		if(enemy2[i].shown==1){

		vgLoadIdentity();
		
		if(enemy2[i].dir == DIR_RIGHT){
			vgTranslate(enemy2[i].x,enemy2[i].y);
		}
		if(enemy2[i].dir == DIR_LEFT)
		{
		vgTranslate(enemy2[i].x + enemy2width - 1, enemy2[i].y);
		vgScale(-1, 1);
		}
		vgDrawImage(enemyImage2[enemy2[i].count]);
		}
	}

	eglSwapBuffers( display, surface );
}

void TimerProc( void )
{
	gameCount++;
	if(gameCount%6==0)
	{
	player.count+=1;
	player.count=player.count%4;
	}
	if(KEY_DOWN(VK_UP)&&player.y<=480){	
		player.y += 5;
	}
	if(KEY_DOWN(VK_LEFT)&&player.x >= 0){
		player.x-=10;
		player.dir=DIR_LEFT;
	}
	if(KEY_DOWN(VK_RIGHT)){
		player.x+=10;
		player.dir=DIR_RIGHT;
	}
	player.y-=1;


	for(int i = 0; i < NO_BALL; i++)
	{
		if(ball[i].shown==1)
		{
			ball[i].vy-=1;
			ball[i].y+=ball[i].vy;
			if(ball[i].y<0)
			{
				ball[i].shown=0;
			}
		}
	}

	if(rand()%30 == 0)
	{
		for(int i = 0; i < NO_FIRE; i++)
		{
			if(fire[i].shown==0)
			{
			fire[i].x= enemy[rand()%4].x+12;
			fire[i].y=0;
			fire[i].shown=1;
			break;
			}
		}
	}

	for(int i = 0; i < NO_FIRE; i++)
	{
		if(fire[i].shown==1)
		{
			fire[i].y+=5;
			if(fire[i].y>480)
			{
				fire[i].shown=0;
			}
		}
	}

	if(rand()%30 == 0)
	{
		for(int i = 0; i < NO_ENEMY; i++)
		{
			if(enemy2[i].shown==0)
			{
				enemy2[i].shown=1;
				enemy2[i].dir=rand()%2;

				if (enemy2[i].dir == DIR_LEFT)
				{
					enemy2[i].x=710;
					enemy2[i].y=5;
				}
				if (enemy2[i].dir == DIR_RIGHT)
				{
					enemy2[i].x=10;
					enemy2[i].y=5;
				}
				break;
			}
		}

	}
		for(int i = 0; i < NO_ENEMY; i++)
			{
				if(enemy2[i].shown==1)
				{
					if (enemy2[i].dir == DIR_LEFT)
					{
						enemy2[i].x-=2;
					}
					if (enemy2[i].dir == DIR_RIGHT)
					{
						enemy2[i].x+=2;
					}

					if(enemy2[i].x>720 || enemy2[i].x<0)
					{	
						enemy2[i].shown=0;
					}
				}
			}
	
	
	if(gameCount%6==0)
	{
		for(int i = 0; i < NO_ENEMY; i++)
		{
			enemy2[i].count+=1;
			enemy2[i].count=enemy2[i].count%2;
		}
	}
	
	Draw();
}