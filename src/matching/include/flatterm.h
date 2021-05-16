#ifndef MC_FLATTERM
#define MC_FLATTERM

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
} term;

flatterm* flatterm_init();

flatterm* flatterm_init_complete(term* first, term* end);

term* flatterm_push_back(flatterm* ft);

term* flatterm_first(flatterm* ft);

term* flatterm_end(flatterm* ft);

void flatterm_free(flatterm* ft);

void flatterm_print(flatterm* ft);


#endif