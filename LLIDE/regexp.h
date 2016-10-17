#ifndef __REGEXP_H__
#define __REGEXP_H__

typedef struct __regexp_state
{
	size_t code;
	struct __regexp_state* out;
	struct __regexp_state* out_alter;
	size_t last_list;
} regexp_state;

typedef struct
{
	regexp_state** state;
	size_t cnt;
} regexp_list;

typedef struct __regexp_dstate
{
	regexp_list ls;
	struct __regexp_dstate* next[256];
	struct __regexp_dstate* left;
	struct __regexp_dstate* right;
} regexp_dstate;

char* regexp_infx2postfx(char*);
regexp_state* regexp_postfx2nfa(char*);
regexp_dstate* dstate_start(regexp_state*);
int regexp_match(regexp_dstate*, char*);
void regexp_init(void);

#endif /* __REGEXP_H__ */