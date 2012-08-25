//  ***************************************************************
//  HTTPComponent - Creation date: 06/06/2009
//  -------------------------------------------------------------
//  Robinson Technologies Copyright (C) 2009 - All Rights Reserved
//
//  ***************************************************************
//  Programmer(s):  Seth A. Robinson (seth@rtsoft.com)
//  ***************************************************************

#ifndef HTTPComponent_h__
#define HTTPComponent_h__

#include "Component.h"
#include "Network/NetHTTP.h"

class HTTPComponent: public EntityComponent
{
public:
	HTTPComponent();
	virtual ~HTTPComponent();

	virtual void OnAdd(Entity *pEnt);
	virtual void OnRemove();

	enum eState
	{
		STATE_IDLE,
		STATE_CHECKING_CONNECTION,
		STATE_CONNECTED,
		STATE_FINISHED
	};

private:

	//our stuff
	void OnUpdate(VariantList *pVList);
	void InitAndStart(VariantList *pVList);
	void AddPostData(VariantList *pVList);
	void OnOS(VariantList *pVList);
	void PrepareConnection(VariantList *pVList);
	void SetFileOutput(VariantList *pVList);

	int m_prepareTryCount;
	NetHTTP m_netHTTP;
	eState m_state;
	string m_fileName;
};

#endif // HTTPComponent_h__