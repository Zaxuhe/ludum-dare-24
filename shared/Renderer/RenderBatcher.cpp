#include "PlatformPrecomp.h"
#include "RenderBatcher.h"
#include "SoftSurface.h"
#include "util/GLESUtils.h"

RenderBatcher::RenderBatcher()
{
}

RenderBatcher::~RenderBatcher()
{
}

void RenderBatcher::BlitEx(Surface *pSurf, rtRectf dst, rtRectf src, unsigned int rgba)
{

	if (dst.bottom < 0) return;
	if (dst.top > GetOrthoRenderSizeYf()) return;

	if ( GET_ALPHA(rgba) == 0)
	{
		return;	
	}

	if (m_batchEvents.size() > 0 && m_batchEvents.back().m_pSurf == pSurf)
	{
		//we can just add to the existing event
		m_batchEvents.back().m_vertCount += 6;
	} else
	{
		//create a new event
		RenderBatchEvent event;
		event.m_type = RenderBatchEvent::TYPE_2D_BLIT;
		event.m_pSurf = pSurf;
		event.m_vertCount = 6;
		m_batchEvents.push_back(event);

	}

	glColorBytes color;

	color.a = GET_ALPHA(rgba);
	color.r = GET_RED(rgba);
	color.g = GET_GREEN(rgba);
	color.b = GET_BLUE(rgba);

	if (!pSurf->IsLoaded()) return;
	m_pSurf = pSurf;
	m_verts.resize(m_verts.size()+6);

	BatchVert * vertices = &m_verts[m_verts.size()-6];
	
	//if we were doing quads/fans it would be this:
	//	0 1
	//	3 2

	//but since we're not I duplicate a few verts for two triangles after I fill out the 4 I care about


	//  0 1   3
    //    2   5 4
	vertices[0].vPos.x = dst.left; vertices[0].vPos.y = dst.top; vertices[0].vPos.z = 0;
	vertices[1].vPos.x = dst.right; vertices[1].vPos.y = dst.top; vertices[1].vPos.z = 0;
	vertices[2].vPos.x = dst.right; vertices[2].vPos.y = dst.bottom; vertices[2].vPos.z = 0;
	vertices[5].vPos.x = dst.left; vertices[5].vPos.y = dst.bottom; vertices[5].vPos.z = 0;
	
	//rgba = MAKE_RGBA(255,255,255,255);

	//colors
	vertices[0].color = color;
	vertices[1].color = color;
	vertices[2].color = color;
	vertices[5].color = color;

	float originalWidth = (float)m_pSurf->GetRawTextureWidth();
	float originalHeight = (float)m_pSurf->GetRawTextureHeight();
	 
	//another step to convert the coordinates into ratios
	static float texW;
	texW = float(originalWidth)/float(m_pSurf->GetWidth());
	static float texH;
	texH = float(originalHeight)/float(m_pSurf->GetHeight());
	
	vertices[0].vUv.x = src.left/float( originalWidth ) * texW;
	vertices[0].vUv.y =  (1-texH) + texH*( (originalHeight-src.top) /float(originalHeight));

	vertices[1].vUv.x = src.right/float(originalWidth) * texW;
	vertices[1].vUv.y =  (1-texH) + texH*( (originalHeight-src.top) /float(originalHeight));

	vertices[2].vUv.x = src.right/float(originalWidth) * texW;
	vertices[2].vUv.y =  1-(texH*(src.bottom /(float(originalHeight))));
	
	vertices[5].vUv.x = src.left/float(originalWidth) * texW;
	vertices[5].vUv.y =  1-(texH*(src.bottom /(float(originalHeight))));

	//fill in the rest since we're using standard triangles and not quads..
	vertices[3] = vertices[0];
	vertices[4] = vertices[2];
}


void RenderBatcher::Flush(eFlushMode mode)
{
	if (mode == FLUSH_SETUP || mode == FLUSH_SETUP_RENDER_UNSETUP)
	{

		if (m_batchEvents.empty()) return;
		CHECK_GL_ERROR();
		
		if (m_verts.empty())
		{
			assert(!"Hmm?");
			m_batchEvents.clear();
			return;
		}

		glEnable(GL_BLEND);
		glVertexPointer(3, GL_FLOAT, sizeof(BatchVert), &m_verts[0].vPos.x);
		glTexCoordPointer(2, GL_FLOAT,  sizeof(BatchVert), &m_verts[0].vUv.x);
		glColorPointer(4, GL_UNSIGNED_BYTE,  sizeof(BatchVert), &m_verts[0].color);
		glEnableClientState(GL_COLOR_ARRAY);
		glColor4x(1 << 16, 1 << 16, 1 << 16, 1 << 16);
	}

	if (mode == FLUSH_RENDER || mode == FLUSH_SETUP_RENDER_UNSETUP)
	{

		CHECK_GL_ERROR();

		int curPrim = 0;
		RenderBatchEvent event;

		while (!m_batchEvents.empty())
		{
			event = m_batchEvents.front();
		//	LogMsg("Rendering event, %d prims", event.m_vertCount);

			m_batchEvents.pop_front();
			if (event.m_pSurf) event.m_pSurf->Bind();
			::glDrawArrays(GL_TRIANGLES,curPrim,event.m_vertCount);
			curPrim += event.m_vertCount;
			CHECK_GL_ERROR();
		}
	}

	if (mode == FLUSH_UNSETUP || mode == FLUSH_SETUP_RENDER_UNSETUP)
	{
		glDisable( GL_BLEND );
		glDisableClientState(GL_COLOR_ARRAY);
		glColor4x(1 << 16, 1 << 16, 1 << 16, 1 << 16); //need this for some gl drivers

		m_verts.clear();
		assert(m_batchEvents.empty());
	}
}

void RenderBatcher::BlitRawImage(int x, int y, SoftSurface &soft)
{
	
	if (soft.GetSurfaceType() == SoftSurface::SURFACE_RGBA)
	{
		BlitRawImage(x, y, soft.GetPixelData(), soft.GetWidth(), soft.GetHeight(), soft.GetUsesAlpha());
	} else
	{
		//do a slow convert to RGBA first
		static SoftSurface s;
		s.Init(soft.GetWidth(), soft.GetHeight(), SoftSurface::SURFACE_RGBA);
		s.Blit(0,0,&soft);
		BlitRawImage(x, y, s.GetPixelData(), s.GetWidth(), s.GetHeight(), soft.GetUsesAlpha());
	}
}

void RenderBatcher::BlitRawImage(int dstX, int dstY,  byte *pRaw, int width, int height, bool bNeedsAlpha )
{
	static vector<CL_Vec3s> vertBuff;  //vertex of shorts
	static int lastSize = 0;

	int dataSize = width*height;
	if (lastSize != dataSize)
	{
		if (dataSize > int(vertBuff.size()))
		{
			vertBuff.resize(dataSize);
		}

		lastSize = dataSize;
		
		
		//setup coords
		for (int y=0; y < height; y++)
			for (int x = 0; x < width; x++)
			{
				vertBuff[x + y*width] = CL_Vec3s(dstX+x,dstY+y,0);
			}
	}
	
	SetupOrtho();
	glDisable(GL_TEXTURE_2D);
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4, GL_UNSIGNED_BYTE, 0, pRaw);

	if (bNeedsAlpha)
	{
		glEnable(GL_BLEND);
	}

	glVertexPointer(3, GL_SHORT, 0, &vertBuff[0]);
	glPushMatrix();
	//glTranslatef(-0.5f, 0.5f, 0);
	::glDrawArrays(GL_POINTS, 0, dataSize);
	glPopMatrix();

	glDisableClientState(GL_COLOR_ARRAY);
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	glEnable(GL_TEXTURE_2D);
	if (bNeedsAlpha)
	{
		glDisable(GL_BLEND);
	}
	CHECK_GL_ERROR();

}

void RenderBatcher::BuildVert(BatchVert *pVert, const CL_Vec3f *pVerts, const CL_Vec3f *vNormals, const CL_Vec2f *TexCords, glColorBytes glColorByte, CL_Mat4f *pMatrix, int srcVertIndex)
{
assert(TexCords == NULL);

	pVert->vPos = pMatrix->get_transformed_point(pVerts[srcVertIndex]);
	pVert->color = glColorByte;
	pVert->vNormal = vNormals[srcVertIndex];

}
void RenderBatcher::glDrawArrays( const CL_Vec3f *pVerts, const CL_Vec3f *vNormals, const CL_Vec2f *texCoords, glColorBytes glColorByte, CL_Mat4f *pMatrix, int glDrawMode,  int primCount )
{
	assert(texCoords == NULL  && "Ok, now would be a good time to add support for that");

	if (glDrawMode != GL_TRIANGLE_STRIP)
	{
		assert(!"Uh, we don't handle anything else right now.. implement yourself?  Won't be hard, as we just convert to triangles anyway below");
		return;
	}
	
	assert(primCount >= 4 && "We don't handle only 3 verts.. add handling here");

	assert(primCount%4 == 0); 
	assert(primCount == 4);
	int newVertsToAdd = (primCount + (primCount/2));

	m_verts.resize(m_verts.size()+newVertsToAdd);

	BatchVert * vertices = & (m_verts[m_verts.size()-newVertsToAdd]);

	//convert this:
	// 	0 2  tri-strip box							0 3 1 2
	// 	1 3

	//to this:

	//  0(0) 2(2)
	//  1(1)

	//		 2(2)
	//  1(0) 3(1)


	BuildVert(&vertices[0], pVerts, vNormals, texCoords, glColorByte, pMatrix, 1);
	BuildVert(&vertices[1], pVerts, vNormals, texCoords, glColorByte, pMatrix, 2);
	BuildVert(&vertices[2], pVerts, vNormals, texCoords, glColorByte, pMatrix, 0);

	BuildVert(&vertices[3+0], pVerts, vNormals, texCoords, glColorByte, pMatrix, 3);

	//OPTIMIZE: Just copy these two verts, don't rebuild them

	BuildVert(&vertices[3+1], pVerts, vNormals, texCoords, glColorByte, pMatrix, 2);
	BuildVert(&vertices[3+2], pVerts, vNormals, texCoords, glColorByte, pMatrix, 1);

}

void RenderBatcher::Flush3D( bool bUseNormals, bool bUseTextures, bool bUseColorByte )
{
	
	CHECK_GL_ERROR();
	if (m_verts.empty()) return;
	glVertexPointer(3, GL_FLOAT, sizeof(BatchVert), &m_verts[0].vPos.x);
	
	if (bUseTextures)
	{
		m_pSurf->Bind();
		glTexCoordPointer(2, GL_FLOAT,  sizeof(BatchVert), &m_verts[0].vUv.x);
	} else
	{
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	}

	if (bUseNormals)
	{
		glEnableClientState(GL_NORMAL_ARRAY);	
		glNormalPointer(GL_FLOAT,  sizeof(BatchVert), &m_verts[0].vNormal.x);
	}

	if (bUseColorByte)
	{
		glColorPointer(4, GL_UNSIGNED_BYTE,  sizeof(BatchVert), &m_verts[0].color);
		glEnableClientState(GL_COLOR_ARRAY);
		glColor4x(1 << 16, 1 << 16, 1 << 16, 1 << 16);
	}
	
	CHECK_GL_ERROR();

	::glDrawArrays(GL_TRIANGLES, 0, m_verts.size());
	CHECK_GL_ERROR();
	glDisable( GL_BLEND );

	if (bUseColorByte)
	{
		glDisableClientState(GL_COLOR_ARRAY);
		glColor4x(1 << 16, 1 << 16, 1 << 16, 1 << 16); //need this for some gl drivers
	}

	if (bUseNormals)
	{
		glDisableClientState(GL_NORMAL_ARRAY);	
	}

	if (bUseTextures)
	{

	} else
	{
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	}
	m_verts.clear();
}