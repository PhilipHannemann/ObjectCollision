#include "stdafx.h"
#include "Compression.h"
#include "lz4\lz4.h"
#include <iostream>
#include <limits>

using namespace util::lz4;
#define BLOCK_BYTES 1024 * 64

namespace util {

	Compression::Compression()
	{
	}


	Compression::~Compression()
	{
	}

	void Compression::decompress(std::istream & is, std::ostream & os)
	{

		LZ4_streamDecode_t lz4StreamDecode_body;
		LZ4_streamDecode_t* lz4StreamDecode = &lz4StreamDecode_body;

		char decBuf[2][BLOCK_BYTES];
		int  decBufIndex = 0;

		LZ4_setStreamDecode(lz4StreamDecode, NULL, 0);

		for (;;) {
			char cmpBuf[LZ4_COMPRESSBOUND(BLOCK_BYTES)];
			int  cmpBytes = 0;

			{
				is.read((char*)&cmpBytes, sizeof(cmpBytes));
				if (!is || cmpBytes <= 0) {
					break;
				}

				is.read(cmpBuf, cmpBytes);
				if (is.gcount() != cmpBytes) {
					break;
				}
			}

			{
				char* const decPtr = decBuf[decBufIndex];
				const int decBytes = LZ4_decompress_safe_continue(lz4StreamDecode, cmpBuf, decPtr, cmpBytes, BLOCK_BYTES);
				if (decBytes <= 0) {
					break;
				}
				os.write(decPtr, decBytes);
			}

			decBufIndex = (decBufIndex + 1) % 2;
		}

	}

	int Compression::compress(std::istream &is, std::ostream &os)
	{
		return compress(is, os, std::numeric_limits<int>::max());
	}

	int Compression::compress(std::istream &is, std::ostream &os, int length)
	{
		LZ4_stream_t lz4Stream_body;
		LZ4_stream_t* lz4Stream = &lz4Stream_body;

		char inpBuf[2][BLOCK_BYTES];
		int  inpBufIndex = 0;

		LZ4_resetStream(lz4Stream);

		size_t total = 0;
		int tcomp = 0;

		for (;;) {
			char* const inpPtr = inpBuf[inpBufIndex];

			if (length - total < BLOCK_BYTES)
				is.read(inpPtr, length - total);
			else
				is.read(inpPtr, BLOCK_BYTES);

			size_t inpBytes = is.gcount();
			total += inpBytes;
			if (0 == inpBytes) {
				break;
			}

			{
				char cmpBuf[LZ4_COMPRESSBOUND(BLOCK_BYTES)];
				const int cmpBytes = LZ4_compress_fast_continue(lz4Stream, inpPtr, cmpBuf, (int)inpBytes, sizeof(cmpBuf), 1);
				if (cmpBytes <= 0) {
					break;
				}
				os.write((char*)&cmpBytes, sizeof(cmpBytes));
				os.write(cmpBuf, cmpBytes);
				tcomp += cmpBytes + sizeof(cmpBytes);
			}

			inpBufIndex = (inpBufIndex + 1) % 2;

			if (total == length)
				break;

		}

		int i = 0;
		//terminate with size 0
		os.write((char*)&i, sizeof(i));

		return (tcomp + sizeof(i));
	}
}
