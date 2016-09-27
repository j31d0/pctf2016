#include <stdio.h>

/* 
 * CS:APP Data Lab 
 * 
 * Park jihee, 20150349
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting an integer by more
     than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implent floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operators (! ~ & ^ | + << >>)
     that you are allowed to use for your implementation of the function. 
     The max operator count is checked by dlc. Note that '=' is not 
     counted; you may use as many of these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
/* 
 * bitNor - ~(x|y) using only ~ and & 
 *   Example: bitNor(0x6, 0x5) = 0xFFFFFFF8
 *   Legal ops: ~ &
 *   Max ops: 8
 *   Rating: 1
 */
int bitNor(int x, int y) {
	/*compute nor by de morgan's laws, ~( x | y )==( ~x & ~y ).*/
	int notX = ~x;
       	int notY = ~y;
	int norXY = (notX & notY);

	return norXY;

}

/* 
 * copyLSB - set all bits of result to least significant bit of x
 *   Example: copyLSB(5) = 0xFFFFFFFF, copyLSB(6) = 0x00000000
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int copyLSB(int x) {
	/*use rightshift to find lsb, and use property of arthmetic
	 * shift to copy lsb.*/
	int lsb = ( x << 31 );
	int copylsb = (lsb >> 31);

	return copylsb;

}

/* 
 * isEqual - return 1 if x == y, and 0 otherwise 
 *   Examples: isEqual(5,5) = 1, isEqual(4,5) = 0
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int isEqual(int x, int y) {
	/*use xor to find equality. if x ^ y ==0 then x ==y, 
	 * so compute boolean not to expression.*/
	int xor = x ^ y;
	int equal = !xor;

	return equal;

}

/* 
 * bitMask - Generate a mask consisting of all 1's 
 *   lowbit and highbit
 *   Examples: bitMask(5,3) = 0x38
 *   Assume 0 <= lowbit <= 31, and 0 <= highbit <= 31
 *   If lowbit > highbit, then mask should be all 0's
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int bitMask(int highbit, int lowbit) {
	/* In first try, i use xor operation and divide cases
	 * for (highbit-lowbit)<0. but in second try I found
	 * more efficient way, to make unsymmetric situation
	 * and use & operator.*/
	int minus1 = ~0;
	int usualBitmask = ~((minus1 << highbit) << 1) & (minus1 << lowbit);

  	return usualBitmask;
}

/*
 * bitCount - returns count of number of 1's in word
 *   Examples: bitCount(5) = 2, bitCount(7) = 3
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 40
 *   Rating: 4
 */
int bitCount(int x) {
	/* basic idea is bottom-up divide and conquer, 
	 * to use 32 >> n operations at once (in nth 
	 * add operation). to implement this addition 
	 * properly, I made 5 masks.*/
	int maskA = 0x55;
	int maskB = 0x33;
	int maskC = 0x0f;
	int maskD = 0xff;
	int maskE = 0xff;
	int x1=0;
	int x2=0;
	int x3=0;
	int x4=0;
	int x5=0;

	maskA = maskA | (maskA << 8);
	maskA = maskA | (maskA << 16);
	maskB = maskB | (maskB << 8);
	maskB = maskB | (maskB << 16);
	maskC = maskC | (maskC << 8);
	maskC = maskC | (maskC << 16);
	maskD = maskD | (maskD << 16);
	maskE = maskE | (maskE << 8);
	x1 = (x & maskA) + ((x >> 1) & maskA);
	x2 = (x1 & maskB) + ((x1 >> 2) & maskB);
	x3 = (x2 & maskC) + ((x2 >> 4) & maskC);
	x4 = (x3 & maskD) + ((x3 >> 8) & maskD);
	x5 = (x4 & maskE) + ((x4 >> 16) & maskE);
	
	return x5;

}

/* 
 * TMax - return maximum two's complement integer 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmax(void) {
	/* make all-1 bits and make 0 to top bit.*/

	return ~(1<<31);

}

/* 
 * isNonNegative - return 1 if x >= 0, return 0 otherwise 
 *   Example: isNonNegative(-1) = 0.  isNonNegative(0) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 3
 */
int isNonNegative(int x) {
	/*just see top bit.*/
	int sign = x >> 31;

  	return !sign;
}

/* 
 * addOK - Determine if can compute x+y without overflow
 *   Example: addOK(0x80000000,0x80000000) = 0,
 *            addOK(0x80000000,0x70000000) = 1, 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3
 */
int addOK(int x, int y) {
	/*base on idea about sign of operands.
	 * if x and y has different sign, it never
	 * overflows. but if x and y has same sign,
	 * (x+y) have to be same sign, but it has 
	 * different sign only if it overflows.*/

	int signX = x;
	int signY = y;
	int signSum = (x + y);
	int overflow = (signX ^ signSum) & ( signY ^ signSum);
	return !(overflow >> 31);

}

/* 
 * rempwr2 - Compute x%(2^n), for 0 <= n <= 30
 *   Negative arguments should yield negative remainders
 *   Examples: rempwr2(15,2) = 3, rempwr2(-35,3) = -3
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3
 */
int rempwr2(int x, int n) {
	/* if x is positive, take bit mask with 2^n-1.
	 * if x is negative, make x positive(by -x)
	 * and do same thing. 
	 * maybe~ values are used to calculate abs(x).*/

	int minusOne = ~0;
	int signMask = x >> 31;
	int maybeNegOfX = (signMask ^ x);
	int maybeAddOne = signMask & 1;
	int nnBitMask = (minusOne << n);
	int nBitMask = ~nnBitMask;
	int absx = maybeNegOfX + maybeAddOne;
	int someDivide = absx & nBitMask; // computes |x| % 2^n
	int ans = (signMask ^ someDivide) + maybeAddOne;

	return ans;

}

/* 
 * isLess - if x < y  then return 1, else return 0 
 *   Example: isLess(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLess(int x, int y) {
	/*if it is not same sign, x is less only if x is negative.
	 * if it is same sign, we take substraction and look
	 * the sign of result. i use masks to implement 
	 * if-else terms like 
	 * (conditionMask & exp1) | (!conditionMask & exp2)*/
	int isDiffSign = (x ^ y);
	int minusY = (~y + 1);
	int compare = (x + minusY); // if x < y, compare have minus sign.

	return ((((~isDiffSign) & compare) | (isDiffSign & x)) >> 31) & 1;




}

/* 
 * absVal - absolute value of x
 *   Example: absVal(-1) = 1.
 *   You may assume -TMax <= x <= TMax
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 10
 *   Rating: 4
 */
int absVal(int x) {
	/*use technique used in rempwr2 to compute
	 * x>=0 -> return x, else return -x.*/
	int signMask = x >> 31;
	int maybeNegOfX = (signMask ^ x);
	int maybeAddOne = signMask & 1;
	return maybeNegOfX + maybeAddOne;

}

/*
 * isPower2 - returns 1 if x is a power of 2, and 0 otherwise
 *   Examples: isPower2(5) = 0, isPower2(8) = 1, isPower2(0) = 0
 *   Note that no negative number is a power of 2.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 4
 */
int isPower2(int x) {
	/* make Mask to compute sign of x, and 
	 * use some techniques to determine power of 2.
	 * if x is power of 2, -x includes x because -x + x = 0.
	 * so -x's bit represntation is like 11...110000000..0.
	 * buf if x is not power of 2, -x can't include all bits
	 * of x, because -x have 0 which locations x have 1.
	 *  */

	int minusX = (~x + 1);
	int isPlus = (x >> 31 | !x);
	int isPosP2 = ((x & minusX) ^ x); 
	return !(isPlus | isPosP2);

}

/* 
 * float_neg - Return bit-level equivalent of expression -f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representations of
 *   single-precision floating point values.
 *   When argument is NaN, return argument.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 10
 *   Rating: 2
 */
unsigned float_neg(unsigned uf) {
	/*first, take exception case to return argument.
	 * and otherwise, flip sign bit by xor, and returns.*/
	
	if((uf & 0x7FFFFFFF) > 0x7F800000)
		return uf;
	return uf ^ 0x80000000;

}

/* 
 * float_half - Return bit-level equivalent of expression 0.5*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_half(unsigned uf) {
	/* first extract exponent, sign, significand,
	 * and take exception case. the main problem was
	 * divide expression case and compute rounding.
	 * I use several if-case to divide exponents.
	 * and to fit in IEEE round-to-even, I search
	 * last significand, to handle this. 
	 * I use round policy if (significand & 3 ==3),
	 * this is the only case for make half of float.
	 * the more specific cases are implemented in float_i2f.*/
	int others = (uf & 0x7FFFFFFF);
	int uf2 = uf;

	if(others >= 0x7F800000)
		return uf;
	if(others > 0x008FFFFF)
		return uf2 - 0x00800000;
	
	uf2 = ((uf2+((uf2 & 3) == 3)) >> 1);
	return uf2 & 0xBFFFFFFF;


}

/* 
 * float_i2f - Return bit-level equivalent of expression (float) x
 *   Result is returned as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point values.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_i2f(int x) {
	/*the main algorithm for this function is
	 * first to extract sign and absolute value
	 * second to take exception case(tmin and 0)
	 * third to make rounding policy.
	 * first and second was explained at upwards,
	 * and I use three branches to round.  
	 * Also I use several techniques to reduce num.of Op.*/

	int mask = (x >> 31);
	int sign = (mask << 31);
	int y = x;
	int z = y;
	int minusone = 0xFFFFFFFF;
	int count = minusone;
	int base = 0;
	int ans = 0;
	int thres = 0;
	int moves = 0;
	int tmp = 0;

	if( x == sign )
	//this branches takes 0 and tmin, and return
	//0 and (1 << 31 | (127+31) << 23), respectively.
		return (mask & 0xCF000000);
	if(sign)
	{	
		y = -x;
		z = y;
	}
	while(z)
	{
		// to find logy base 2 for convert
		// fixed-point value to floating-point value.
		count += 1;
		z = z >> 1;
	}
	base = (sign | ((count + 127) << 23));
	y = y ^ ( 1 << count);
	thres = 23 - count;
	moves= count - 24;
	if ( 0 <= thres)
	{
		//if logy < 24, don't need rounding.
		return base | (y << thres); 
	}
	ans = base | (y >> (-thres));
	tmp = ans + 1;
	//branches by rounding policy ( round-to-nearest-even )
	if( ((y >>moves) & 3) == 3)
		// if y's rounding bits are 1.1.....
		return tmp;
	if(((y << (31 - moves)) + minusone) < minusone)
		// if y's rounding bits are 0.1xxxxxx, 
		// but not 0.1000000000 ( check by integer underflow )
	{		
		return tmp;
	}
	return ans;
}

int* hex_to_int(float a)
{
    float* x = (float*)&a;
    *x = *x * 1337.0;
    return (int*)x;
}
int main()
{
    int a;
    float *ftemp;
    
    scanf("%08x",&a);
    float* c = (float*)&a;
    printf("%f\n",*c);
    *c = *c * 1337.0;
    printf("%f\n", *c);
    int b = (int)(*c);
    printf("%08x\n",b);
    printf("%08x\n",float_i2f(a));
    //printf("%08x\n", ftemp);
}

