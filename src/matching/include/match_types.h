#ifndef MC_MATCH_TYPES
#define MC_MATCH_TYPES

//Can use bitsetting, but seems unneccessary with this few options
#define M_SYMBOL 1
#define M_VARIABLE 2
#define M_SEQUENCE 3
#define M_STAR 4
#define M_FUNCTION_SYMBOL 5
#define M_FUNCTION_VARIABLE 6

enum matchtype {
	MT_CONSTANT, // 2 or f
	MT_VARIABLE, // f_ or x_
	MT_SEQUENCE, // f__ or x__
	MT_STAR // f___ or x___
};

enum functype {
	FT_NOTAFUNC,
	FT_PREFIX,
	FT_INFIX
};

#endif
