#include "PlatformPrecomp.h"
#include "SoftSurface.h"
#include "FileSystem/FileManager.h"
#include "bitmap.h"
#include "BaseApp.h" //only needed for the memory statistics counter
#include "JPGSurfaceLoader.h"

SoftSurface::SoftSurface()
{
	m_pPixels = NULL;
	m_surfaceType = SURFACE_NONE;
	m_memUsed = 0;
	m_paletteColors = 0;
	m_bAutoPremultiplyAlpha = true;
	m_bHasPremultipliedAlpha = false;
	m_originalHeight = m_originalWidth = 0;
	m_height = m_width = 0;
}

SoftSurface::~SoftSurface()
{
	Kill();
}

void SoftSurface::Kill()
{
	if (m_pPixels && IsBaseAppInitted())
	{
		GetBaseApp()->ModMemUsed(-m_memUsed);
		m_memUsed = 0;

	}
	SAFE_DELETE_ARRAY(m_pPixels);
	m_surfaceType = SURFACE_NONE;
	SetUsesAlpha(false);
	SetColorKeyType(COLOR_KEY_NONE);
	m_bModified = true;
}

bool SoftSurface::Init( int sizex, int sizey, eSurfaceType type )
{
	Kill();

	m_width = sizex;
	m_height = sizey;
	m_surfaceType = type;

	if (m_originalWidth == 0)	m_originalWidth = m_width;
	if (m_originalHeight == 0) m_originalHeight = m_height;

	switch (type)
	{
	case SURFACE_PALETTE_8BIT:
		m_bytesPerPixel = 1;
		break;
	case SURFACE_RGBA:
		m_bytesPerPixel = 4;
		break;
	case SURFACE_RGB:
		m_bytesPerPixel = 3;
		break;

        default:;
}
	
	int dataSize = sizex*sizey*m_bytesPerPixel;
	m_pPixels = new byte[dataSize];
	m_usedPitch = sizex*m_bytesPerPixel;
	m_pitchOffset = 0;
	if (!m_pPixels)
	{
		assert(!"Out of mem?");
		return NULL;
	}
	IncreaseMemCounter(dataSize);
	return true;
	
}

void SoftSurface::FillColor( glColorBytes color )
{
	
	if (m_surfaceType == SURFACE_RGBA)
	{
		glColorBytes *pSurf = (glColorBytes*)m_pPixels;

		for (int n=0; n < m_width*m_height; n++)
			{
				*pSurf = color;
				pSurf++;
			}
		
	} else if (m_surfaceType == SURFACE_RGB)
	{
		byte *pSurf = m_pPixels;

		for (int n=0; n < m_width*m_height; n+=1)
		{
			memcpy(pSurf, &color, 3);
			pSurf +=3;
			
		}

	} else if (m_surfaceType == SURFACE_PALETTE_8BIT)
	{
		int indexColor = color.r;
		//TODO: I believe Android has a memset bug where this will get set to 0 on pre 2.2 devices.. rewrite for them?
		memset(m_pPixels, indexColor, GetPitch() * GetHeight());

	} else
	{
		assert(!"We don't handle this type of surface yet");
	}
}

bool SoftSurface::LoadFile( string fName, eColorKeyType colorKey, bool addBasePath)
{

	FileInstance f(fName, addBasePath);
	if (!f.IsLoaded()) return false;

	return LoadFileFromMemory(f.GetAsBytes(), colorKey);
}

bool SoftSurface::SetPaletteFromBMP(const string fName, eColorKeyType colorKey)
{
	
	m_colorKeyType = colorKey;

	assert(GetSurfaceType() == SURFACE_PALETTE_8BIT && "Load a palette requires an 8 bit surface!");

	FileInstance f(fName);
	if (!f.IsLoaded()) 
	{
		//try again
		if (!f.Load(fName, false))
		{
			return false;

		}
	}

	BMPImageHeader *pBmpImageInfo = (BMPImageHeader*)(f.GetAsBytes()+14);

	byte *pPaletteData = (byte*)pBmpImageInfo+sizeof(BMPImageHeader);

	int colors = 256;
	if (pBmpImageInfo->ColorsUsed != 0)
	{
		//looks like they don't use all the colors available
		colors = pBmpImageInfo->ColorsUsed;
	}

	LoadPaletteDataFromBMPMemory(pPaletteData, colors);
	return true; //success
}

glColorBytes SoftSurface::GetColorKeyColor()
{
	switch (GetColorKeyType())
	{
	case COLOR_KEY_WHITE:
		return glColorBytes(255,255,255,255);

	case COLOR_KEY_BLACK:
		return glColorBytes(0,0,0,255);

	case COLOR_KEY_MAGENTA:
		return glColorBytes(255,0,255,0);

        default:;
	}

	return glColorBytes(0,0,0,255);
}

bool SoftSurface::LoadBMPTexture(byte *pMem)
{

	//BMPFileHeader *pBmpHeader = (BMPFileHeader*)&pMem[0];
	BMPImageHeader *pBmpImageInfo = (BMPImageHeader*)&pMem[14];

	unsigned short offsetToImageData;

	memcpy(&offsetToImageData, &pMem[10], 2);
	byte *pPixelData = &pMem[offsetToImageData];

	m_width = pBmpImageInfo->Width;
	m_height = pBmpImageInfo->Height;
	m_bytesPerPixel = pBmpImageInfo->BitCount/8;
	int srcBytesPerPixel = m_bytesPerPixel;

	if (m_width == 0 || m_height == 0) return false;
	//note, yes, bmps are stored upside down, but so do our gl textures so we don't flip the Y

	switch (srcBytesPerPixel)
	{
	case 0: //1 bit

		m_surfaceType = SURFACE_RGBA;
		m_bytesPerPixel = 4;
		break;

	case 1:

		m_surfaceType = SURFACE_PALETTE_8BIT;
		break;

	case 2:
		m_surfaceType = SURFACE_RGBA;
		m_bytesPerPixel = 4;
		break;

	case 3:

		m_surfaceType = SURFACE_RGBA;
		m_bytesPerPixel = 4;

		break;

	case 4:
		m_surfaceType = SURFACE_RGBA;
		m_bytesPerPixel = 4;
		break;

	default:
		LogError("Don't handle %d bit bmps yet", pBmpImageInfo->BitCount);
		assert(0);
		return false;

	} 

	switch (pBmpImageInfo->Compression)
	{
	case BMP_COMPRESSION_NONE:
		break;

	case BMP_COMPRESSION_RLE8:
		break;

	default:

		LogError("Don't handle this kind of compressed bitmap yet");
		assert(0);
		return false;
	}


	m_usedPitch = m_width * m_bytesPerPixel;
	m_pitchOffset = 0;


	while ((m_usedPitch+m_pitchOffset)%4) {m_pitchOffset++;} //what's needed to pad it to a 32 byte boundry

	int dataSize = (m_usedPitch+m_pitchOffset)*m_height;

	m_pPixels = new byte[dataSize*m_bytesPerPixel];
	if (!m_pPixels) return false;
	if (m_height < 0)
	{
		LogError("BMP loader does not support top-down bitmaps yet");
		SAFE_DELETE_ARRAY(m_pPixels);
		return false;
	}

	if (srcBytesPerPixel == 0)
	{
		//convert 1 bit to 32

		int colors = 2;

		if (pBmpImageInfo->ColorsUsed != 0)
		{
			//looks like they don't use all the colors available
			colors = pBmpImageInfo->ColorsUsed;
		}

		assert(pBmpImageInfo->Compression == BMP_COMPRESSION_NONE);

		//load the palette info.  Note:  Bitmaps are BGR format
		byte *pPaletteData = (byte*)pBmpImageInfo+sizeof(BMPImageHeader);
		LoadPaletteDataFromBMPMemory(pPaletteData, colors);

		glColorBytes *pImg = (glColorBytes*)m_pPixels;

		int srcUsedPitch = m_width /8;
		int srcPitchOffset = 0;
		while ((srcUsedPitch+srcPitchOffset)%4) {srcPitchOffset++;} //what's needed to pad it to a 32 byte boundry
		int totalPitch = srcUsedPitch+srcPitchOffset;

		for (int y=0; y < m_height; y++)
		{
			for (int x=0; x < m_width; x++)
			{
				//first calculate the byte where the bit we want would be in.. fast?  No, but so what, who even uses 1-bit bmps!
				byte *pByte = &pPixelData[(m_height-y-1)*totalPitch+(x/8)];
				byte colorIndex = 0;

				if (*pByte & 1<< (x%8) ) //first figuring out which bit we want (the x%8 figures it out) from pByte, then shifting and getting it
				{
					colorIndex = 1;
				} 

				pImg[y*m_width+x] = GetFinalRGBAColorWithColorKey(m_palette[colorIndex]);
			}
		}

		if (GetSurfaceType() == SURFACE_RGBA)
		{
			if (GetAutoPremultiplyAlpha() && !GetHasPremultipliedAlpha())
			{
				PreMultiplyAlpha();
			}
		}


		//memset(pImg, 600, m_bytesPerPixel*GetWidth()*GetHeight());
	} else if (srcBytesPerPixel == 1)
	{

		if (pBmpImageInfo->Compression == BMP_COMPRESSION_RLE8)
		{
			if (!RLE8BitDecompress(m_pPixels, pPixelData, dataSize, pBmpImageInfo->Size))
			{
				LogError("Error decompressing RLE bitmap");
				SAFE_DELETE_ARRAY(m_pPixels);
				m_surfaceType = SURFACE_NONE;
				return false;
			}
		} else
		{
			memcpy(m_pPixels, pPixelData, dataSize);
		}

		int colors = 256;
		if (pBmpImageInfo->ColorsUsed != 0)
		{
			//looks like they don't use all the colors available
			colors = pBmpImageInfo->ColorsUsed;
		}

		//load the palette info.  Note:  Bitmaps are BGR format

		byte *pPaletteData = (byte*)pBmpImageInfo+sizeof(BMPImageHeader);
		LoadPaletteDataFromBMPMemory(pPaletteData, colors);

	}else if (srcBytesPerPixel == 2)
	{
		//convert 16 bit to 32

		glColorBytes *pImg = (glColorBytes*)m_pPixels;

		int srcUsedPitch = m_width * srcBytesPerPixel;
		int srcPitchOffset = 0;
		while ((srcUsedPitch+srcPitchOffset)%2) {srcPitchOffset++;} //what's needed to pad it to a 16 byte boundry
		int totalPitch = srcUsedPitch+srcPitchOffset;

		for (int y=0; y < m_height; y++)
		{
			for (int x=0; x < m_width; x++)
			{
				byte *pByte = &pPixelData[(m_height-y-1)*totalPitch+x*2];

				uint16 color = *(uint16*)pByte;
				uint8 red = ( color >> 10 ) & 31;
				uint8 green = ( color >> 5 ) & 31;
				uint8 blue = ( color ) & 31;

				red <<= 3;
				green <<= 3;
				blue <<= 3;

				pImg[y*m_width+x] = GetFinalRGBAColorWithColorKey(glColorBytes(red, green, blue, 255));
			}
		}

		if (GetAutoPremultiplyAlpha() && !GetHasPremultipliedAlpha())
		{
			PreMultiplyAlpha();
		}

		//memset(pImg, 600, m_bytesPerPixel*GetWidth()*GetHeight());
	} else if (srcBytesPerPixel == 3)
	{
		//convert 24 bit bmp to 32 bit rgba

		glColorBytes *pImg = (glColorBytes*)m_pPixels;

		int srcUsedPitch = m_width * srcBytesPerPixel;
		int srcPitchOffset = 0;
		while ((srcUsedPitch+srcPitchOffset)%4) {srcPitchOffset++;} //what's needed to pad it to a 32 byte boundry
		int totalPitch = srcUsedPitch+srcPitchOffset;

		for (int y=0; y < m_height; y++)
		{
			for (int x=0; x < m_width; x++)
			{
				byte *pSrc = &pPixelData[ ((m_height-y-1)*totalPitch)+x*3];
				pImg[y*m_width+x] = GetFinalRGBAColorWithColorKey(glColorBytes(pSrc[2], pSrc[1], pSrc[0], 255));
			}
		}

		if (GetAutoPremultiplyAlpha() && !GetHasPremultipliedAlpha())
		{
			PreMultiplyAlpha();
		}


	} else if (srcBytesPerPixel == 4)
	{
		//convert 32 bit bmp to 32 bit rgba

		glColorBytes *pImg = (glColorBytes*)m_pPixels;

		int srcUsedPitch = m_width * srcBytesPerPixel;
		int srcPitchOffset = 0;
		while ((srcUsedPitch+srcPitchOffset)%4) {srcPitchOffset++;} //what's needed to pad it to a 32 byte boundry
		int totalPitch = srcUsedPitch+srcPitchOffset;

		for (int y=0; y < m_height; y++)
		{
			for (int x=0; x < m_width; x++)
			{
				byte *pSrc = &pPixelData[(m_height-y-1)*totalPitch+x*4];
				pImg[y*m_width+x] = GetFinalRGBAColorWithColorKey(glColorBytes(pSrc[2], pSrc[1], pSrc[0], pSrc[3]));
			}
		}

		if (GetAutoPremultiplyAlpha() && !GetHasPremultipliedAlpha())
		{
			PreMultiplyAlpha();
		}

		//memset(pImg, 600, m_bytesPerPixel*GetWidth()*GetHeight());
	}
	else
	{
		assert(!"Unhandled");
	}

	if (m_bytesPerPixel == 4 && GetAutoPremultiplyAlpha() && !GetHasPremultipliedAlpha())
	{
		SetHasPremultipliedAlpha(true);
	}

	if (GetColorKeyType() != COLOR_KEY_NONE)
	{
		m_bUsesAlpha = true;
	}

	assert(GetWidth() && GetHeight());

	IncreaseMemCounter(dataSize);
	return true;
}

#define GL_RGBA8 0x8058
bool SoftSurface::LoadRTTexture(byte *pMem)
{
	rttex_header *pTexHeader = (rttex_header*)pMem;
	rttex_mip_header *pMipSection;

	m_width = pTexHeader->width;
	m_height = pTexHeader->height;

	int bUsesAlpha = pTexHeader->bUsesAlpha != 0;
	int texType = pTexHeader->format;
	m_originalWidth = pTexHeader->originalWidth;
	m_originalHeight = pTexHeader->originalHeight;
	byte *pCurPos = pMem + sizeof(rttex_header);

#define GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG			0x8C00 //35840
#define GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG			0x8C03 //35843


	pMipSection = (rttex_mip_header*)pCurPos;
	pCurPos += sizeof(rttex_mip_header);
	byte *pTextureData =  (byte*)pCurPos ;

	//only load the first image, not the mipmaps, if we have any
	if (pTexHeader->format < GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG || pTexHeader->format > GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG) //doesn't look like a compressed texture
	{
		int colorType = GL_RGBA;
		if (!bUsesAlpha)
		{
			colorType = GL_RGB;
		}
		//			LogMsg("Loading surface: miplevel %d, internal color type:0x%02lX  colortype 0x%02lX, x%d y%d, format type: 0x%02lX", nMipLevel, colorTypeSource, colorType, pMipSection->width, pMipSection->height, pTexHeader->format );

		assert( (colorType == GL_RGBA || colorType == GL_RGB) && "We don't support anything other than RGB or RGBA at 8 bits per channel in SoftSurface rttex loading");

		m_bUsesAlpha = bUsesAlpha != 0;

		if (texType == RT_FORMAT_EMBEDDED_FILE)
		{
			if (!LoadFileFromMemory(pTextureData, SoftSurface::COLOR_KEY_NONE, pMipSection->dataSize))
			{
				LogMsg("(Failed to load jpg in SoftSurface)");
				assert(!"failed to load jpg in SoftSurface");
				return false;
			}

		} else
		{

			eSurfaceType surfType = SURFACE_RGBA;
			
			if (colorType == GL_RGB)
			{
				surfType = SURFACE_RGB;
			}
			
			Init(m_width, m_height, surfType);

			//copy image data
			assert(pMipSection->dataSize == m_width*m_height*GetBytesPerPixel() && "We only support RGB and RGBA when loading rttex from SoftSurface");
			memcpy(m_pPixels, pTextureData, pMipSection->dataSize);
		}
	} else
	{
		assert(!"SoftSurface can't decode PVR images!");
	}

	if (GetSurfaceType() == SURFACE_RGBA)
	{
		if (GetAutoPremultiplyAlpha() && !GetHasPremultipliedAlpha())
		{
			PreMultiplyAlpha();
			SetHasPremultipliedAlpha(true);
		}
	}

	return true;
}

bool SoftSurface::LoadFileFromMemory( byte *pMem, eColorKeyType colorKey, int inputSize, bool bAddAlphaChannelIfPadded)
{
	
	Kill();
	SetColorKeyType(colorKey);

	if (*((uint16*)pMem) == C_JPG_HEADER_MARKER)
	{
		//it's a jpg file.  Do we have the proper encoder?
		#if defined(RT_JPG_SUPPORT)

		JPGSurfaceLoader loader;
		if (!loader.LoadFromMem(pMem, inputSize, this, bAddAlphaChannelIfPadded))
		{
			LogError("Error loading jpg from memory chunk");
			return false;
		} else
		{
			//LogMsg("Loaded jpg!");
			return true;
		}
		#else
		LogError("Can't open .jpg files unless RT_JPG_SUPPORT is defined and");
		LogError("we're compiled with the LibJPeg files included. Check RTSimpleApp for an example of how to do this.");
		assert(!"Not compiled with jpg support! Need it to load this image...!") ;
		return false;
		#endif

	} else if (strncmp((char*)pMem, "BM", 2) == 0)
	{
		//we've got a bitmap on our hands it looks like
		return LoadBMPTexture(pMem);
	} else if (strncmp((char*)pMem, C_RTFILE_TEXTURE_HEADER, 6) == 0)
	{
		return LoadRTTexture(pMem);
	} else
	{
		LogError("Surface: Unknown file type");
		return false; 
	}

}

void SoftSurface::IncreaseMemCounter(int mem)
{
	assert(!m_memUsed);
	m_memUsed = mem;
	GetBaseApp()->ModMemUsed(m_memUsed);
}


//based on source by Codehead 08/11/04 taken from http://gpwiki.org/index.php/LoadBMPCpp

bool SoftSurface::RLE8BitDecompress(byte *pDst, byte *pSrc, int dstSize, int imageInfoSize)
{
	unsigned char bOpCode,bVal;
	int iCount,iPos,iIndex;
	
	iPos=0;
	iIndex=0;

	int x = 0;
	int y = 0;

	while(1)
	{
		// Stay on even bytes
		while(iPos%2)
		{
			iPos++;
		}

		bOpCode=pSrc[iPos];
		bVal=pSrc[iPos+1];
		iPos+=2;

		if(bOpCode>0) // Run mode, Repeat 'bVal' 'OpCode' times
		{
			for(iCount=0;iCount!=bOpCode;iCount++)
			{
				SetPixel(x,y, bVal);
				//pDst[iIndex]=bVal;
				x++;
			}
		}
		else // Absolute Mode (Opcode=0), various options
		{
			switch(bVal)
			{
			case 0:  // EOL, no action
				x = 0;
				y++;
				break;

			case 1:  // EOF, STOP!
				return true;
				break;

			case 2:  // Reposition, Never used
				
				assert(!"Uh.. you sure about that?");
				//oh well, handle it anyway

				x += pSrc[iPos++];
				y += pSrc[iPos++];
				
				break;

			default: // Copy the next 'bVal' bytes directly to the image
				for(iCount=bVal;iCount!=0;iCount--)
				{
					SetPixel(x,y, pSrc[iPos]);
					++iPos;
					x++;
				}
				break;
			}
		}
	}
	
	return false; //error if we got here
}



void SoftSurface::PreMultiplyAlpha()
{


	assert(GetSurfaceType() == SURFACE_RGBA && "This doesn't make sense premuliplying something that has no alpha!");

	glColorBytes *pDestImage = (glColorBytes*)GetPointerToPixel(0, 0);

	//slower way that supports transparency
	for (int y=0; y < GetHeight(); y++)
	{
		for (int x=0; x < GetWidth(); x++)
		{

			pDestImage->r = (pDestImage->r*pDestImage->a)/255;
			pDestImage->g = (pDestImage->g*pDestImage->a)/255;
			pDestImage->b = (pDestImage->b*pDestImage->a)/255;
			pDestImage++;
		}
	}

}

void SoftSurface::LoadPaletteDataFromBMPMemory(byte *pPaletteData, int colors)
{
	glColorBytes colorKeyColor = GetColorKeyColor();
	SetUsesAlpha(false);
	m_paletteColors = colors;
	m_colorKeyPaletteIndex = -1;
	for (int i=0; i < colors; i++)
	{
		m_palette[i] = glColorBytes(pPaletteData[2],pPaletteData[1],pPaletteData[0], 255);

		if (GetColorKeyType() != COLOR_KEY_NONE)
		{
			if (m_palette[i].r == colorKeyColor.r && m_palette[i].g == colorKeyColor.g && m_palette[i].b == colorKeyColor.b)
			{
				m_palette[i] = glColorBytes(0,0,0,0); //transparent and black
				if (m_colorKeyPaletteIndex == -1) m_colorKeyPaletteIndex = i; //set the first instance of this color found as the "color key index"
			}	
		}

		pPaletteData += 4; //skip three bytes to the next set of colors.. oh, and 1 byte filler (?)
	}

	if (m_colorKeyPaletteIndex != -1)
	{
		SetUsesAlpha(true);
	}

}

void SoftSurface::BlitRGBAFrom8Bit( int dstX, int dstY, SoftSurface *pSrc, int srcX /*= 0*/, int srcY /*= 0*/, int srcWidth /*= 0*/, int srcHeight /*= 0*/ )
{
	byte *pDestImage = GetPointerToPixel(dstX, dstY);
	byte *pSrcImage = pSrc->GetPointerToPixel(srcX, srcY);

	int colorKey = pSrc->GetColorKeyPaletteIndex();

	glColorBytes transparent(0,0,0,0);

	bool bPerformPreMultAlpha = false;

	if (pSrc->GetAutoPremultiplyAlpha() && !pSrc->GetHasPremultipliedAlpha())
	{
		bPerformPreMultAlpha = true;
	}

	if ((pSrc->GetAutoPremultiplyAlpha() || pSrc->GetHasPremultipliedAlpha()))
	{
		m_bHasPremultipliedAlpha = true;
	}

	//if (!GetUsesAlpha())
	{
		for (int y=0; y < srcHeight; y++)
		{
			for (int x=0; x < srcWidth; x++)
			{
				if (colorKey != pSrcImage[x])
				{
					
					//don't actually have to do this, because 8 bit things don't have translucency
					/*
					if (bPerformPreMultAlpha)
					{
						c.r = (c.r*c.a)/255;
						c.g = (c.g*c.a)/255;
						c.b = (c.b*c.a)/255;
					}
					*/

					memcpy(pDestImage+x*m_bytesPerPixel, &pSrc->GetPalette()[pSrcImage[x]], 4);
				} else
				{
					//make it a transparent pixel
					//memcpy(pDestImage+x*m_bytesPerPixel, &transparent, 4);
				}
			}
			
			pDestImage += GetPitch();
			pSrcImage -= pSrc->GetPitch();
		} 
	}
	/*
	else
	{
			assert(!"You don't handle transparency here yet");

			
			//a slow way that works fine:

			glColorBytes p;
			for (int y=0; y < srcHeight; y++)
			{
			for (int x=0; x < srcWidth; x++)
			{
			p = pSrc->GetPixel(srcX+x, srcY+y);

			if (p.a > 0)
			{
			SetPixel(dstX+x,dstY+y, p);
			}
			}
			}
			
	} 
*/
}



void SoftSurface::BlitRGBAFromRGBA( int dstX, int dstY, SoftSurface *pSrc, int srcX /*= 0*/, int srcY /*= 0*/, int srcWidth /*= 0*/, int srcHeight /*= 0*/ )
{

	byte *pDestImage = GetPointerToPixel(dstX, dstY);
	byte *pSrcImage = pSrc->GetPointerToPixel(srcX, srcY);

	if ((pSrc->GetAutoPremultiplyAlpha() || pSrc->GetHasPremultipliedAlpha()))
	{
		m_bHasPremultipliedAlpha = true;
	}

	if (!pSrc->GetUsesAlpha())
	{
		for (int y=0; y < srcHeight; y++)
		{
			memcpy(pDestImage, pSrcImage, srcWidth*m_bytesPerPixel);
			pDestImage += GetPitch();
			pSrcImage += pSrc->GetPitch();
		}
	} else
	{
		//slower way that supports transparency
		for (int y=0; y < srcHeight; y++)
		{
			for (int x=0; x < srcWidth; x++)
			{
				if (pSrcImage[3] == 0)
				{
					//blank!
				} else
				{
					memcpy(pDestImage, pSrcImage, m_bytesPerPixel);
				}
				
				pDestImage +=m_bytesPerPixel;
				pSrcImage += m_bytesPerPixel;
			}
			pDestImage += GetPitch()-(srcWidth*m_bytesPerPixel);
			pSrcImage += pSrc->GetPitch()-(srcWidth*m_bytesPerPixel);
		}
	}
}


void SoftSurface::BlitRGBFromRGBA( int dstX, int dstY, SoftSurface *pSrc, int srcX /*= 0*/, int srcY /*= 0*/, int srcWidth /*= 0*/, int srcHeight /*= 0*/ )
{

	byte *pDestImage = GetPointerToPixel(dstX, dstY);
	byte *pSrcImage = pSrc->GetPointerToPixel(srcX, srcY);

	int bytesPerPixelSource = pSrc->GetBytesPerPixel();

	for (int y=0; y < srcHeight; y++)
	{
		for (int x=0; x < srcWidth; x++)
		{
			//TODO: Add merges that take alpha into account?
			/*
			if (pSrcImage[3] == 0)
			{
				//blank!
			} else*/
			{
			//	glColorBytes converted(pSrcImage[0], pSrcImage[1], pSrcImage[2], pSrcImage[3]);
				memcpy(pDestImage, pSrcImage, m_bytesPerPixel);
			}

			pDestImage +=m_bytesPerPixel;
			pSrcImage += bytesPerPixelSource;
		}
		pDestImage += GetPitch()-(srcWidth*m_bytesPerPixel);
		pSrcImage += pSrc->GetPitch()-(srcWidth*bytesPerPixelSource);
	}

}


void SoftSurface::BlitRGBFromRGB( int dstX, int dstY, SoftSurface *pSrc, int srcX /*= 0*/, int srcY /*= 0*/, int srcWidth /*= 0*/, int srcHeight /*= 0*/ )
{

	byte *pDestImage = GetPointerToPixel(dstX, dstY);
	byte *pSrcImage = pSrc->GetPointerToPixel(srcX, srcY);

	for (int y=0; y < srcHeight; y++)
	{
		memcpy(pDestImage, pSrcImage, srcWidth*m_bytesPerPixel);
		pDestImage += GetPitch();
		pSrcImage += pSrc->GetPitch();
	}
}

int SoftSurface::RGBAToPalette(const glColorBytes &color)
{
	assert(GetSurfaceType() == SURFACE_PALETTE_8BIT);

	for (int i=0; i < m_paletteColors; i++)
	{
		if (m_palette[i].Compare(color))
		{
			return i;
		}
	}

	return 0; //unknown color and we don't care enough to find the closest one

}

void SoftSurface::Blit8BitFromRGBA( int dstX, int dstY, SoftSurface *pSrc, int srcX /*= 0*/, int srcY /*= 0*/, int srcWidth /*= 0*/, int srcHeight /*= 0*/ )
{

	byte *pDestImage = GetPointerToPixel(dstX, dstY);
	glColorBytes *pSrcImage = (glColorBytes*)pSrc->GetPointerToPixel(srcX, srcY);

		for (int y=0; y < srcHeight; y++)
		{
			for (int x=0; x < srcWidth; x++)
			{
				pDestImage[x] = RGBAToPalette(pSrcImage[x]);
			}

			pDestImage -= GetPitch();
			pSrcImage += srcWidth;
		} 
}


void SoftSurface::Blit8BitFrom8Bit( int dstX, int dstY, SoftSurface *pSrc, int srcX /*= 0*/, int srcY /*= 0*/, int srcWidth /*= 0*/, int srcHeight /*= 0*/ )
{

	byte *pDestImage = GetPointerToPixel(dstX, dstY);
	byte *pSrcImage = pSrc->GetPointerToPixel(srcX, srcY);

	if ((pSrc->GetAutoPremultiplyAlpha()))
	{
		SetAutoPremultiplyAlpha(true);
	}

	if (pSrc->GetUsesAlpha())
	{
		int colorKeyPaletteIndex = pSrc->GetColorKeyPaletteIndex();

		for (int y=0; y < srcHeight; y++)
		{
			for (int x=0; x < srcWidth; x++)
			{
				if (pSrcImage[x] != colorKeyPaletteIndex)
				{
					pDestImage[x] = pSrcImage[x];
				}
			}

			pDestImage -= GetPitch();
			pSrcImage -= pSrc->GetPitch();
		}

	} else
	{
	for (int y=0; y < srcHeight; y++)
		{
			memcpy(pDestImage, pSrcImage, srcWidth);
			pDestImage -= GetPitch();
			pSrcImage -= pSrc->GetPitch();
		}
	}
}


void SoftSurface::FlipY()
{
	byte *pData = GetPixelData();
	byte *pDataReverse;
	byte *pTmp = new byte[GetPitch()];
	
	const int pitch = GetPitch();

	for (int y=0; y < GetHeight()/2; y++)
	{
		pDataReverse = GetPointerToPixel(0, (GetHeight()-1)-y);
		memcpy(pTmp, pData, pitch);
		memcpy(pData, pDataReverse, pitch);
		memcpy(pDataReverse, pTmp, pitch);
		pData += pitch;
	}
	delete [] pTmp;
}

void SoftSurface::BlitFromScreen(int dstX, int dstY, int srcX /*= 0*/, int srcY /*= 0*/, int srcWidth /*= 0*/, int srcHeight /*= 0*/)
{
	//LogMsg("Blitting from screen");
	
	SoftSurface temp;

	if (InLandscapeGUIMode())
	{
		swap(srcWidth, srcHeight);
	}

	if (!temp.Init(srcWidth, srcHeight, SURFACE_RGBA))
	{
		assert(!"Low mem");
		return;
	}
	
	glReadPixels(srcX, srcY, srcWidth, srcHeight, GL_RGBA, GL_UNSIGNED_BYTE, temp.GetPixelData());
	temp.SetUsesAlpha(false);
	CHECK_GL_ERROR();
	//temp.FillColor(glColorBytes(255,0,0,255));
	
	Blit(dstX, GetHeight()-srcHeight, &temp);
	//FlipY();
	SetUsesAlpha(false);
}

void SoftSurface::Blit( int dstX, int dstY, SoftSurface *pSrc, int srcX /*= 0*/, int srcY /*= 0*/, int srcWidth /*= 0*/, int srcHeight /*= 0*/ )
{
	if (srcWidth == 0) srcWidth = pSrc->GetWidth();
	if (srcHeight == 0) srcHeight  = pSrc->GetHeight();

	if (dstY < 0)
	{
		//it's out of bounds above the screen.. move it
		srcHeight += dstY;
		srcY -= dstY;
		dstY = 0;
	}

	if (dstX < 0)
	{
		//it's out of bounds to the left of the screen, move it
		srcWidth += dstX;
		srcX -= dstX;
		dstX = 0;
	}

	//truncate to our size
	srcWidth = rt_min(GetWidth()-dstX, srcWidth);
	srcHeight = rt_min(GetHeight()-dstY, srcHeight);

	SetModified(true);
	
	if (GetSurfaceType() == SURFACE_RGBA)
	{
		if (pSrc->GetSurfaceType() == SURFACE_PALETTE_8BIT)
		{
			BlitRGBAFrom8Bit(dstX, dstY, pSrc, srcX, srcY, srcWidth, srcHeight);
			return;
		}
		
		if (pSrc->GetSurfaceType() == SURFACE_RGBA)
		{
			BlitRGBAFromRGBA(dstX, dstY, pSrc, srcX, srcY, srcWidth, srcHeight);
			return;
		}
	} else if (GetSurfaceType() == SURFACE_PALETTE_8BIT)
	{
		if (pSrc->GetSurfaceType() == SURFACE_PALETTE_8BIT)
		{
			Blit8BitFrom8Bit(dstX, dstY, pSrc, srcX, srcY, srcWidth, srcHeight);
			return;
		}

		if (pSrc->GetSurfaceType() == SURFACE_RGBA)
		{
			Blit8BitFromRGBA(dstX, dstY, pSrc, srcX, srcY, srcWidth, srcHeight);
			return;
		}
	} else if (GetSurfaceType() == SURFACE_RGB)
	{
		
		if (pSrc->GetSurfaceType() == SURFACE_RGBA)
		{
			BlitRGBFromRGBA(dstX, dstY, pSrc, srcX, srcY, srcWidth, srcHeight);
			return;
		} else
			if (pSrc->GetSurfaceType() == SURFACE_RGB)
			{
				BlitRGBFromRGB(dstX, dstY, pSrc, srcX, srcY, srcWidth, srcHeight);
				return;
			}
	}

	assert(!"We don't handle this combination of surface types yet... SETTTTHHHHH!");

}

Surface * SoftSurface::CreateGLTexture()
{
	Surface *pSurf = new Surface;
	if (!pSurf) return pSurf;
	pSurf->SetTextureType(Surface::TYPE_GUI);
	pSurf->InitBlankSurface(GetWidth(),GetHeight());
	//pSurf->FillColor(glColorBytes(0,0,0,0));

	if (GetSurfaceType() == SURFACE_PALETTE_8BIT)
	{
		SoftSurface s;
		//do slow conversion first
		s.Init(GetWidth(),GetHeight(), SoftSurface::SURFACE_RGBA);
		s.Blit(0,0, this);
		s.FlipY();
		//put it on the GL surface
		pSurf->UpdateSurfaceRect(rtRect(0,0, s.GetWidth(), s.GetHeight()), s.GetPixelData());
	
		if (s.GetHasPremultipliedAlpha()) 
		{
			pSurf->SetBlendingMode(Surface::BLENDING_PREMULTIPLIED_ALPHA);
		}

	} else
	{
		//put it on the GL surface
		//FlipY();
		pSurf->UpdateSurfaceRect(rtRect(0,0, GetWidth(), GetHeight()), GetPixelData());
		//FlipY();
		if (GetHasPremultipliedAlpha()) 
		{
			pSurf->SetBlendingMode(Surface::BLENDING_PREMULTIPLIED_ALPHA);
		}
	}
	
	pSurf->SetUsesAlpha(GetUsesAlpha());
	return pSurf;
}

bool SoftSurface::IsPaletteTheSame( glColorBytes *palette, int colorCount)
{
	if (m_paletteColors != colorCount) return false;

	for (int i=0; i < m_paletteColors; i++)
	{
		if ( !m_palette[i].Compare(palette[i]) ) return false;
	}
	return true;
}

void SoftSurface::WriteRawDataOut(string fileName)
{
	FILE *fp = fopen(fileName.c_str(), "wb");
	if (!fp)
	{
		LogError("Can't open file %s for writing", fileName.c_str());
		return;
	}

	fwrite(GetPixelData(), m_width*m_height*m_bytesPerPixel, 1, fp);
	fclose(fp);
}

void SoftSurface::Rotate90Degrees( bool bRotateLeft )
{
	
	assert(m_pPixels && "Uh... no image is loaded");
	assert(!m_pitchOffset && "The pitch has dead pixel space in it (loaded a .bmp?).. this func won't work right..");

	//create new buffer the same size (memory-wise) as our current one
	int dataSize = m_width*m_height*m_bytesPerPixel;
	byte *pTemp = new byte[dataSize];

	int targetWidth = m_height;
	int targetHeight = m_width; //flip those around
	int targetPitch = targetWidth*m_bytesPerPixel;

	//step through our current data pixel by pixel, from the top left, reading horizontally
	for (int y=0; y < m_height; y++)
	{
		for (int x=0; x < m_width; x++)
		{
			if (bRotateLeft)
			{
				memcpy(&pTemp[ (( (targetHeight-1)-x) *targetPitch) +y*m_bytesPerPixel ],  &m_pPixels[ (y*m_usedPitch ) + x*m_bytesPerPixel], m_bytesPerPixel);
			} else
			{
				memcpy(&pTemp[ (x*targetPitch) +( (targetWidth-1)-y) *m_bytesPerPixel ],  &m_pPixels[ (y*m_usedPitch ) + x*m_bytesPerPixel], m_bytesPerPixel);
			}

			//test
			//unsigned int color = MAKE_RGBA(255,0,0,255);
			//memcpy(&pTemp[ (( (targetHeight-1)-x) *targetPitch) +y*m_bytesPerPixel ], &color, m_bytesPerPixel);
		}
	}
	
	//switch to using the new surface we just made

	SAFE_DELETE_ARRAY(m_pPixels);
	m_pPixels = pTemp;
	swap(m_width, m_height);
	swap(m_originalHeight, m_originalWidth);
	m_usedPitch = targetPitch;
}

void SoftSurface::WriteBMPOut( string fileName )
{
	if (GetSurfaceType() != SURFACE_RGB && GetSurfaceType() != SURFACE_RGBA)
	{
		LogError("Can only save bmps for RGB and RGBA formats, not 8 bit stuff.");
		return;
	}

	//assert(sizeof(BMPFileHeader) == 14); //fails.. but instead of figuring out the packing commands for each os, I'll work around it
	
	//write it
	FILE *fp = fopen(fileName.c_str(), "wb");
	if (!fp)
	{
		LogError("Unable to open %s for writing", fileName.c_str());
		return;
	}

	const char *fileHeader="BM";
	fwrite(fileHeader, 2, 1, fp);
	unsigned int temp = 0;
	fwrite(&temp, 4, 1, fp);
	fwrite(&temp, 4, 1, fp);
	temp = 14+sizeof(BMPImageHeader);
	fwrite(&temp, 4, 1, fp);

	BMPImageHeader bmpImageInfo;
	memset(&bmpImageInfo, 0, sizeof(BMPImageHeader));
	
	assert(sizeof(BMPImageHeader) == 40);

	bmpImageInfo.Size = sizeof(BMPImageHeader);
	bmpImageInfo.Width = m_width;
	bmpImageInfo.Height = m_height;
	bmpImageInfo.BitCount = m_bytesPerPixel*8;
	bmpImageInfo.Compression = BMP_COMPRESSION_NONE;
	
	fwrite(&bmpImageInfo, sizeof(BMPImageHeader), 1, fp);

	//now write the data

	int totalPitch = m_width*m_bytesPerPixel;
	int imagePitch = totalPitch;

	while (totalPitch%4 != 0) totalPitch++;

	for (int y=0; y < m_height; y++)
	{
		for (int x=0; x < m_width; x++)
		{
			byte *pSrc = &m_pPixels[y*imagePitch+x*m_bytesPerPixel];
			
			if (m_bytesPerPixel == 4)
			{
				glColorBytes converted(pSrc[2], pSrc[1], pSrc[0], pSrc[3]);
				fwrite(&converted, 4, 1, fp);
			} else
			{
				fwrite(&pSrc[2], 1, 1, fp);
				fwrite(&pSrc[1], 1, 1, fp);
				fwrite(&pSrc[0], 1, 1, fp);

			}

			if (x == m_width-1)
			{
				//writing padding if needed
				for (int j=0; j < (totalPitch-imagePitch); j++)
				{
					char crap = 0;
					fwrite(&crap, 1, 1, fp);
				}
			}

		}
	}
	fclose(fp);

}

void SoftSurface::FillAlphaBit( unsigned char alpha )
{
	
	if (GetSurfaceType() != SURFACE_RGBA)
	{
		assert(!"This would only work on rgba surfaces, naturally");
		return;
	}

	glColorBytes *pSrcImage = (glColorBytes*)GetPointerToPixel(0, 0);

	for (int y=0; y < GetHeight(); y++)
	{
		for (int x=0; x < GetWidth(); x++)
		{
			(pSrcImage[x]).a = alpha;
		}
		pSrcImage += GetWidth();
	} 
}

void SoftSurface::Scale( int newX, int newY )
{
	assert(m_pPixels && "Uh... no image is loaded");
	assert(!m_pitchOffset && "The pitch has dead pixel space in it (loaded a .bmp?).. this func won't work right..");

	//create new buffer the same size (memory-wise) as our current one
	int dataSize = newX*newY*m_bytesPerPixel;
	byte *pTemp = new byte[dataSize];

	int targetWidth = newX;
	int targetHeight = newY;
	int targetPitch = targetWidth*m_bytesPerPixel;
	float ratioX = (float)m_width/(float)targetWidth;
	float ratioY = (float)m_height/(float)targetHeight;
	
	//step through our current data pixel by pixel, from the top left, reading horizontally
	for (int y=0; y < targetHeight; y++)
	{
		for (int x=0; x < targetWidth; x++)
		{
			memcpy(&pTemp[ (y*targetPitch) + x*m_bytesPerPixel ],  GetPointerToPixel(x*ratioX, y*ratioY), m_bytesPerPixel);
		}
	}

	//switch to using the new surface we just made

	SAFE_DELETE_ARRAY(m_pPixels);
	m_pPixels = pTemp;
	m_usedPitch = targetPitch;
	m_width = targetWidth;
	m_height = targetHeight;
	
	int oldMem = m_memUsed;
	m_memUsed = m_width*m_height*m_bytesPerPixel;
	GetBaseApp()->ModMemUsed(m_memUsed - oldMem); //adjust for our memory counter

}