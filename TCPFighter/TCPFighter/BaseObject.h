#pragma once
class BaseObject
{
public:

	enum class ObjectType
	{
		Player,
		Effect,
	};


	BaseObject();
	virtual ~BaseObject();

	virtual void Action() = 0;
	virtual void Draw() = 0;
	void ActionInput();
	void NextFrame();
	BOOL isEndFrame() { return m_bEndFrame; }

	int GetCurX() { return m_iCurX; }
	int GetCurY() { return m_iCurY; }
	int GetObjectID() { return m_iObjectID; }
	ObjectType GetObjectType() { return m_eObjectType; }
	int GetOldX() { return m_iOldX; }
	int GetOldY() { return m_iOldY; }
	int GetSprite() { return m_iSpriteNow; }
	void SetCurPosition(int x, int y);
	void SetObjectID(int objectID);
	void SetObjectType(ObjectType obType);
	void SetOldPosition(int x, int y);
	void SetSprite(int spriteStart, int spriteEnd, int frameDelay);

protected:
	int m_iCurX;
	int m_iCurY;
	int m_iObjectID;
	ObjectType m_eObjectType;
	int m_iOldX;
	int m_iOldY;
	DWORD m_dwActionInput;
	BOOL m_bEndFrame;
	int m_iDelayCount;
	int m_iFrameDelay;
	int m_iSpriteEnd;
	int m_iSpriteNow;
	int m_iSpriteStart;
};

