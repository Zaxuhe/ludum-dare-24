#include "PlatformPrecomp.h"
#include "SurfaceAnim.h"

SurfaceAnim::SurfaceAnim()
{
	m_framesX = 1;
	m_framesY = 1;
	m_frameWidth = 0;
	m_frameHeight = 0;
}

SurfaceAnim::~SurfaceAnim()
{
}
 
void SurfaceAnim::SetupAnim( int framesX, int framesY )
{
	assert(IsLoaded() && "Load your image before doing SetupAnim on it");

	m_framesX = framesX;
	m_framesY = framesY;

	assert(! (GetWidth() % m_framesX) && "image Doesn't evenly divide by frame count");
	assert(! (GetHeight() % m_framesY) && "image Doesn't evenly divide by frame count");

	m_frameWidth = (float)GetWidth()/framesX;
	m_frameHeight = (float)GetHeight()/framesY;
}

void SurfaceAnim::BlitAnim(float x, float y, int frameX, int frameY, unsigned int rgba, float rotation, CL_Vec2f vRotationPt)
{
	if (GetFrameWidth() == GetWidth() && GetFrameHeight() == GetHeight())
	{
		Blit(x,y, rgba, rotation, vRotationPt); //don't need the anim code
		return;
	}

	//first calculate the rect we need for the frame
	rtRectf src;
	src.left = m_frameWidth*frameX;
	src.top = m_frameHeight*frameY;
	src.right = src.left+m_frameWidth;
	src.bottom = src.top + m_frameHeight;

	//calculate the target
	rtRectf dst(x,y, x+m_frameWidth, y+m_frameHeight);
	BlitEx(dst, src, rgba, rotation, vRotationPt);
}

bool SurfaceAnim::LoadFileFromMemory( byte *pMem, int inputSize )
{
	if (!Surface::LoadFileFromMemory(pMem, inputSize)) return false;

	m_frameWidth = (float)GetWidth();
	m_frameHeight = (float)GetHeight();
	return true;
}

void SurfaceAnim::BlitScaledAnim( float x, float y, int frameX , int frameY, CL_Vec2f vScale, eAlignment alignment /*= ALIGNMENT_CENTER*/,
								 unsigned int rgba /*= MAKE_RGBA(255,255,255,255)*/, float rotation, CL_Vec2f vRotationPt, bool flipX, bool flipY)
{
	assert(vScale.x != 0 && vScale.y != 0 && "Dahell?");
	if (GetFrameWidth() == GetWidth() && GetFrameHeight() == GetHeight() && !flipX && !flipY) 
	{
		BlitScaledWithRotatePoint(x,y, vScale, alignment, rgba, rotation, vRotationPt); //don't need the anim code
		return;
	}

	CL_Vec2f vStart = CL_Vec2f(x,y);
	rtRectf src;
	src.left = m_frameWidth*frameX;
	src.top = m_frameHeight*frameY;
	src.right = src.left+m_frameWidth;
	src.bottom = src.top + m_frameHeight;
	
	rtRectf dst(0,0, m_frameWidth, m_frameHeight);
	
	if (flipX)
	{
		swap(src.left, src.right);
	}

	if (flipY)
	{
		swap(src.top, src.bottom);
	}
	if (alignment != ALIGNMENT_UPPER_LEFT)
	{
		vStart -= GetAlignmentOffset(CL_Vec2f(GetFrameWidth(), GetFrameHeight()), alignment);
	}

	dst.AdjustPosition(vStart.x, vStart.y);
	dst.Scale(alignment, vScale);

	BlitEx(dst, src, rgba, rotation, vRotationPt);
}

void SurfaceAnim::ReloadImage()
{
	int framesX = m_framesX;
	int framesY = m_framesY;
	float frameWidth = m_frameWidth;
	float frameHeight = m_frameHeight;
	Surface::ReloadImage();

	m_framesX = framesX;
	m_framesY = framesY;
	m_frameWidth = frameWidth;
	m_frameHeight = frameHeight;
}

bool SurfaceAnim::InitBlankSurface( int x, int y )
{
	if (!Surface::InitBlankSurface(x,y)) return false;

	m_frameWidth = (float)GetWidth();
	m_frameHeight = (float)GetHeight();
	return true;
}

bool SurfaceAnim::InitFromSoftSurface( SoftSurface *pSurf )
{
	if (!Surface::InitFromSoftSurface(pSurf)) return false;

	m_frameWidth = (float)GetWidth();
	m_frameHeight = (float)GetHeight();
	return true;
}
