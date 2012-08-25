//  ***************************************************************
//  SurfaceAnim - Creation date: 06/28/2009
//  -------------------------------------------------------------
//  Robinson Technologies Copyright (C) 2009 - All Rights Reserved
//
//  ***************************************************************
//  Programmer(s):  Seth A. Robinson (seth@rtsoft.com)
//  ***************************************************************

#ifndef SurfaceAnim_h__
#define SurfaceAnim_h__

#include "Surface.h"

class SurfaceAnim: public Surface
{
public:
	SurfaceAnim();
	virtual ~SurfaceAnim();

	void SetupAnim(int framesX, int framesY);
	void BlitAnim(float x, float y, int frameX = 0, int frameY = 0, unsigned int rgba = MAKE_RGBA(255,255,255,255), float rotation=0, CL_Vec2f vRotationPt = CL_Vec2f(0,0));
	void BlitScaledAnim( float x, float y,  int frameX, int frameY, CL_Vec2f vScale, eAlignment alignment = ALIGNMENT_CENTER, unsigned int rgba  = MAKE_RGBA(255,255,255,255), float rotation=0, CL_Vec2f vRotationPt = CL_Vec2f(0,0),
		bool flipX = false, bool flipY = false);

	int GetFramesX() {return m_framesX;}
	int GetFramesY() {return m_framesY;}
	float GetFrameWidth() {return m_frameWidth;}
	float GetFrameHeight() {return m_frameHeight;}
	CL_Vec2f GetFrameSize() {return CL_Vec2f(GetFrameWidth(), GetFrameHeight());}
	virtual bool LoadFileFromMemory( byte *pMem, int inputSize =0 ); //override it so we can set the frame size
	virtual bool InitBlankSurface( int x, int y); //override it so we can set the frame size
	virtual bool InitFromSoftSurface(SoftSurface *pSurf); //override it so we can set the frame size

protected:
	
	virtual void ReloadImage();

private:

	int m_framesX;
	int m_framesY;
	float m_frameWidth;
	float m_frameHeight;
	
};
#endif // SurfaceAnim_h__