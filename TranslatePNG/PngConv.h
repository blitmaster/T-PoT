//=============================================================================
// T-PoT - Total Commander file system plug-in for iPod and iPhone devices
//-----------------------------------------------------------------------------
// File:			PngConv.h
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

#ifndef TRANSLATEPNG_H_
#define TRANSLATEPNG_H_

#define PNGCONV_ERR_OK				0
#define PNGCONV_ERR_STAT_SRC		1
#define PNGCONV_ERR_READ_SRC		2
#define PNGCONV_ERR_NOT_PNG			3
#define PNGCONV_ERR_WRITE_DST		4
#define PNGCONV_ERR_WRONG_FORMAT	5
#define PNGCONV_ERR_ZLIB			6
#define PNGCONV_ERR_INFLATED_OVER	7
#define PNGCONV_ERR_DEFLATED_OVER	8
#define PNGCONV_ERR_LAST         	8

// ----------------------------------------------------------------------------

typedef unsigned int uint32;
typedef unsigned char uint8;

class CPngConv {

	// ------------------------------------------------------------------------

	private:
		typedef struct st_PngChunk{
			uint32 length;
			uint32 name;
			uint8 *data;
			uint32 crc;
			struct st_PngChunk *next;
		} t_PngChunk;

		typedef struct {
			uint32 width;
			uint32 height;
			uint8 bit_depth;
			uint8 color_type;
			uint8 compression;
			uint8 filter;
			uint8 interlace;
		} t_PngIHDRChunk;

		static const uint8 c_pngHeader[8];
		static const uint32 c_pngIHDR;
		static const uint32 c_pngIDAT;
		static const uint32 c_pngIEND;
		static const uint32 c_pngCgBl;
		
		static const char *c_ErrorMessage[PNGCONV_ERR_LAST + 1];

		bool m_containsCgBI;
		t_PngIHDRChunk m_pngIHDR;

		unsigned int m_maxInflatedBufSize;
		unsigned int m_maxDeflatedBufSize;
		t_PngChunk *m_chunks;
		t_PngChunk *m_lastChunk;
		uint8 *m_inflatedBuf;
		uint8 *m_deflatedBuf;
		FILE *m_fSrc;
		FILE *m_fDst;

	// ------------------------------------------------------------------------

	private:

		void Init();
		void ReadChunks(void);
		void ParseChunks(void);
		void InverseRedBlue(uint8 *buf);
		void WritePNG(char *);
		unsigned long CheckCRC(uint32, uint8 *, int);
		void CleanUp();

	public:

		CPngConv();
		bool IsConverted() const { return m_containsCgBI; }	
		int Convert(char *srcFilename, char *dstFilename);
		const char *GetErrorMessage(int errorCode);
};

#endif
