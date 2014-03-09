//Generation 3
//The GBA Games

//Ruby/Sapphire/Emerald
//Fire Red/Leaf Green

#include "game_gba.h"

//http://bulbapedia.bulbagarden.net/wiki/Save_data_structure_in_Generation_III

static const uint16_t gba_to_codepage[256] = {
	0x0020,0x3042,0x3044,0x3046,0x3048,0x304a,0x304b,0x304d,
	0x304f,0x3051,0x3053,0x3055,0x3057,0x3059,0x305b,0x305d,
	0x305f,0x3061,0x3064,0x3066,0x3068,0x306a,0x306b,0x306c,
	0x306d,0x306e,0x306f,0x3072,0x3075,0x3078,0x307b,0x307e,
	0x307f,0x3080,0x3081,0x3082,0x3084,0x3086,0x3088,0x3089,
	0x308a,0x308b,0x308c,0x308d,0x308f,0x3092,0x3093,0x3041,
	0x3043,0x3045,0x3047,0x3049,0x3083,0x3085,0x3087,0x304c,
	0x304e,0x3050,0x3052,0x3054,0x3056,0x3058,0x305a,0x305c,
	0x305e,0x3060,0x3062,0x3065,0x3067,0x3069,0x3070,0x3073,
	0x3076,0x3079,0x307c,0x3071,0x3074,0x3077,0x307a,0x307d,
	0x0020,0x30a2,0x30a4,0x30a6,0x30a8,0x30aa,0x30ab,0x30ad,
	0x30af,0x30b1,0x30b3,0x30b5,0x30b7,0x30b9,0x30bb,0x30bd,
	0x30bf,0x30c1,0x30c4,0x30c6,0x30c8,0x30ca,0x30cb,0x30cc,
	0x30cd,0x30ce,0x30cf,0x30d2,0x30d5,0x30d8,0x30db,0x30de,
	0x30df,0x30e0,0x30e1,0x30e2,0x30e4,0x30e6,0x30e8,0x30e9,
	0x30ea,0x30eb,0x30ec,0x30ed,0x30ef,0x30f2,0x30f3,0x30a1,
	0x30a3,0x30a5,0x30a7,0x30a9,0x30e3,0x30e5,0x30e7,0x30ac,
	0x30ae,0x30b0,0x30b2,0x30b4,0x30b6,0x30b8,0x30ba,0x30bc,
	0x30be,0x30c0,0x30c2,0x30c5,0x30c7,0x30c9,0x30d0,0x30d3,
	0x30d6,0x30d9,0x30dc,0x30d1,0x30d4,0x30d7,0x30da,0x30dd,
	0x0020,0x0030,0x0031,0x0032,0x0033,0x0034,0x0035,0x0036,
	0x0037,0x0038,0x0039,0x0021,0x003f,0x002e,0x002d,0x0020,
	0x2026,0x201c,0x201d,0x2018,0x2019,0x2642,0x2640,0x0020,
	0x002c,0x0020,0x002f,0x0041,0x0042,0x0043,0x0044,0x0045,
	0x0046,0x0047,0x0048,0x0049,0x004a,0x004b,0x004c,0x004d,
	0x004e,0x004f,0x0050,0x0051,0x0052,0x0053,0x0054,0x0055,
	0x0056,0x0057,0x0058,0x0059,0x005a,0x0061,0x0062,0x0063,
	0x0064,0x0065,0x0066,0x0067,0x0068,0x0069,0x006a,0x006b,
	0x006c,0x006d,0x006e,0x006f,0x0070,0x0071,0x0072,0x0073,
	0x0074,0x0075,0x0076,0x0077,0x0078,0x0079,0x007a,0x0020,
	0x0020,0x00c4,0x00d6,0x00dc,0x00e4,0x00f6,0x00fc,0x0020,
	0x0020,0x0020,0x0020,0x2192,0x0020,0x0020,0x000a,0x0000,
};

enum {
	GBA_SAVE_BLOCK_COUNT = 14,
	GBA_BLOCK_LENGTH = 0x1000,
	GBA_BLOCK_DATA_LENGTH = 0xFF0,
	GBA_BLOCK_FOOTER_LENGTH = 0x10,
};

gba_savetype_t gba_detect_save_type(void *ptr, size_t size) {
	return GBA_TYPE_UNKNOWN;
}

#pragma pack(push, 1)
//16 byte footer for every 0xFF0 data block
typedef struct {
	uint32_t padding;
	uint16_t section_id;
	uint16_t checksum;
	uint32_t mark; //usually 25 20 01 08
	uint32_t save_index; //counts the number of times saved as well
} gba_footer_t;
#pragma pack(pop)

static inline gba_footer_t *get_block_footer(void*ptr) {
	return ptr + GBA_BLOCK_DATA_LENGTH;
}

static inline uint16_t get_block_checksum(void*ptr) {
	return gba_checksum(ptr, GBA_BLOCK_DATA_LENGTH);
}

void gba_fix_checksum(void*ptr) {
	for(size_t i = 0; i < GBA_SAVE_BLOCK_COUNT; ++i) {
		size_t offset = i * GBA_BLOCK_LENGTH;
		gba_footer_t *footer = get_block_footer(ptr + offset);
		footer->checksum = get_block_checksum(ptr + offset);
	}
}