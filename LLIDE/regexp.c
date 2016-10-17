#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "regexp.h"

#define REGEXP_CONCAT 16

enum
{
	Regexp_Match = 256,
	Regexp_Split = 257
};

static regexp_state match_state = { Regexp_Match };
static size_t state_cnt;

static regexp_list l1, l2;
static size_t list_id;

static regexp_dstate *dstate_list;

char* regexp_infx2postfx(char* infx)
{
	size_t alternate_cnt = 0, atomic_cnt = 0;
	static char buffer[8192];
	char* dest = buffer;

	struct
	{
		size_t alternate_cnt;
		size_t atomic_cnt;
	} subexpr_stack[100], *stack_ptr;
	stack_ptr = subexpr_stack;

	if (strlen(infx) >= 8192 / 2)
	{
		return NULL;
	}
	for (; *infx; infx++)
	{
		switch (*infx)
		{
		case '(':
			if (atomic_cnt > 1)
			{
				atomic_cnt--;
				*dest++ = REGEXP_CONCAT;
			}
			if (stack_ptr >= subexpr_stack + 100)
			{
				return NULL;
			}
			stack_ptr->alternate_cnt = alternate_cnt;
			stack_ptr->atomic_cnt = atomic_cnt;
			stack_ptr++;
			alternate_cnt = 0;
			atomic_cnt = 0;
			break;

		case ')':
			if (stack_ptr == subexpr_stack)
			{
				return NULL;
			}
			if (atomic_cnt == 0)
			{
				return NULL;
			}
			while (--atomic_cnt > 0)
			{
				*dest++ = REGEXP_CONCAT;
			}
			for (; alternate_cnt > 0; alternate_cnt--)
			{
				*dest++ = '|';
			}
			stack_ptr--;
			alternate_cnt = stack_ptr->alternate_cnt;
			atomic_cnt = stack_ptr->atomic_cnt;
			atomic_cnt++;
			break;

		case '|':
			if (atomic_cnt == 0)
			{
				return NULL;
			}
			while (--atomic_cnt > 0)
			{
				*dest++ = REGEXP_CONCAT;
			}
			alternate_cnt++;
			break;
		
		case '*':
		case '+':
		case '?':
			if (atomic_cnt == 0)
			{
				return NULL;
			}
			*dest++ = *infx;
			break;

		default:
			if (atomic_cnt > 1)
			{
				atomic_cnt--;
				*dest++ = REGEXP_CONCAT;
			}
			*dest++ = *infx;
			atomic_cnt++;
			break;
		}
	}

	if (stack_ptr != subexpr_stack)
	{
		return NULL;
	}
	while (--atomic_cnt > 0)
	{
		*dest++ = REGEXP_CONCAT;
	}
	for (; alternate_cnt > 0; alternate_cnt--)
	{
		*dest++ = '|';
	}
	*dest = 0;
	return buffer;
}

static regexp_state* state_make(int c, regexp_state *out, regexp_state *outalt)
{
	regexp_state* s = (regexp_state*)malloc(sizeof(regexp_state));
	state_cnt++;
	s->last_list = 0;
	s->code = c;
	s->out = out;
	s->out_alter = outalt;
	return s;
}

typedef union __regexp_ptrlist
{
	union __regexp_ptrlist* next;
	regexp_state* s;
} regexp_ptrlist;

typedef struct
{
	regexp_state* start;
	regexp_ptrlist* out;
} regexp_frag;

static regexp_frag frag_make(regexp_state* start, regexp_ptrlist* out)
{
	regexp_frag n = { start, out };
	return n;
}

static regexp_ptrlist* list_make(regexp_state** outp)
{
	regexp_ptrlist* l;

	l = (regexp_ptrlist*)outp;
	l->next = NULL;
	return l;
}

static void ptrlist_patch(regexp_ptrlist* l, regexp_state* s)
{
	regexp_ptrlist* next;

	for (; l; l = next)
	{
		next = l->next;
		l->s = s;
	}
}

static regexp_ptrlist* ptrlist_append(regexp_ptrlist* l1, regexp_ptrlist* l2)
{
	regexp_ptrlist* oldl1;

	oldl1 = l1;
	while (l1->next)
	{
		l1 = l1->next;
	}
	l1->next = l2;
	return oldl1;
}

#define push(s) *stackp++ = (s)
#define pop() *--stackp

regexp_state* regexp_postfx2nfa(char* postfix)
{
	char* p = postfix;
	regexp_frag stack[1000], *stackp, e1, e2, e;
	regexp_state *s;

	if (postfix == NULL)
	{
		return NULL;
	}

	stackp = stack;
	for (; *p; p++)
	{
		switch (*p)
		{
		case REGEXP_CONCAT:
			e2 = pop();
			e1 = pop();
			ptrlist_patch(e1.out, e2.start);
			push(frag_make(e1.start, e2.out));
			break;

		case '|':
			e2 = pop();
			e1 = pop();
			s = state_make(Regexp_Split, e1.start, e2.start);
			push(frag_make(s, ptrlist_append(e1.out, e2.out)));
			break;

		case '?':
			e = pop();
			s = state_make(Regexp_Split, e.start, NULL);
			push(frag_make(s, ptrlist_append(e.out, list_make(&s->out_alter))));
			break;

		case '*':
			e = pop();
			s = state_make(Regexp_Split, e.start, NULL);
			ptrlist_patch(e.out, s);
			push(frag_make(s, list_make(&s->out_alter)));
			break;

		case '+':
			e = pop();
			s = state_make(Regexp_Split, e.start, NULL);
			ptrlist_patch(e.out, s);
			push(frag_make(e.start, list_make(&s->out_alter)));
			break;

		default:
			s = state_make(*p, NULL, NULL);
			push(frag_make(s, list_make(&s->out)));
			break;
		}
	}

	e = pop();
	if (stackp != stack)
	{
		return NULL;
	}

	ptrlist_patch(e.out, &match_state);
	return e.start;
}

#undef pop
#undef push

static void state_add(regexp_list*, regexp_state*);
static void list_step(regexp_list*, size_t, regexp_list*);

static regexp_list* list_start(regexp_state* start, regexp_list* l)
{
	l->cnt = 0;
	list_id++;
	state_add(l, start);
	return l;
}

static int ismatch(regexp_list *l)
{
	int i;

	for (i = 0; i < l->cnt; i++)
	{
		if (l->state[i] == &match_state)
		{
			return 1;
		}
	}
	return 0;
}

static void state_add(regexp_list* l, regexp_state* s)
{
	if (s == NULL || s->last_list == list_id)
	{
		return;
	}
	s->last_list = list_id;
	if (s->code == Regexp_Split)
	{
		state_add(l, s->out);
		state_add(l, s->out_alter);
		return;
	}
	l->state[l->cnt++] = s;
}

static void list_step(regexp_list* clist, size_t c, regexp_list* nlist)
{
	int i;
	regexp_state *s;

	list_id++;
	nlist->cnt = 0;
	for (i = 0; i<clist->cnt; i++)
	{
		s = clist->state[i];
		if (s->code == c)
		{
			state_add(nlist, s->out);
		}
	}
}

static int list_compare(regexp_list* l1, regexp_list* l2)
{
	int i;

	if (l1->cnt < l2->cnt)
	{
		return -1;
	}
	if (l1->cnt > l2->cnt)
	{
		return 1;
	}
	for (i = 0; i < l1->cnt; i++)
	{
		if (l1->state[i] < l2->state[i])
		{
			return -1;
		}
		else if (l1->state[i] > l2->state[i])
		{
			return 1;
		}
	}
	return 0;
}

static int ptr_compare(const void* a, const void* b)
{
	if (a < b)
	{
		return -1;
	}
	if (a > b)
	{
		return 1;
	}
	return 0;
}

static regexp_dstate* dstate_make(regexp_list* l)
{
	int i;
	regexp_dstate **dp, *d;

	qsort(l->state, l->cnt, sizeof(regexp_state*), ptr_compare);
	dp = &dstate_list;

	while ((d = *dp) != NULL)
	{
		i = list_compare(l, &d->ls);
		if (i < 0)
		{
			dp = &d->left;
		}
		else if (i > 0)
		{
			dp = &d->right;
		}
		else
		{
			return d;
		}
	}

	d = (regexp_dstate*)malloc(sizeof(regexp_dstate) + l->cnt * sizeof(regexp_state));
	memset(d, 0, sizeof(regexp_dstate));
	d->ls.state = (regexp_state**)(d + 1);
	memmove(d->ls.state, l->state, l->cnt * sizeof(regexp_state));
	d->ls.cnt = l->cnt;
	*dp = d;
	return d;
}

static void nfa_start(regexp_state* start, regexp_list* l)
{
	l->cnt = 0;
	list_id++;
	state_add(l, start);
}

regexp_dstate* dstate_start(regexp_state* start)
{
	return dstate_make(list_start(start, &l1));
}

static regexp_dstate* state_next(regexp_dstate* d, size_t c)
{
	list_step(&d->ls, c, &l1);
	return d->next[c] = dstate_make(&l1);
}

int regexp_match(regexp_dstate* start, char* s)
{
	regexp_dstate *d, *next;
	int c, i;

	d = start;
	for (; *s; s++)
	{
		c = *s & 0xFF;
		if ((next = d->next[c]) == NULL)
		{
			next = state_next(d, c);
		}
		d = next;
	}
	return ismatch(&d->ls);
}

void regexp_init(void)
{
	l1.state = (regexp_state**)malloc(state_cnt * sizeof(regexp_state*));
	l2.state = (regexp_state**)malloc(state_cnt * sizeof(regexp_state*));
}