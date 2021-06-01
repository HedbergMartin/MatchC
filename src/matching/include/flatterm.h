#ifndef MC_FLATTERM
#define MC_FLATTERM
#include "match_types.h"
#include "variable_vector.h"

typedef struct flatterm flatterm;

typedef struct term {
	struct term* next;
	struct term* prev;
	struct term* parent;
	struct term* end;
	int argno;
	enum matchtype m_type;
	enum functype f_type;
	char* symbol;
	int id;
} term;

flatterm* flatterm_init();

flatterm* flatterm_init_complete(term* first, term* end, char* pattern, int variables);

term* flatterm_push_back(flatterm* ft);

term* flatterm_first(flatterm* ft);

term* flatterm_end(flatterm* ft);

char* flatterm_pattern(flatterm* ft);

void flatterm_free(flatterm* ft);

void flatterm_print(flatterm* ft);


#endif