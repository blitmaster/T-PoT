//=============================================================================
// T-PoT - Total Commander file system plug-in for iPod and iPhone devices
//-----------------------------------------------------------------------------
// File:			PngConv.cpp
// Purpose:			Apple-modified PNG translation to PNG-compliant format.
// Limitations:		-
// Platform:		Win32
//-----------------------------------------------------------------------------
// Based on MHW's and Guillaume Cottenceau's PNG conversion code. 
//-----------------------------------------------------------------------------
// Copyright (c) 2007-2009, Scythal
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the distribution.
// * Neither the name of the software nor the names of its contributors may be 
//   used to endorse or promote products derived from this software without 
//   specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY ITS AUTHOR ``AS IS'' AND ANY EXPRESS OR 
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN 
// NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED 
// TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//=============================================================================

#pragma warning(disable:4996)

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <zlib.h>

#include "mac_png_conv.h"

#define SWAP32(a) ((((a)&0xFF)<<24)|(((a)&0xFF00)<<8)|(((a)&0xFF0000)>>8)|(((a)&0xFF000000)>>24))

#define ASSERT(val,expect,except) { if ((val) != (expect)) throw (except); }
#define ASSERTNOT(val,expectn,except) { if ((val) == (expectn)) throw (except); }

const uint8 CPngConv::c_pngHeader[8] = { 137, 80, 78, 71, 13, 10, 26, 10 };
const uint32 CPngConv::c_pngIHDR = 0x52444849;
const uint32 CPngConv::c_pngIDAT = 0x54414449;
const uint32 CPngConv::c_pngIEND = 0x444E4549;
const uint32 CPngConv::c_pngCgBl = 0x49426743;

const char *CPngConv::c_ErrorMessage[] = {
	"No error",
	"Could not find source file",
	"Could not read source file",
	"Source file is not a PNG file",
	"Could not write to destination file",
	"Source file has not the expected format",
	"Error in zlib library",
	"Inflated image overflow",
	"Deflated image overflow"
};

// ----------------------------------------------------------------------------

CPngConv::CPngConv()
{
	Init();
}

void CPngConv::Init()
{
	m_chunks = NULL;
	m_lastChunk = NULL;
	m_inflatedBuf = NULL;
	m_deflatedBuf = NULL;
	m_fSrc = NULL;
	m_fDst = NULL;
	m_containsCgBI = false;
}

int CPngConv::Convert(char *srcFilename, char *dstFilename)
{
	uint8 buf[8];

	Init();
	m_sourceFilename = srcFilename;
	#if C_MALLOC_DEBUG
	unlink(C_MALLOC_FILE);
	#endif
	try {
		ASSERTNOT(
			m_fSrc = fopen(m_sourceFilename, "rb"), 
			NULL, PNGCONV_ERR_STAT_SRC);
		ASSERT(
			fread(buf, 1, 8, m_fSrc), 
			8, PNGCONV_ERR_READ_SRC);
		ASSERT(
			memcmp(buf, c_pngHeader, 8),
			0, PNGCONV_ERR_NOT_PNG);
		ReadChunks();
		ParseChunks();
		WritePNG(dstFilename);
		CleanUp();
	}
	catch(int errorCode) {
		CleanUp();
		if (m_fDst)
			// If destination file was created, removes it
			unlink(dstFilename);
		return errorCode;
	}
	return PNGCONV_ERR_OK;
}

void CPngConv::ReadChunks() {
	#define FREAD(buf,n) { if (fread(buf, 1, (n), m_fSrc) != (n)) throw PNGCONV_ERR_READ_SRC; }

	t_PngChunk *chunk;

	while(1) {
		chunk = (t_PngChunk *)malloc(sizeof(t_PngChunk));
		if (m_chunks == NULL)
			m_chunks = chunk;
		else
			m_lastChunk->next = chunk;
		m_lastChunk = chunk;
		chunk->next = NULL;

		FREAD(&chunk->length, 4);
		chunk->length = SWAP32(chunk->length);
		chunk->data = (uint8 *)malloc(chunk->length);
		FREAD(&chunk->name, 4);
		FREAD(chunk->data, chunk->length)
		FREAD(&chunk->crc, 4);
		chunk->crc = SWAP32(chunk->crc);
		if (chunk->name == c_pngIHDR) {
			// IHDR
			m_pngIHDR = *(t_PngIHDRChunk*)chunk->data;
			m_pngIHDR.width = SWAP32(m_pngIHDR.width);
			m_pngIHDR.height = SWAP32(m_pngIHDR.height);
			
			// Estimates the maximum size of the inflated and deflated IDAT:
			// the 100 term is to prevent overflow in very small images due
			// to header overhead (never occurred though). Terrible guess.
			m_maxInflatedBufSize = 4*(m_pngIHDR.width + 1)*m_pngIHDR.height;
			m_maxDeflatedBufSize = 100 + m_maxInflatedBufSize;
		} else if (chunk->name == c_pngCgBl) {
			// CgBl
			m_containsCgBI = true;
		} else if (chunk->name == c_pngIEND) {
			// IEND - End of img.
			break;
		}
	}

	if (m_pngIHDR.compression != 0 || m_pngIHDR.bit_depth != 8 || m_pngIHDR.color_type != 6 ||
	    m_pngIHDR.filter != 0 || m_pngIHDR.interlace != 0) {
	    if (m_containsCgBI)
			throw PNGCONV_ERR_WRONG_FORMAT;
	}
	m_inflatedBuf = (uint8 *)malloc(m_maxInflatedBufSize);
	m_deflatedBuf = (uint8 *)malloc(m_maxDeflatedBufSize);
}

void CPngConv::ParseChunks() {
	t_PngChunk *chunk = m_chunks;
	int ret;
	z_stream infstrm, defstrm;

	// Poke at any IDAT m_chunks and de/recompress them
	while(chunk) {
		if (m_containsCgBI && chunk->name == c_pngIDAT) {
			
			infstrm.zalloc = Z_NULL;
			infstrm.zfree = Z_NULL;
			infstrm.opaque = Z_NULL;
			infstrm.avail_in = chunk->length;
			infstrm.next_in = chunk->data;
			infstrm.next_out = m_inflatedBuf;
			infstrm.avail_out = m_maxInflatedBufSize;
			
			// Inflate using raw inflation
			if (inflateInit2(&infstrm,-8) != Z_OK) {
				throw PNGCONV_ERR_ZLIB;
			}
			ret = inflate(&infstrm, Z_NO_FLUSH);
			switch (ret) {
				case Z_NEED_DICT:
					ret = Z_DATA_ERROR;     /* and fall through */
				case Z_DATA_ERROR:
				case Z_MEM_ERROR:
					inflateEnd(&infstrm);
					throw PNGCONV_ERR_ZLIB;
			}
			
			ret = inflateEnd(&infstrm);
			
			if (infstrm.total_out > m_maxInflatedBufSize) {
				throw PNGCONV_ERR_INFLATED_OVER;
			}
			// Now deflate again, the regular, PNG-compatible, way
			InverseRedBlue(m_inflatedBuf);
			
			defstrm.zalloc = Z_NULL;
			defstrm.zfree = Z_NULL;
			defstrm.opaque = Z_NULL;
			defstrm.avail_in = infstrm.total_out;
			defstrm.next_in = m_inflatedBuf;
			defstrm.next_out = m_deflatedBuf;
			defstrm.avail_out = m_maxDeflatedBufSize;
			deflateInit(&defstrm, Z_DEFAULT_COMPRESSION);
			ret = deflate(&defstrm, Z_FINISH);

			if (defstrm.total_out > m_maxDeflatedBufSize) {
				throw PNGCONV_ERR_DEFLATED_OVER;
			}
			if (defstrm.total_out > chunk->length)
				chunk->data = (uint8 *)realloc(chunk->data, defstrm.total_out);
			memcpy(chunk->data, m_deflatedBuf, defstrm.total_out);
			chunk->length = defstrm.total_out;
			chunk->crc = CheckCRC(chunk->name, chunk->data, chunk->length);
		}
		chunk = chunk->next;
	}
}

void CPngConv::InverseRedBlue(uint8 *buf)
{
	uint32 x, y;
	uint8 *pixel = buf, tmp;
	
	for (y = 0; y < m_pngIHDR.height; y++) {
		pixel++;
		for (x= 0; x < m_pngIHDR.width; x++) {
			tmp = pixel[0];
			pixel[0] = pixel[2];
			pixel[2] = tmp;
			pixel += 4;
		}
	}
}

void CPngConv::WritePNG(char *filename) {
	int tmp;
	t_PngChunk *chunk = m_chunks;
	
	m_fDst = fopen(filename, "wb");
	ASSERT(
		fwrite(c_pngHeader, 1, 8, m_fDst),
		8, PNGCONV_ERR_WRITE_DST);
	while(chunk) {
		tmp = SWAP32(chunk->length);
		chunk->crc = SWAP32(chunk->crc);
		if (chunk->name != c_pngCgBl) {
			// Anything but a CgBI
			ASSERT(
				fwrite(&tmp, 1, 4, m_fDst),
				4, PNGCONV_ERR_WRITE_DST);
			ASSERT(
				fwrite(&chunk->name, 1, 4, m_fDst),
				4, PNGCONV_ERR_WRITE_DST);
			if (chunk->length > 0) {
				ASSERT(
					fwrite(chunk->data, 1, chunk->length, m_fDst),
					chunk->length, PNGCONV_ERR_WRITE_DST);
			}
			ASSERT(
				fwrite(&chunk->crc, 1, 4, m_fDst),
				4, PNGCONV_ERR_WRITE_DST);
		}
		chunk = chunk->next;
	}
}

unsigned long CPngConv::CheckCRC(uint32 name, uint8 *buf, int len)
{
	uint32 crc;
	
	crc = crc32(0, (uint8 *)&name, 4);
	return crc32(crc, buf, len);
}

void CPngConv::CleanUp()
{
	if (m_fSrc)
		fclose(m_fSrc);
	if (m_fDst)
		fclose(m_fDst);
	if (m_inflatedBuf)
		free(m_inflatedBuf);
	if (m_deflatedBuf)
		free(m_deflatedBuf);
	if (m_chunks) {
		uint32 name = 0;
		t_PngChunk *chunk = m_chunks, *tmpChunk;
	
		while(chunk) {
			tmpChunk = chunk->next;
			name = chunk->name;
			free(chunk->data);
			free(chunk);
			chunk = tmpChunk;
		}
	}
}

const char *CPngConv::GetErrorMessage(int errorCode)
{
	if (errorCode < 0 || errorCode > PNGCONV_ERR_LAST)
		return "Unknown error message";
	else
		return c_ErrorMessage[errorCode];
}