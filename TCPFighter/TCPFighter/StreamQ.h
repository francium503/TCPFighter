#pragma once


class StreamQ {
public:
	enum eBuffer {
		eBuffer_DEFAULT = 1000
	};
	StreamQ(void);
	StreamQ(int iBufferSize);

	~StreamQ();

	// 버퍼 초기화 후 사이즈 재 지정
	void ReSize(int iSize);
	// 총 버퍼 사이즈 리턴
	int GetBufferSize(void);

	//쓰는 공간
	int GetUseSize(void);
	//남은 공간
	int GetFreeSize(void);

	//원형큐의 끊어진 부분을 통과하지 않고 Get 할 수 있는 사이즈
	int GetNotBrokenGetSize(void);
	//원형큐의 끊어진 부분을 통과하지 않고 Put 할 수 있는 사이즈
	int GetNotBrokenPutSize(void);

	// 넣고 성공한 바이트 수 리턴
	int Enqueue(char *chpData, int iSize);
	// 꺼내고 성공한 바이트 수 리턴
	int DeQueue(char *chpDest, int iSize);
	// Peek 하고 성공한 바이트 수 리턴
	int Peek(char* chpDest, int iSize);

	void MoveRear(int iSize);
	int MoveFront(int iSize);

	// 멤버변수 이동으로 버퍼 초기화
	void ClearBuffer(void);

	// 직접 접근용 버퍼 포인터 리턴
	char* GetFrontBufferPtr(void);
	// 직접 접근용 버퍼 포인터 리턴
	char* GetRearBufferPtr(void);


private:

	char *m_pBuffer;
	int m_front;
	int m_rear;
	int m_bufferSize;
};