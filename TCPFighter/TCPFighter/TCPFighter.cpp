// TCPFighter.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "TCPFighter.h"
#include "ScreenDIB.h"
#include "Sprite.h"
#include "PlayerObject.h"

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
ScreenDIB g_ScreenDib(640, 480, 32);
HWND g_hwnd;
Sprite g_sprite(64, 0x00ffffff);
BOOL g_bActiveApp;
PlayerObject *g_playerObject = nullptr;


// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
void Update();
void InitialGame();
void KeyProcess();
void Action();
void Draw();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 여기에 코드를 입력합니다.

	timeBeginPeriod(1);
	

	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TCPFIGHTER));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDI_TCPFIGHTER);
	wcex.lpszClassName = L"TCPFighter";
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	RegisterClassExW(&wcex);

	// 응용 프로그램 초기화를 수행합니다:

	hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.
	InitialGame();


	HWND hWnd = CreateWindowW(L"TCPFighter", L"TCPFighter", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, nullptr, nullptr, hInstance, nullptr);

	g_hwnd = hWnd;

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	SetFocus(hWnd);

	RECT WindowRect;
	WindowRect.top = 0;
	WindowRect.left = 0;
	WindowRect.right = 640;
	WindowRect.bottom = 480;

	AdjustWindowRectEx(&WindowRect, GetWindowStyle(hWnd), GetMenu(hWnd) != NULL, GetWindowExStyle(hWnd));

	MSG msg;

	// 기본 메시지 루프입니다:
	while (true)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}else
		{
			//TODO 게임 로직 처리 루프
			Update();
			Sleep(20);
		}
	}

	return (int)msg.wParam;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
		{
			int wmId = LOWORD(wParam);
			// 메뉴 선택을 구문 분석합니다:
			switch (wmId)
			{
			case IDM_EXIT:
				DestroyWindow(hWnd);
				break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			// TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...


			EndPaint(hWnd, &ps);
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_ACTIVATEAPP:
		g_bActiveApp = static_cast<BOOL>(wParam);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void Update(void)
{
	if(g_bActiveApp)
	{
		//TODO 키처리
		KeyProcess();
	}

	//TODO action 처리
	Action();

	//TODO 50 프레임 처리


	//TODO draw 처리
	Draw();

	g_ScreenDib.DrawBuffer(g_hwnd);

	Sleep(0);
}

void InitialGame()
{
	WCHAR filename[64][256] = { 
		L"Resource/Stand_L_01.bmp" ,
		L"Resource/Stand_L_02.bmp" ,
		L"Resource/Stand_L_03.bmp" ,
		L"Resource/Stand_R_01.bmp" ,
		L"Resource/Stand_R_02.bmp" ,
		L"Resource/Stand_R_03.bmp" ,
		L"Resource/Move_L_01.bmp" ,
		L"Resource/Move_L_02.bmp" ,
		L"Resource/Move_L_03.bmp" ,
		L"Resource/Move_L_04.bmp" ,
		L"Resource/Move_L_05.bmp" ,
		L"Resource/Move_L_06.bmp" ,
		L"Resource/Move_L_07.bmp" ,
		L"Resource/Move_L_08.bmp" ,
		L"Resource/Move_L_09.bmp" ,
		L"Resource/Move_L_10.bmp" ,
		L"Resource/Move_L_11.bmp" ,
		L"Resource/Move_L_12.bmp" ,
		L"Resource/Move_R_01.bmp" ,
		L"Resource/Move_R_02.bmp" ,
		L"Resource/Move_R_03.bmp" ,
		L"Resource/Move_R_04.bmp" ,
		L"Resource/Move_R_05.bmp" ,
		L"Resource/Move_R_06.bmp" ,
		L"Resource/Move_R_07.bmp" ,
		L"Resource/Move_R_08.bmp" ,
		L"Resource/Move_R_09.bmp" ,
		L"Resource/Move_R_10.bmp" ,
		L"Resource/Move_R_11.bmp" ,
		L"Resource/Move_R_12.bmp" ,
		L"Resource/Attack1_L_01.bmp" ,
		L"Resource/Attack1_L_02.bmp" ,
		L"Resource/Attack1_L_03.bmp" ,
		L"Resource/Attack1_L_04.bmp" ,
		L"Resource/Attack1_R_01.bmp" ,
		L"Resource/Attack1_R_02.bmp" ,
		L"Resource/Attack1_R_03.bmp" ,
		L"Resource/Attack1_R_04.bmp" ,
		L"Resource/Attack2_L_01.bmp" ,
		L"Resource/Attack2_L_02.bmp" ,
		L"Resource/Attack2_L_03.bmp" ,
		L"Resource/Attack2_L_04.bmp" ,
		L"Resource/Attack2_R_01.bmp" ,
		L"Resource/Attack2_R_02.bmp" ,
		L"Resource/Attack2_R_03.bmp" ,
		L"Resource/Attack2_R_04.bmp" ,
		L"Resource/Attack3_L_01.bmp" ,
		L"Resource/Attack3_L_02.bmp" ,
		L"Resource/Attack3_L_03.bmp" ,
		L"Resource/Attack3_L_04.bmp" ,
		L"Resource/Attack3_L_05.bmp" ,
		L"Resource/Attack3_L_06.bmp" ,
		L"Resource/Attack3_R_01.bmp" ,
		L"Resource/Attack3_R_02.bmp" ,
		L"Resource/Attack3_R_03.bmp" ,
		L"Resource/Attack3_R_04.bmp" ,
		L"Resource/Attack3_R_05.bmp" ,
		L"Resource/Attack3_R_06.bmp" ,
		L"Resource/xSpark_1.bmp" ,
		L"Resource/xSpark_2.bmp" ,
		L"Resource/xSpark_3.bmp" ,
		L"Resource/xSpark_4.bmp" ,
		L"Resource/Shadow.bmp" ,
		L"Resource/HPGuage.bmp"
	};

	for (int i = 0; i < 58; ++i) {
		g_sprite.LoadDibSprite(i, filename[i], 71, 90);
	}

	g_sprite.LoadDibSprite(58, filename[58], 70, 70);
	g_sprite.LoadDibSprite(59, filename[59], 70, 70);
	g_sprite.LoadDibSprite(60, filename[60], 70, 70);
	g_sprite.LoadDibSprite(61, filename[61], 70, 70);
	g_sprite.LoadDibSprite(62, filename[62], 32, 4);
	g_sprite.LoadDibSprite(63, filename[63], 0, 0);

	g_playerObject = new PlayerObject(TRUE);
	g_playerObject->SetSprite(ePLAYER_STAND_L01, ePLAYER_STAND_L03, 5);
	g_playerObject->SetHP(100);
	g_playerObject->SetCurPosition(200, 200);
	g_playerObject->SetDirection(TRUE);
}

void KeyProcess()
{
	if (g_playerObject == nullptr)
		return;
}

void Action()
{
	g_playerObject->Action();
}

void Draw()
{
	BYTE *bypDest = g_ScreenDib.GetDibBuffer();
	int iDestWidth = g_ScreenDib.GetWidth();
	int iDestHeight = g_ScreenDib.GetHeight();
	int iDestPitch = g_ScreenDib.GetPitch();

	memset(bypDest, 0xff, iDestHeight * iDestPitch);

	g_playerObject->Draw(bypDest, iDestWidth, iDestHeight, iDestPitch);
}
