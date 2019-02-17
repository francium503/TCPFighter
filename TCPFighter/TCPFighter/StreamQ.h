#pragma once


class StreamQ {
public:
	enum eBuffer {
		eBuffer_DEFAULT = 1000
	};
	StreamQ(void);
	StreamQ(int iBufferSize);

	~StreamQ();

	// ���� �ʱ�ȭ �� ������ �� ����
	void ReSize(int iSize);
	// �� ���� ������ ����
	int GetBufferSize(void);

	//���� ����
	int GetUseSize(void);
	//���� ����
	int GetFreeSize(void);

	//����ť�� ������ �κ��� ������� �ʰ� Get �� �� �ִ� ������
	int GetNotBrokenGetSize(void);
	//����ť�� ������ �κ��� ������� �ʰ� Put �� �� �ִ� ������
	int GetNotBrokenPutSize(void);

	// �ְ� ������ ����Ʈ �� ����
	int Enqueue(char *chpData, int iSize);
	// ������ ������ ����Ʈ �� ����
	int DeQueue(char *chpDest, int iSize);
	// Peek �ϰ� ������ ����Ʈ �� ����
	int Peek(char* chpDest, int iSize);

	void MoveRear(int iSize);
	int MoveFront(int iSize);

	// ������� �̵����� ���� �ʱ�ȭ
	void ClearBuffer(void);

	// ���� ���ٿ� ���� ������ ����
	char* GetFrontBufferPtr(void);
	// ���� ���ٿ� ���� ������ ����
	char* GetRearBufferPtr(void);


private:

	char *m_pBuffer;
	int m_front;
	int m_rear;
	int m_bufferSize;
};