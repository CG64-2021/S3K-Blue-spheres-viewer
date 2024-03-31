//Reference: https://info.sonicretro.org/SPG:Calculations#Pixel_and_Subpixel

#ifndef __M_UTILS__
#define __M_UTILS__

#define fixed_t int16_t
#define fixedu_t uint16_t
#define FRACBITS 8
#define SHIFT_MASK ((1 << FRACBITS) - 1)

#ifndef __BIG_ENDIAN__
	#define SHORT(x) ((uint16_t)((x >> 8) | (x << 8)))
#else
	#define SHORT(x) (x)
#endif

extern fixed_t SINCOSLIST[];
extern fixedu_t ANGLELIST[];

fixed_t FixedSin(uint16_t hex_angle);
fixed_t FixedCos(uint16_t hex_angle);
fixed_t FixedMul(fixed_t a, fixed_t b);
fixed_t FixedInt(fixed_t value);
fixedu_t FixedUint(fixedu_t value);

#endif