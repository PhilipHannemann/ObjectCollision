#pragma once

namespace util 
{

	int countUTF8segments(char seq_start) 
	{
		unsigned char c = *reinterpret_cast<unsigned char*>(&seq_start);
		if (c < 0x80)
			return 1;
		else if (c < 0xE0)
			return  2;
		else if (c < 0xF0)
			return  3;
		else if (c < 0xF8)
			return  4;

		return 0;
	}

	int countUTF8segments_reverse(int last_index, const char* data)
	{
		const unsigned char *c = reinterpret_cast<const unsigned char*>(data);

		if (c[last_index] < 0x80)
			return 1;

		int count = 1;
		for(int i = last_index; i >= 0; i--) 
		{
			if (c[i] >= 0xC0)
				return count;
		
			count++;
		}

		return 0;
	}





}