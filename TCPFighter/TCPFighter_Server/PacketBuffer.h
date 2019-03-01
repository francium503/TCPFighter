#pragma once

class PacketBuffer
{
public:
	enum ePACKET {
		eBuffer_DEFAULT = 1000
	};

	PacketBuffer();
	PacketBuffer(int iBuffSize);

	virtual ~PacketBuffer();

	// ��Ŷ ������ �ı�
	void Release(void);

	// ��Ŷ ���� û��
	void Clear(void);

	// ���� ������ ���
	int GetBufferSize(void)
	{
		return m_iBufferSize;
	};

	// ��Ŷ ������ ���
	int GetDataSize(void)
	{
		return m_iDataSize;
	};

	// ���� ������ ���
	char* GetBufferPtr(void)
	{
		return m_chpBuffer;
	};


	// ���� ������ ��ġ �̵��� �Լ�
	int MoveWritePos(unsigned int iPos);
	int MoveReadPos(unsigned int iPos);

	// ���� ������ �����ε�
	PacketBuffer& operator =(PacketBuffer &rhs);


	// ���� ������ �����ε�
	PacketBuffer& operator<<(const BYTE brhs);
	PacketBuffer& operator<<(const char chrhs);
	PacketBuffer& operator<<(const short shrhs);
	PacketBuffer& operator<<(const int irhs);
	PacketBuffer& operator<<(const float frhs);
	PacketBuffer& operator<<(const double drhs);
	PacketBuffer& operator<<(const __int64 i64rhs);
	PacketBuffer& operator<<(const WORD wrhs);
	PacketBuffer& operator<<(const DWORD dwrhs);


	// ���� ������ �����ε�
	PacketBuffer& operator>>(BYTE &brhs);
	PacketBuffer& operator>>(char &chrhs);
	PacketBuffer& operator>>(short &shrhs);
	PacketBuffer& operator>>(int &irhs);
	PacketBuffer& operator>>(float &frhs);
	PacketBuffer& operator>>(double &drhs);
	PacketBuffer& operator>>(__int64 &i64rhs);
	PacketBuffer& operator>>(WORD &wrhs);
	PacketBuffer& operator>>(DWORD &dwrhs);

	// ���� ����
	int GetData(char *chpDest, int iGetSize);

	// ���� �ֱ�
	int PutData(char *chpSrc, int iPutSize);


protected:
	int m_iBufferSize;
	int m_iDataSize;

	int m_iWritePos;
	int m_iReadPos;

	char* m_chpBuffer;
	BOOL m_bIsBufferLive;
};

