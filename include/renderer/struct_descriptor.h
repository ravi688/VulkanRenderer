/*
  		*----------------------- CPU LOW LEVEL STUFFS -----------------------------*

		NOT 0111 (decimal 7) 			10101011 (decimal 171)
		 = 	1000 (decimal 8) 		= 	01010100 (decimal 84)
		For unsigned integers, the bitwise complement of a number is the "mirror reflection" of the number across the half-way 
		point of the unsigned integer's range. For example, for 8-bit unsigned integers, NOT x = 255 - x, which can be visualized 
		on a graph as a downward line that effectively "flips" an increasing range from 0 to 255, to a decreasing range from 255 to 0. 
		A simple but illustrative example use is to invert a grayscale image where each pixel is stored as an unsigned integer

		
		AND 0110 (decimal 6)
			0001 (decimal 1)
  		=	0000 (decimal 0)
  		Because 6 AND 1 is zero, 6 is divisible by two and therefore even.


		XOR 0101 (decimal 5)
			0011 (decimal 3)
  		= 	0110 (decimal 6)
  		The bitwise XOR may be used to invert selected bits in a register (also called toggle or flip). 
  		Any bit may be toggled by XORing it with 1.
  		Assembly language programmers and optimizing compilers sometimes use XOR as a short-cut to setting the value of a 
  		register to zero. Performing XOR on a value against itself always yields zero, and on many architectures this 
  		operation requires fewer clock cycles and memory than loading a zero value and saving it to the register.

	
		Little-endian ordering:	a left shift by 8 positions increases the byte address by 1,
								a right shift by 8 positions decreases the byte address by 1.
		Big-endian ordering:	a left shift by 8 positions decreases the byte address by 1,
								a right shift by 8 positions increases the byte address by 1.

		
		A left arithmetic shift by n is equivalent to multiplying by 2n (provided the value does not overflow), 
		while a right arithmetic shift by n of a two's complement value is equivalent to taking the floor of division by 2n. 
		If the binary number is treated as ones' complement, then the same right-shift operation results in division by 2n 
		and rounding toward zero.
 
		
		In a logical shift, zeros are shifted in to replace the discarded bits. Therefore, the logical and arithmetic left-shifts are exactly the same.
		However, as the logical right-shift inserts value 0 bits into the most significant bit, instead of copying the sign bit, 
		it is ideal for unsigned binary numbers, while the arithmetic right-shift is ideal for signed two's complement binary numbers.
 
 		
 		In C-family languages, the logical shift operators are "<<" for left shift and ">>" for right shift.
 		
 		In C#, the right-shift is an arithmetic shift when the first operand is an int or long. 
 		If the first operand is of type uint or ulong, the right-shift is a logical shift
 
		@https://bits.stephan-brumme.com/
		CIRCULAR SHIFTS: 

		Left-Rotate: uint32_t y = (x << n) | (x >> (32 - n));
 		However, a shift by 0 bits results in undefined behavior in the right-hand expression (x >> (32 - n)) because 32 - 0 is 32, 
 		and 32 is outside the range 0–31 inclusive. A second try might result in:
 		uint32_t y = n ? (x << n) | (x >> (32 - n)) : x;
		where the shift amount is tested to ensure that it does not introduce undefined behavior. 
		However, the branch adds an additional code path and presents an opportunity for timing analysis and attack, 
		which is often not acceptable in high-integrity software.
		In addition, the code compiles to multiple machine instructions, which is often less efficient than the processor's 
		native instruction.
		To avoid the undefined behavior and branches under GCC and Clang, the following is recommended. 
		The pattern is recognized by many compilers, and the compiler will emit a single rotate instruction:
		uint32_t y = (x << n) | (x >> (-n & 31));

		Powers of two have one and only one bit set in their binary representation.

		ABSOLUTE VALUE OF A FLOAT (IEEE 754):

		01: float myAbs(float x)
		02: {
		03:   // copy and re-interpret as 32 bit integer
		04:   int casted = *(int*) &x;
		05:   // clear highest bit
		06:   casted &= 0x7FFFFFFF;
		07:
		08:   // re-interpret as float
		09:   return *(float*)&casted;
		10: }
		IEEE 754 floats' highest bit is the so-called sign bit: set to 1 for negative and 0 for positive numbers (incl. zero).
		We just always set it to 0 − and we are done !
		The data type "float" requires 32 bits. Unfortunately, C does not allow any bit operations on floats.
		To work around this issue, these 32 bits are re-interpreted as a 32 bit integer (line 4).
		Then clearing the sign bit becomes simple and easy (line 6), however, the required code looks a bit nasty.
		The built-in C function fabs() is translated into its FABS intrinsic when the data value is already on the FPU stack.
		When data is stored in main memory and will remain there after fabs, the above trick outperforms the FPU by far
		because one FPU load and one store operation can be saved.
		These FPU load/store can vastly skew the performance chart, so please be careful with interpreting the results.

		ABSOLUTE VALUE OF A INTEGER:

		01: int myAbs(int x)
		02: {
		03:   const int bit31 = x >> 31;
		04:   return (x ^ bit31) - bit31;
		05: }
		All major processors represent negative numbers using the two's-complement which is defined as:
		for x ≥ 0 → x
		for x < 0 → NOT(x) + 1
		On the lowest level, computers provide logical bit shifts and arithmetic bit shifts.
		Both shifts differ in handling how to fill the empty bits on the left side.
		Logical shifts insert zeros while arithmetic shifts replicate the formerly highest bit.
		Whereas signed integers are arithmetically shifted in C, unsigned integers are logically shifted.
		In our case x is shifted arithmetically 31 times to the right which basically erases its value
		and spreads the highest bit. That means, line 3 evaluates either to 0x00000000 (→ 0) or
		0xFFFFFFFF (→ −1).
		Note: 32 bit systems require a shift by 31, 64 bit systems require a shift by 63 accordingly.
		Consequently, line 4 turns out to be (x XOR 0) − 0 for positive values of x (including x=0).
		x XOR 0 is still x and x − 0 remains x, too. So for positive x we get x ≥ 0 → x.
		We saw that for negative values of x, bit31 is set to 0xFFFFFFFF.
		Line 4 is then (x XOR 0xFFFFFFFF) − 0xFFFFFFFF. The bracketed XOR is equivalent to NOT(x) and
		the constant −0xFFFFFFFF turns out to be −(-1) = +1.
		In the end, the whole term is NOT(x) + 1, exactly what we wanted: for x < 0 → NOT(x) + 1
		Note: Current C++ compilers (Microsoft, GCC and Intel) implemented a special assembler code sequence
        		for abs which runs faster than the shown algorithm on x86 CPUs (see below for its source code).
 
		APPROXIMATE INVERSE OF A FLOAT:

		01: float inverse(float x)
		02: {
		03:   // re-interpret as a 32 bit integer
		04:   unsigned int *i = (unsigned int*)&x;
		05:
		06:   // adjust exponent
		07:   *i = 0x7F000000 - *i;
		08:   //*i = 0x7EEEEEEE - *i;
		09:
		10:   return x;
		11: }
		The standard IEEE-754 defines the binary layout of floating point numbers.
		All major CPUs (or better to say: FPUs) follow this standard.
		The data type "float" is a 32 bit wide floating point number consisting of a 23 bit mantissa,
		an 8 bit exponent and a sign bit. Because the exponent can be negative, too, it is implemented using
		a bias of 28-1 = 127. That means, 127 is added to the actual exponent to keep it always positive.
		Written in a formula: x = sign * mantissa * 2exponent .
		Note: To save one bit, the most significant bit of the mantissa is omitted because it is by definition always set to one.
		Note, too: Binary exponents below −127 or above +127 cannot be represented by "float" (≈ ±1038 decimal).
		The inverse can be written as: inverse(a) → a-1 or more specific: inverse(ab) → a-b .
		If speed is more important than precision, we can approximate x-1 by computing sign * mantissa * 2-exponent .
		The only difference to the original formula is the minus in front of exponent.
		Because the mantissa remains unadjusted, the result will slightly deviate from the true inverse value.
		In the end, a simple subtraction of the exponent bits, often performed in merely one CPU cycle,
		does the trick (line 7): exponent → 127 − exponent
		The constant 0x7F000000 represents 127 shifted left by 23 bits.
		A brute force search revealed that a few constants expose a better overall accuracy and cut down the
		error to about 4%. So far, 0x7EEEEEEE (line 8) seems to be the best candidate.
		My brute force search didn't analyze all possible numbers, that means, there might be even better constants.
		But you should keep in mind that only for 0x7F000000 we get inverse(1) = 1, which might be important for
		many algorithms. No other constant can produce the same result for inverse(1).

		BIT COUNT: 
		
		01: unsigned int countBits(unsigned int x)
		02: {
		03:   // count bits of each 2-bit chunk
		04:   x  = x - ((x >> 1) & 0x55555555);
		05:   // count bits of each 4-bit chunk
		06:   x  = (x & 0x33333333) + ((x >> 2) & 0x33333333);
		07:   // count bits of each 8-bit chunk
		08:   x  = x + (x >> 4);
		09:   // mask out junk
		10:   x &= 0xF0F0F0F;
		11:   // add all four 8-bit chunks
		12:   return (x * 0x01010101) >> 24;
		13: }

		DETECTS ZERO BYTES INSIDE A 32 BIT INTEGER:

		01: bool hasZeroByte(unsigned int x)
		02: {
		03:   return ((x - 0x01010101) & ~x & 0x80808080) != 0;
		04: }
		05:
		One of the most prominent example of detecting zeroes is strlen. To improve performance many bytes are examined in parallel.
		The expression x − 0x01010101 sets the highest bit of a byte if a single input byte is within the input set
		{ 0x00, 0x81, 0x82, 0x83, ..., 0xFF } because the result will be { 0xFF, 0x80, 0x81, 0x82, ..., 0xFE }
		On the other hand, ~x sets the highest bit of a byte if a single input byte is within the input set
		{ 0x00, 0x01, 0x02, 0x03, ..., 0x7F } because the result will be { 0xFF, 0xFE, 0xFD, 0xFC, ..., 0x80 }
		The only value contained in both input sets is 0x00, exactly what we are looking for.
		That means, only for x = 0 the highest bit is still set after ANDing: (x − 0x01010101) & ~x
		Please do not be confused by the fact that a result ≠ 0 actually indicates a zero byte.
		Then, all bits except for the highest bit of each byte are reset to zero using AND 0x80808080.
		The resulting value is equal to zero if all (!) of these highest bits are zero.
		If at least one is still set to 1, caused by a zero byte, the comparison ≠ 0 gives "true".

		ENDIANESS:

		01: bool isLittleEndian()
		02: {
		03:   // 16 bit value, represented as 0x0100 on Intel, and 0x0001 else
		04:   short pattern = 0x0001;
		05:   // access first byte, will be 1 on Intel, and 0 else
		06:   return *(char*) &pattern == 0x01;
		07: }
		Intel's x86 chips (and of cause compatible chips from AMD etc.) rely on "little endian":
		Numeric values consisting of multiple bytes have increasing numeric significance with increasing memory addresses.
		The opposite, called big endian, is true for Motorola chips, PowerPC and various other designs.
		A few CPUs can even switch at runtime like ARM or newer PowerPC or x64.
		The programming language C itself usually takes care of the endianness but whenever direct memory access is
		requires by the software, it might be important to know the endianness of the system.

		
		@https://en.wikipedia.org/wiki/Data_structure_alignment
		DATA STRUCTURE ALIGNMENT
		
		A memory address a is said to be n-byte aligned when a is a multiple of n bytes (where n is a power of 2).
		An n-byte aligned address would have a minimum of log2(n) least-significant zeros when expressed in binary.
		A memory access is said to be aligned when the data being accessed is n bytes long and the datum address is n-byte aligned.

		COMPUTING PADDING
		The following formulas provide the number of padding bytes required to align the start of a data structure 
		(where mod is the modulo operator):
		padding = (align - (offset mod align)) mod align
		aligned = offset + padding
        		= offset + ((align - (offset mod align)) mod align)
        Since the alignment is by definition a power of two,[a] the modulo operation can be reduced to a bitwise boolean AND operation.
        padding = (align - (offset & (align - 1))) & (align - 1)
        		= (-offset & (align - 1))
		aligned = (offset + (align - 1)) & ~(align - 1)
        		= (offset + (align - 1)) & -align


        ALIGNMENTS WHEN COMPILING FOR 32 BIT X86
        
        A char (one byte) will be 1-byte aligned.
		A short (two bytes) will be 2-byte aligned.
		An int (four bytes) will be 4-byte aligned.
		A long (four bytes) will be 4-byte aligned.
		A float (four bytes) will be 4-byte aligned.
		A double (eight bytes) will be 8-byte aligned on Windows and 4-byte aligned on Linux (8-byte with -malign-double compile time option).
		A long long (eight bytes) will be 4-byte aligned.
		A long double (ten bytes with C++Builder and DMC, eight bytes with Visual C++, twelve bytes with GCC) will be 8-byte aligned with C++Builder, 2-byte aligned with DMC, 8-byte aligned with Visual C++, and 4-byte aligned with GCC.
		Any pointer (four bytes) will be 4-byte aligned. (e.g.: char*, int*)

		ALIGNMENT DIFFERENCES WHEN COMPILING FOR 64 BIT X64

		A long (eight bytes) will be 8-byte aligned.
		A double (eight bytes) will be 8-byte aligned.
		A long long (eight bytes) will be 8-byte aligned.
		A long double (eight bytes with Visual C++, sixteen bytes with GCC) will be 8-byte aligned with Visual C++ and 16-byte aligned with GCC.
		Any pointer (eight bytes) will be 8-byte aligned.
 
 		struct MixedData  		//After compilation in 32-bit x86 machine
		{
    		char Data1;  		//1 byte
    		char Padding1[1];  	//1 byte for the following 'short' to be aligned on a 2 byte boundary
							   	//assuming that the address where structure begins is an even number
    		short Data2; 		//2 bytes
    		int Data3;  		//4 bytes - largest structure member
    		char Data4; 		//1 byte
    		char Padding2[3]; 	//3 bytes to make total size of the structure 12 bytes
		};

		struct FinalPadShort 
		{
			short s;
 		 	char n[3];
		}; //sizeof(FilePadShort) = 6 bytes on 32 bit machine

		For memory alignment and performance -> https://fylux.github.io/2017/07/11/Memory_Alignment/
		For portable data alignment -> https://www.codesynthesis.com/~boris/blog/2009/04/06/cxx-data-alignment-portability/


		*----------------------- GPU LOW LEVEL STUFFS -----------------------------*
		
 */

#pragma once

#include <renderer/defines.h>
#define STRUCT_FIELD_MAX_NAME_SIZE 32
#define STRUCT_DESCRIPTOR_MAX_NAME_SIZE 32
#define STRUCT_FIELD_INVALID_HANDLE 0xFFFF

enum 
{
	// 0 = UNDEFINED
	STRUCT_FIELD_FLOAT = 1,
	STRUCT_FIELD_INT,
	STRUCT_FIELD_UINT,
	STRUCT_FIELD_VEC4,
	STRUCT_FIELD_VEC3,
	STRUCT_FIELD_VEC2,
	STRUCT_FIELD_IVEC4,
	STRUCT_FIELD_IVEC3,
	STRUCT_FIELD_IVEC2,
	STRUCT_FIELD_UVEC4,
	STRUCT_FIELD_UVEC3,
	STRUCT_FIELD_UVEC2,
	STRUCT_FIELD_MAT4,
	STRUCT_FIELD_MAT3,
	STRUCT_FIELD_MAT2,
	STRUCT_FIELD_MAX
};

typedef struct struct_field_t
{
	char name[STRUCT_FIELD_MAX_NAME_SIZE];
	u8 type;
	u16 size;
	u16 alignment;

	INTERNAL u16 offset;
} struct_field_t;

typedef struct struct_descriptor_t
{
	char name[STRUCT_DESCRIPTOR_MAX_NAME_SIZE];
	u8 type;
	u16 field_count;
	struct_field_t* fields;
	
	INTERNAL void* ptr;
	INTERNAL u32 size;
} struct_descriptor_t;

typedef u16 struct_field_handle_t;

BEGIN_CPP_COMPATIBLE

RENDERER_API void struct_descriptor_map(struct_descriptor_t* descriptor, void* ptr);
RENDERER_API void struct_descriptor_unmap(struct_descriptor_t* descriptor);
RENDERER_API void struct_descriptor_recalculate(struct_descriptor_t* descriptor);
RENDERER_API u32 struct_descriptor_sizeof(struct_descriptor_t* descriptor);
RENDERER_API struct_field_handle_t struct_descriptor_get_field_handle(struct_descriptor_t* descriptor, const char* field_name);
RENDERER_API void struct_descriptor_set_value(struct_descriptor_t* descriptor, struct_field_handle_t handle, const void* const in);
RENDERER_API void struct_descriptor_set_float(struct_descriptor_t* descriptor, struct_field_handle_t handle, const float* const in);
RENDERER_API void struct_descriptor_set_int(struct_descriptor_t* descriptor, struct_field_handle_t handle, const int* const in);
RENDERER_API void struct_descriptor_set_uint(struct_descriptor_t* descriptor, struct_field_handle_t handle, const uint* const in);
RENDERER_API void struct_descriptor_set_vec4(struct_descriptor_t* descriptor, struct_field_handle_t handle, const float* const in);
RENDERER_API void struct_descriptor_set_vec3(struct_descriptor_t* descriptor, struct_field_handle_t handle, const float* const in);
RENDERER_API void struct_descriptor_set_vec2(struct_descriptor_t* descriptor, struct_field_handle_t handle, const float* const in);
RENDERER_API void struct_descriptor_set_ivec4(struct_descriptor_t* descriptor, struct_field_handle_t handle, const int* const in);
RENDERER_API void struct_descriptor_set_ivec3(struct_descriptor_t* descriptor, struct_field_handle_t handle, const int* const in);
RENDERER_API void struct_descriptor_set_ivec2(struct_descriptor_t* descriptor, struct_field_handle_t handle, const int* const in);
RENDERER_API void struct_descriptor_set_uvec4(struct_descriptor_t* descriptor, struct_field_handle_t handle, const uint* const in);
RENDERER_API void struct_descriptor_set_uvec3(struct_descriptor_t* descriptor, struct_field_handle_t handle, const uint* const in);
RENDERER_API void struct_descriptor_set_uvec2(struct_descriptor_t* descriptor, struct_field_handle_t handle, const uint* const in);
RENDERER_API void struct_descriptor_set_mat4(struct_descriptor_t* descriptor, struct_field_handle_t handle, const float* const in);
RENDERER_API void struct_descriptor_set_mat3(struct_descriptor_t* descriptor, struct_field_handle_t handle, const float* const in);
RENDERER_API void struct_descriptor_set_mat2(struct_descriptor_t* descriptor, struct_field_handle_t handle, const float* const in);

RENDERER_API void struct_descriptor_get_value(struct_descriptor_t* descriptor, struct_field_handle_t handle, void* const in);
RENDERER_API void struct_descriptor_get_float(struct_descriptor_t* descriptor, struct_field_handle_t handle, float* const out);
RENDERER_API void struct_descriptor_get_int(struct_descriptor_t* descriptor, struct_field_handle_t handle, int* const out);
RENDERER_API void struct_descriptor_get_uint(struct_descriptor_t* descriptor, struct_field_handle_t handle, uint* const out);
RENDERER_API void struct_descriptor_get_vec4(struct_descriptor_t* descriptor, struct_field_handle_t handle, float* const out);
RENDERER_API void struct_descriptor_get_vec3(struct_descriptor_t* descriptor, struct_field_handle_t handle, float* const out);
RENDERER_API void struct_descriptor_get_vec2(struct_descriptor_t* descriptor, struct_field_handle_t handle, float* const out);
RENDERER_API void struct_descriptor_get_ivec4(struct_descriptor_t* descriptor, struct_field_handle_t handle, int* const out);
RENDERER_API void struct_descriptor_get_ivec3(struct_descriptor_t* descriptor, struct_field_handle_t handle, int* const out);
RENDERER_API void struct_descriptor_get_ivec2(struct_descriptor_t* descriptor, struct_field_handle_t handle, int* const out);
RENDERER_API void struct_descriptor_get_uvec4(struct_descriptor_t* descriptor, struct_field_handle_t handle, uint* const out);
RENDERER_API void struct_descriptor_get_uvec3(struct_descriptor_t* descriptor, struct_field_handle_t handle, uint* const out);
RENDERER_API void struct_descriptor_get_uvec2(struct_descriptor_t* descriptor, struct_field_handle_t handle, uint* const out);
RENDERER_API void struct_descriptor_get_mat4(struct_descriptor_t* descriptor, struct_field_handle_t handle, float* const out);
RENDERER_API void struct_descriptor_get_mat3(struct_descriptor_t* descriptor, struct_field_handle_t handle, float* const out);
RENDERER_API void struct_descriptor_get_mat2(struct_descriptor_t* descriptor, struct_field_handle_t handle, float* const out);


END_CPP_COMPATIBLE
