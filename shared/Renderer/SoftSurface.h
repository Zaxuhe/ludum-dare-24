//  ***************************************************************
//  SoftSurface - Creation date: 12/07/2009
//  -------------------------------------------------------------
//  Robinson Technologies Copyright (C) 2009 - All Rights Reserved
//
//  ***************************************************************
//  Programmer(s):  Seth A. Robinson (seth@rtsoft.com)
//  ***************************************************************

#ifndef SoftSurface_h__
#define SoftSurface_h__

#include "RenderBatcher.h"

const int C_MAX_PALETTE_SIZE=256;

//Normally this isn't used, but it's useful for handling raw pixel data including palletized stuff.
//Note:  We keep palletized surfaces in the same format as bmp, ie, upside down

class SoftSurface
{

public:

	SoftSurface();
	virtual ~SoftSurface();

	enum eSurfaceType
	{
		SURFACE_NONE,
		SURFACE_PALETTE_8BIT,
		SURFACE_RGBA,
		SURFACE_RGB
	};

	enum eColorKeyType
	{
		COLOR_KEY_NONE,
		COLOR_KEY_BLACK,
		COLOR_KEY_WHITE,
		COLOR_KEY_MAGENTA
	};

	eSurfaceType GetSurfaceType() {return m_surfaceType;}
	int GetWidth(){return m_width;}
	int GetHeight() {return m_height;}
	int GetOriginalWidth() {return m_originalWidth;} //applicable to rttex images only
	int GetOriginalHeight() {return m_originalHeight;} //applicable to rttex images only
	int GetBytesPerPixel() {return m_bytesPerPixel;}

	bool IsActive() {return m_surfaceType != SURFACE_NONE;}

	bool Init(int sizex, int sizey, eSurfaceType type);
	void Kill();
	void FillColor(glColorBytes color);
	byte * GetPixelData() {return m_pPixels;}
	bool LoadFile(string fName, eColorKeyType colorKey, bool addBasePath = true);
	bool LoadFileFromMemory( byte *pMem, eColorKeyType colorKey, int inputSize = 0, bool bAddAlphaChannelIfNotPowerOfTwo =false );
	void Blit(int dstX, int dstY, SoftSurface *pSrc, int srcX = 0, int srcY = 0, int srcWidth = 0, int srcHeight = 0); //paste an image over ours
	Surface * CreateGLTexture();
	unsigned int GetSizeInBytes() {return m_memUsed;}
	void SetAutoPremultiplyAlpha(bool bYes) {m_bAutoPremultiplyAlpha = bYes;} //will convert to pre multiplied alpha ASAP, during the next copy to 32 bit surface in the case of 8 bit images
	bool GetAutoPremultiplyAlpha() {return m_bAutoPremultiplyAlpha;}
	void SetHasPremultipliedAlpha(bool bYes) {m_bHasPremultipliedAlpha = bYes;}; //current state of premultiplied alpha
	bool GetHasPremultipliedAlpha() {return m_bHasPremultipliedAlpha;}
	
	void SetPixel( int x, int y, glColorBytes color )
	{
		if (m_surfaceType == SURFACE_RGBA)
		{
			memcpy(m_pPixels+(y*m_usedPitch+x*4), &color.r, 4);
		} else assert(!"uh");
	}
	
	void SetPixel( int x, int y, byte color )
	{
		assert(m_surfaceType == SURFACE_PALETTE_8BIT);

		m_pPixels[ ( (y)*(m_usedPitch+m_pitchOffset)+x)] = color;
	}

	glColorBytes GetPixel( int x, int y )
	{
		switch (m_surfaceType)
		{
		case SURFACE_PALETTE_8BIT:
			return m_palette[m_pPixels[ ( ((m_height-1)-y)*(m_usedPitch+m_pitchOffset)+x)]];
			break;

		case SURFACE_RGBA:
			return *((glColorBytes*)((m_pPixels+(y*m_usedPitch+x*4))));
			break;

		case SURFACE_RGB:
			return *((glColorBytes*)((m_pPixels+(y*m_usedPitch+x*3))));
			break;

		default:
			assert(!"Unhandled pixel type... SSSSEEEEETTTHH!");
		}

		return glColorBytes(0,0,0,255);
	}

	bool GetUsesAlpha() {return m_bUsesAlpha;}
	void SetUsesAlpha(bool bNew) {m_bUsesAlpha = bNew;}

	int GetPitch() {return m_usedPitch+m_pitchOffset;}
	bool SetPaletteFromBMP(const string fName, eColorKeyType colorKey);
	void SetColorKeyType(eColorKeyType colorKey) {m_colorKeyType = colorKey;}
	eColorKeyType GetColorKeyType() {return m_colorKeyType;}
	glColorBytes GetColorKeyColor();
	int GetColorKeyPaletteIndex() {return m_colorKeyPaletteIndex;} //used for 8 bit palettes that have a color key
	glColorBytes * GetPalette() {return m_palette;}
	int GetPaletteColorCount() {return m_paletteColors;}
	bool IsPaletteTheSame(glColorBytes *palette, int colorCount);
	bool GetModified() {return m_bModified;}
	void SetModified(bool bNew) {m_bModified = bNew;}
	void FlipY();
	void Rotate90Degrees(bool bRotateLeft);
	void BlitFromScreen(int dstX, int dstY, int srcX /*= 0*/, int srcY /*= 0*/, int srcWidth /*= 0*/, int srcHeight /*= 0*/);
	void WriteRawDataOut(string fileName);
	void WriteBMPOut(string fileName);
	void FillAlphaBit(unsigned char alpha);
	void Scale(int newX, int newY); //does simple linear scaling

private:

	void BlitRGBAFrom8Bit( int dstX, int dstY, SoftSurface *pSrc, int srcX /*= 0*/, int srcY /*= 0*/, int srcWidth /*= 0*/, int srcHeight /*= 0*/ );
	void BlitRGBAFromRGBA( int dstX, int dstY, SoftSurface *pSrc, int srcX /*= 0*/, int srcY /*= 0*/, int srcWidth /*= 0*/, int srcHeight /*= 0*/ );
	
	glColorBytes GetFinalRGBAColorWithColorKey(glColorBytes color)
	{
		if (m_colorKeyType == COLOR_KEY_NONE) return color;
		
		if (color.Compare(GetColorKeyColor())) return glColorBytes(0,0,0,0);

		return color;
	}
	byte * GetPointerToPixel(int x, int y)
	{
		switch(m_surfaceType)
		{
		case SURFACE_PALETTE_8BIT:
			return m_pPixels + ( ((m_height-1)-y)*(m_usedPitch+m_pitchOffset)+x);

		case SURFACE_RGBA:
		case SURFACE_RGB:
			return m_pPixels + m_usedPitch*y+(x*m_bytesPerPixel);
		
		default:
			assert(!"Unknown pixel type");
		}
		return 0;
	}

	void Blit8BitFrom8Bit( int dstX, int dstY, SoftSurface *pSrc, int srcX /*= 0*/, int srcY /*= 0*/, int srcWidth /*= 0*/, int srcHeight /*= 0*/ );
	void LoadPaletteDataFromBMPMemory(byte *pPaletteData, int colors);
	void IncreaseMemCounter(int mem);
	void Blit8BitFromRGBA( int dstX, int dstY, SoftSurface *pSrc, int srcX /*= 0*/, int srcY /*= 0*/, int srcWidth /*= 0*/, int srcHeight /*= 0*/ );
	int RGBAToPalette(const glColorBytes &color);
	bool RLE8BitDecompress(byte *pDst, byte *pSrc, int dstSize, int srcSize);
	void PreMultiplyAlpha();
	bool LoadBMPTexture(byte *pMem);
	bool LoadRTTexture(byte *pMem);
	void BlitRGBFromRGBA( int dstX, int dstY, SoftSurface *pSrc, int srcX /*= 0*/, int srcY /*= 0*/, int srcWidth /*= 0*/, int srcHeight /*= 0*/ );
	void BlitRGBFromRGB( int dstX, int dstY, SoftSurface *pSrc, int srcX /*= 0*/, int srcY /*= 0*/, int srcWidth /*= 0*/, int srcHeight /*= 0*/ );
	eSurfaceType m_surfaceType;
	int m_width, m_height;
	byte *m_pPixels;
	int m_bytesPerPixel;
	int m_usedPitch; //read this many per line
	int m_pitchOffset; //filler to add to pitch for 32 bit byte boundries, bmp's need to know this, 0 if unused
	bool m_bUsesAlpha; //hint for the GL renderer
	eColorKeyType m_colorKeyType;
	glColorBytes m_palette[C_MAX_PALETTE_SIZE];
	int m_paletteColors;
	int m_colorKeyPaletteIndex;
	bool m_bModified;
	int m_memUsed;
	bool m_bAutoPremultiplyAlpha;
	bool m_bHasPremultipliedAlpha;

	//used only for RTTEX textures
	int m_originalWidth,m_originalHeight;
};

#endif // SoftSurface_h__