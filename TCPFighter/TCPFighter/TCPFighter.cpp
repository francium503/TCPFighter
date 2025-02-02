// TCPFighter.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"


#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "ws2_32.lib")

#include "TCPFighter.h"
#include "ScreenDIB.h"
#include "Sprite.h"
#include "PlayerObject.h"
#include "EffectObject.h"
#include "StreamQ.h"
#include "PacketDefine.h"
#include "Network.h"
#include "TileMap.h"

// 전역 변수:
HINSTANCE hInst;
ScreenDIB g_ScreenDib(640, 480, 32);
HWND g_hWnd;
Sprite g_sprite(66, 0x00ffffff);
BOOL g_bActiveApp;
PlayerObject *g_playerObject = nullptr;
SOCKET g_socket;
BOOL g_connect = FALSE;
TileMap g_tileMap;
StreamQ g_recvQ(10000);
StreamQ g_sendQ(10000);
std::list<PlayerObject *> g_playerList;
std::list<EffectObject *> g_effectList;
BOOL g_bSend = FALSE;


// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:


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

	g_hWnd = hWnd;

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

	int iX = (GetSystemMetrics(SM_CXSCREEN) / 2) - (640 / 2);
	int iY = (GetSystemMetrics(SM_CYSCREEN) / 2) - (480 / 2);

	MoveWindow(hWnd, iX, iY, WindowRect.right - WindowRect.left, WindowRect.bottom - WindowRect.top, TRUE);

	MSG msg;


	// 소켓 초기화 
	 
	WSAData wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		return GetLastError();
	}

	SOCKADDR_IN serverAddr;

	g_socket = socket(AF_INET, SOCK_STREAM, 0);

	if (INVALID_SOCKET == g_socket)
		return GetLastError();

	if (WSAAsyncSelect(g_socket, g_hWnd, WM_SOCKET, FD_CONNECT | FD_READ | FD_WRITE | FD_CLOSE) == SOCKET_ERROR)
		return GetLastError();

	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	InetPton(AF_INET, dfIP, &serverAddr.sin_addr);
	serverAddr.sin_port = htons(dfPORT);

	if (SOCKET_ERROR == connect(g_socket, (SOCKADDR *)&serverAddr, sizeof(serverAddr))) {
		DWORD lastError = GetLastError();
		if (lastError != WSAEWOULDBLOCK) {
			return lastError;
		}
	}

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
			//if(g_connect == TRUE)
				Update();
		}
	}

	closesocket(g_socket);
	WSACleanup();

	timeEndPeriod(1);

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

	case WM_SOCKET:
		if (WSAGETSELECTERROR(lParam)) {
			PostQuitMessage(GetLastError());
			break;
		}

		switch(WSAGETSELECTEVENT(lParam)) {
		case FD_READ:
			if (!RecvEvent()) {
				MessageBox(g_hWnd, L"연결 오류 FD_READ FALSE", L"FD_READ", MB_OK);
				g_connect = FALSE;
			}
			break;

		case FD_WRITE:
			g_bSend = TRUE;

			SendEvent();

			break;

		case FD_CONNECT:
			g_connect = TRUE;
			break;

		case FD_CLOSE:
			g_connect = FALSE;
			break;
		}

		break;

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
	DWORD time = timeGetTime();
	static DWORD oldTime = 0;
	static DWORD lastFrameTime = 0;
	static int frameCount = 0;
	static BOOL isDraw = TRUE;
	if (oldTime == 0)
		oldTime = time;

	if (time - oldTime >= 1000)
	{
		WCHAR windowText[50];

		swprintf_s(windowText, 50, L"%d fps", frameCount);
		SetWindowTextW(g_hWnd, windowText);
		oldTime = time;
		frameCount = 0;
	}

	if (lastFrameTime == 0) {
		lastFrameTime = time;
	}
	else {
		frameCount++;

		if (time - lastFrameTime < dfTARGET_MS) {
			Sleep(dfTARGET_MS - (time - lastFrameTime));
			lastFrameTime = time + dfTARGET_MS - (time - lastFrameTime);
		}else
		{
			lastFrameTime = time;
		}
	}

	//////////// TODO 프레임 느리게 테스트 하는 방법...

	//TODO draw 처리
	if (isDraw) { // 프레임 스킵이 아닐때만
		Draw();
	}

	g_ScreenDib.DrawBuffer(g_hWnd);

	Sleep(0);
}

void InitialGame()
{
	WCHAR filename[66][256] = { 
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
		L"Resource/HPGuage.bmp",
		L"Resource/_Map.bmp",
		L"Resource/Tile_01.bmp"
	};

	for (int i = 0; i < 58; ++i) {
		g_sprite.LoadDibSprite(i, filename[i], 71, 90);
	}

	for (int i = 58; i < 62; ++i) {
		g_sprite.LoadDibSprite(i, filename[i], 70, 70);
	}

	g_sprite.LoadDibSprite(62, filename[62], 32, 4);
	g_sprite.LoadDibSprite(63, filename[63], 0, 0);
	g_sprite.LoadDibSprite(64, filename[64], 0, 0);
	g_sprite.LoadDibSprite(65, filename[65], 0, 0);

}

void KeyProcess()
{
	if (g_playerObject == nullptr)
		return;

	DWORD dwAction;

	if (GetAsyncKeyState(VK_LEFT) & 0x8001) {
		if (GetAsyncKeyState(VK_UP) & 0x8001) {
			dwAction = dfACTION_MOVE_LU;
		} else if (GetAsyncKeyState(VK_DOWN) & 0x8001) {
			dwAction = dfACTION_MOVE_LD;
		} else {
			dwAction = dfACTION_MOVE_LL;

		}
	} else if(GetAsyncKeyState(VK_RIGHT) & 0x8001) {
		if (GetAsyncKeyState(VK_UP) & 0x8001) {
			dwAction = dfACTION_MOVE_RU;
		}
		else if (GetAsyncKeyState(VK_DOWN) & 0x8001) {
			dwAction = dfACTION_MOVE_RD;
		}
		else {
			dwAction = dfACTION_MOVE_RR;
		}
	} else if(GetAsyncKeyState(VK_UP) & 0x8001) {
		dwAction = dfACTION_MOVE_UU;
	} else if(GetAsyncKeyState(VK_DOWN) & 0x8001) {
		dwAction = dfACTION_MOVE_DD;
	} else if (GetAsyncKeyState(0x5A) & 0x8001) { //0x5A = z키
		dwAction = dfACTION_ATTACK1;
	} else if (GetAsyncKeyState(0x58) & 0x8001) { // X키 
		dwAction = dfACTION_ATTACK2;
	} else if (GetAsyncKeyState(0x43) & 0x8001) { // C 키 
		dwAction = dfACTION_ATTACK3;
	} else {
		dwAction = dfACTION_STAND;
	}

	g_playerObject->ActionInput(dwAction);
}

void Action()
{
	for (auto iter = g_playerList.begin(); iter != g_playerList.end(); ++iter) {
		(*iter)->Action();
	}
	
	g_playerList.sort(PlayerObjectComp());

	for (auto iter = g_effectList.begin(); iter != g_effectList.end(); ++iter) {
		(*iter)->Action();

		if ((*iter)->IsEndFrame()) {
			delete (*iter);
			g_effectList.erase(iter);
		}
	}
}

void Draw()
{
	BYTE *bypDest = g_ScreenDib.GetDibBuffer();
	int iDestWidth = g_ScreenDib.GetWidth();
	int iDestHeight = g_ScreenDib.GetHeight();
	int iDestPitch = g_ScreenDib.GetPitch();

	memset(bypDest, 0xff, iDestHeight * iDestPitch);

	// 맵 랜더링 
	//g_sprite.DrawImage(64, 0, 0, bypDest, 640, 480, 2560);
	if (g_playerObject != nullptr) {
		g_tileMap.SetDrawX(g_playerObject->GetCurX() - 320);
		g_tileMap.SetDrawY(g_playerObject->GetCurY() - 220);
	}
	else {
		g_tileMap.SetDrawX(0);
		g_tileMap.SetDrawY(0);
	}

	g_tileMap.DrawMap(bypDest, iDestWidth, iDestHeight, iDestPitch);

	for (auto iter = g_playerList.begin(); iter != g_playerList.end(); ++iter) {
		(*iter)->Draw(bypDest, iDestWidth, iDestHeight, iDestPitch);
	}

	for (auto iter = g_effectList.begin(); iter != g_effectList.end(); ++iter) {
		(*iter)->Draw(bypDest, iDestWidth, iDestHeight, iDestPitch);
	}
}

BOOL RecvEvent()
{
	int size = g_recvQ.GetFreeSize();
	int retval;

	char* buff = new char[size];


	retval = recv(g_socket, buff, size, 0);

	if (retval == SOCKET_ERROR) {
		if (GetLastError() == WSAEWOULDBLOCK) {
			delete[] buff;
			return TRUE;
		}

		MessageBox(g_hWnd, L"recv size error", L"recv", MB_OK);
		g_connect = FALSE;
		delete[] buff;
		return FALSE;
	}

	g_recvQ.Enqueue(buff, retval);

	delete[] buff;

	while (1) {
		st_NETWORK_PACKET_HEADER header;

		retval = g_recvQ.Peek((char *)&header, sizeof(header));

		// 헤더 peek 실패
		if (retval != sizeof(header))
			break;

		// 헤더 코드 다름
		if (header.byCode != dfNETWORK_PACKET_CODE) {
			MessageBox(g_hWnd, L"head code", L"recv", MB_OK);
			return FALSE;
		}

		// 패킷 다 안들어옴
		if (g_recvQ.GetUseSize() < header.bySize + sizeof(header) + sizeof(dfNETWORK_PACKET_END))
			break;

		g_recvQ.MoveFront(sizeof(header));

		char* buff = new char[header.bySize];

		retval = g_recvQ.DeQueue(buff, header.bySize);

		// 패킷 뽑기 실패
		if (retval != header.bySize) {
			MessageBox(g_hWnd, L"dequeue pack", L"recv", MB_OK);
			return FALSE;
		}

		BYTE endCode;

		retval = g_recvQ.DeQueue((char *)&endCode, sizeof(endCode));

		// endcode 뽑기 실패
		if (retval != sizeof(endCode)) {
			MessageBox(g_hWnd, L"dequeue endcode", L"recv", MB_OK);
			return FALSE;
		}

		// endcode 다름
		if (endCode != dfNETWORK_PACKET_END) {
			MessageBox(g_hWnd, L"endcode false", L"recv", MB_OK);
			return FALSE;
		}

		PacketProc(header.byType, buff);

		delete[] buff;
	}

	return TRUE;
}

void PacketProc(BYTE byPacketType, char * Packet)
{
	switch(byPacketType) {
	case dfPACKET_SC_ATTACK1:
		PACKET_SC_ATTACK1(Packet);
		break;

	case dfPACKET_SC_ATTACK2:
		PACKET_SC_ATTACK2(Packet);
		break;

	case dfPACKET_SC_ATTACK3:
		PACKET_SC_ATTACK3(Packet);
		break;

	case dfPACKET_SC_CREATE_MY_CHARACTER:
		PACKET_SC_CREATE_MY_CHARACTER(Packet);
		break;

	case dfPACKET_SC_CREATE_OTHER_CHARACTER:
		PACKET_SC_CREATE_OTHER_CHARACTER(Packet);
		break;

	case dfPACKET_SC_DAMAGE:
		PACKET_SC_DAMAGE(Packet);
		break;

	case dfPACKET_SC_DELETE_CHARACTER:
		PACKET_SC_DELETE_CHARACTER(Packet);
		break;

	case dfPACKET_SC_MOVE_START:
		PACKET_SC_MOVE_START(Packet);
		break;

	case dfPACKET_SC_MOVE_STOP:
		PACKET_SC_MOVE_STOP(Packet);
		break;

	case dfPaCKET_SC_SYNC:
		PACKET_SC_SYNC(Packet);
		break;

	default:
		break;
	}
}

BOOL SendEvent()
{
	if (!g_connect || !g_bSend)
		return FALSE;

	while (1) {
		int size = g_sendQ.GetUseSize();

		if (size == 0)
			break;

		char *buff = new char[size];

		int retval = g_sendQ.Peek(buff, size);

		// peek 에러
		if (retval != size)
			return FALSE;

		retval = send(g_socket, buff, size, 0);

		if (retval == SOCKET_ERROR) {
			int lastError = GetLastError();

			if (lastError == WSAEWOULDBLOCK) {
				g_bSend = FALSE;
				break;
			}

			MessageBox(g_hWnd, L"send Error", L"Send", MB_OK);
			g_connect = FALSE;
			return FALSE;
		}

		g_sendQ.MoveFront(retval);

		delete[] buff;
	}

	return TRUE;
}

BOOL SendPacket(st_NETWORK_PACKET_HEADER * pHeader, char * pPacket)
{
	if (!g_connect)
		return FALSE;

	// 큐에 넣기 실패시 상대 수신 버퍼 + 내 송신 버퍼 + 큐 모두 꽉찬 상대이므로 연결 에러 처리	
	int retval = g_sendQ.Enqueue((char *)pHeader, sizeof(st_NETWORK_PACKET_HEADER));

	if (retval != sizeof(st_NETWORK_PACKET_HEADER))
		return FALSE;

	retval = g_sendQ.Enqueue(pPacket, pHeader->bySize);

	if (retval != pHeader->bySize)
		return FALSE;

	BYTE endcode = dfNETWORK_PACKET_END;
	retval = g_sendQ.Enqueue((char *)&endcode, sizeof(endcode));

	if (retval != sizeof(endcode))
		return FALSE;

	SendEvent();

	return TRUE;
}
