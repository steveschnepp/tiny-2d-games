#ifndef BITMAP_H
#define BITMAP_H

#include <nds.h>

static inline void setBit(u32 *bitmap, u32 bit) {
	bitmap[bit>>5] |= 1<<(bit&31);
}

static inline void clearBit(u32 *bitmap, u32 bit) {
	bitmap[bit>>5] &= ~(1<<(bit&31));
}

static inline bool getBit(u32 *bitmap, u32 bit) {
	return bitmap[bit>>5] & (1<<(bit&31));
}

static inline u32 findClearBit(u32 *bitmap, u32 limit) {
	u32 bit = 0;
	u32 index;

	while(bit < limit) {
		index = bit>>5;
		if(bitmap[index] != 0xFFFFFFFF) {
			for(int i = 0; i < 32; i++)
				if((bitmap[index] & (1<<i)) == 0)
					return bit+i;
		}
		bit += 32;
	}
	return limit;
}

static inline u32 findSetBit(u32 *bitmap, u32 limit) {
	u32 bit = 0;
	u32 index;

	while(bit < limit) {
		index = bit>>5;
		if(bitmap[index]) {
			for(int i = 0; i < 32; i++)
				if(bitmap[index] & (1<<i))
					return bit+i;
		}
		bit += 32;
	}
	return limit;
}

static inline int bitCount(u32 val) {
	val = val - ((val >> 1) & 0x55555555);
	val = (val & 0x33333333) + ((val >> 2) & 0x33333333);
	return (((val + (val >> 4)) & 0xF0F0F0F) * 0x1010101) >> 24;
}

static inline u32 numSet(u32 *bitmap, u32 limit) {
	u32 count = 0;
	for(u32 i = 0; i < limit/32; i++)
		count += bitCount(bitmap[i]);
	return count;
}

static inline u32 numClear(u32 *bitmap, u32 limit) {
	return limit - numSet(bitmap, limit);
}
#endif /* BITMAP_H */

