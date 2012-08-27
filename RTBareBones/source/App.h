/*
 *  App.h
 *  Created by Seth Robinson on 3/6/09.
 *  For license info, check the license.txt file that should have come with this.
 *
 */
//TODO: add enemies that can atack anywere :o (every 10 enemies)
//TODO: add enemies on time
//TODO: enemies random movement
//TODO: delete zone
//TODO: add menu
//TODO: add jobs window
//TODO: change color for enemy mutated
//TODO: add enemies that lives on the trees
//TODO: fix the fact that you can add much zones on 1 tiles
//TODO: if ants are destoryed the get stronger, if enemies are destroyed they get stronger
//TODO: rain fades
#pragma once

#include "BaseApp.h"

class App: public BaseApp
{
public:
	
	App();
	virtual ~App();
	
	virtual bool Init();
	virtual void Kill();
	virtual void Draw();
	virtual void OnScreenSizeChange();
	virtual void OnEnterBackground();
	virtual void OnEnterForeground();
	virtual void Update();
	void OnExitApp(VariantList *pVarList);
	
	
	//we'll wire these to connect to some signals we care about
	void OnAccel(VariantList *pVList);
	void OnArcadeInput(VariantList *pVList);

private:

	bool m_bDidPostInit;
};


App * GetApp();
const char * GetAppName();
const char * GetBundlePrefix();
const char * GetBundleName();
