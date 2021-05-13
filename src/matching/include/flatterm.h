#ifndef MC_FLATTERM
#define MC_FLATTERM

#include "list.h"

enum matchtype {
	MT_CONSTANT, // 2 or f
	MT_VARIABLE, // f_ or x_
	MT_SEQUENCE // f__ or x__
};

enum functype {
	FT_NOTAFUNC,
	FT_PREFIX,
	FT_INFIX
};

typedef struct term {
	enum matchtype m_type;
	enum functype f_type;
	char* symbol;
	struct term* end;
} term;



#endif