#include <stdio.h>
#include "mpc.h"

static char input[2048];

/* lval types - result or error */
enum {LVAL_NUM, LVAL_ERR};

/* lval error types */
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM};

/* lval - Lisp Value - eval return type */
typedef struct {
	int type;
	long num;
	int err;
} lval;

/* Create lval result */
lval lval_num(long x) {
	lval v;
	v.type = LVAL_NUM;
	v.num = x;
	return v;
}

/* Create lval error */
lval lval_err(long x) {
	lval v;
	v.type = LVAL_ERR;
	v.num = x;
	return v;
}

/* Print lval */
void lval_print(lval v) {
	switch (v.type) {
		case LVAL_NUM: printf("%li", v.num); break;
		
		case LVAL_ERR:
			if (v.err == LERR_DIV_ZERO) {
				printf("Error: Division By Zero!");
			}
			if (v.err == LERR_BAD_OP) {
				printf("Error: Invalid operator!");
			}
			if (v.err == LERR_BAD_NUM) {
				printf("Error: Invalid number!");
			}
			break;
	}
}

void lval_println(lval v) { lval_print(v); putchar('\n');}


// ------------------------------------------------------------
void print_welcome_message() {
	puts("Lispy v 0.0.0.3");
	puts("Press CTRL + C to exit");
}

int number_of_nodes(mpc_ast_t* t) {
	if(t->children_num == 0) {return 1;}
	
	if (t->children_num >= 1) {
		int total = 1;
		for(int i = 0; i < t->children_num; i++) {
			total += number_of_nodes(t->children[i]);
		}
		return total;
	}
	return 0;
}

int number_of_leaves(mpc_ast_t* t) {
	if(t->children_num == 0) {return 1;}

	if (t->children_num >= 1) {
		int total = 0;
		for(int i = 0; i < t->children_num; i++) {
			total += number_of_leaves(t->children[i]);
		}
		return total;
	}	
	return 0;
}

// FIXME: this function is broken - need to be fixed
int number_of_branches(mpc_ast_t* t) {
	if(t->children_num == 0) {return 0;}

	if (t->children_num >= 1) {
		int total = 0;
		for(int i = 0; i < t->children_num; i++) {
			total += number_of_branches(t->children[i]);
			if(t->tag == NULL) {
				total++;
			}
		}
		return total;
	}	
	return 0;
}

lval eval_op(lval x, char* op, lval y) {
	// return if error
	if(x.type == LVAL_ERR) { return x; }
	if(y.type == LVAL_ERR) { return y; }
	
	// return evaluated value
	if (strcmp(op, "+") == 0) {return lval_num(x.num + y.num);}
	if (strcmp(op, "-") == 0) {return lval_num(x.num - y.num);}
	if (strcmp(op, "*") == 0) {return lval_num(x.num * y.num);}
	if (strcmp(op, "/") == 0) {	
		return y.num == 0
			? lval_err(LERR_DIV_ZERO)
			: lval_num(x.num / y.num);
	}
	
	return lval_err(LERR_BAD_OP);
}

lval eval(mpc_ast_t* t) {
	/* If tagged as number - return it directly */
	if(strstr(t->tag, "number")) {
		errno = 0;
		long x = strtol(t->contents, NULL, 10);
		return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);
	}
	
	/* Operator is always second child */
	char* op = t->children[1]->contents;
	
	/* Store third child in 'x' */
	lval x = eval(t->children[2]);
	
	/* Iterate remining children and combining */
	int i = 3;
	
	while(strstr(t->children[i]->tag, "expr")) {
		x = eval_op(x, op, eval(t->children[i]));
		i++;
	}
	
	return x;
}

int main (int argc, char** argv) {
	/* Define parser types */
	mpc_parser_t* Number = mpc_new("number");
	mpc_parser_t* Operator = mpc_new("operator");
	mpc_parser_t* Expr = mpc_new("expr");
	mpc_parser_t* Lispy = mpc_new("lispy");

	/* Define parsers with language definition */
	mpca_lang(MPCA_LANG_DEFAULT,
	  "                                                  \
	   number   : /-?[0-9]+/;                            \
	   operator : '+' | '-' | '*' | '/';                 \
	   expr     : <number> | '(' <operator> <expr>+ ')'; \
	   lispy    : /^/ <operator> <expr>+ /$/;            \
	  ",
	  Number, Operator, Expr, Lispy);


	print_welcome_message();
	
	while(1) {
		fputs("lispy> ", stdout);
		
		fgets(input, 2048, stdin);
			
		//printf("echo: %s", input);
		
		/* Parse user input with language defined earlier */
		mpc_result_t r;
		
		if(mpc_parse("<stdin>", input, Lispy, &r)) {
			/* Parsing was succesfull */
			/*mpc_ast_t* a = r.output;
			printf("Tag: %s\n", a->tag);
			printf("Contents: %s\n", a->contents);
			printf("Number of children: %i\n", a->children_num);
			
			mpc_ast_t* c0 = a->children[0];
			printf("First Child Tag: %s\n", c0->tag);
			printf("First Child Contents: %s\n", c0->contents);
			printf("First Child Number of children: %i\n", c0->children_num);*/
  
            //printf("Total childrens: %i\n", number_of_nodes(r.output));
			//printf("Total leaves: %i\n", number_of_leaves(r.output));
			//printf("Total branches: %i\n", number_of_branches(r.output));
  
			//mpc_ast_print(r.output);
			
			lval result = eval(r.output);
			lval_println(result);
			
			mpc_ast_delete(r.output);
			
		} else {
			// Parsing error			
			mpc_err_print(r.error);
			mpc_err_delete(r.error);
		}
	}
	
	mpc_cleanup(4, Number, Operator, Expr, Lispy);
	
	return 0;
}