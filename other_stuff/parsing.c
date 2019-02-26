#include <stdio.h>
#include <stdlib.h>
#include "mpc.h"

// -------- If we are compiling on Windows compile these functions
#ifdef _WIN32
  #include <string.h>

  // Declare a buffer for user input of size 2048
  // static makes this global variable local only for this file!
  static char buffer[2048];
  
  // Fake readline function
  char* readline(char* prompt) {
  	  fputs(prompt, stdout);
  	  fgets(buffer, 2048, stdin);
  	  char* cpy = malloc(strlen(buffer)+1);
  	  strcpy(cpy, buffer);
  	  cpy[strlen(cpy)-1] = '\0';
  	  
  	  return cpy;
  }
  
  // Fake add_history function
  void add_history(char* unused) {}

// -------- Otherwise include the editline headers
#else
  #include <editline/readline.h>
  #include <editline/history.h>
#endif

int number_of_nodes(mpc_ast_t* t) {
	if (t->children_num == 0) { return 1; }
	if (t->children_num >= 1) {
		int total = 1;
		for(int i = 0; i < t->children_num; i++) {
			total = total + number_of_nodes(t->children[i]);
		}
		
		return total;
	}
	
	return 0;
}

int main(int argc, char** argv) {
	
	// PARSERS
    mpc_parser_t* Number   = mpc_new("number");
    mpc_parser_t* Operator = mpc_new("operator");
    mpc_parser_t* Expr     = mpc_new("expr");
    mpc_parser_t* Lispy    = mpc_new("lispy");
    
    // LANGUAGE GRAMAR DEFINITION
    mpca_lang(
    	MPCA_LANG_DEFAULT,
    	"                                                 \
    	number   : /-?[0-9]+/;                            \
    	operator : '+' | '-' | '*' | '/';                 \
    	expr     : <number> | '(' <operator> <expr>+ ')'; \
    	lispy    : /^/ <operator> <expr>+ /$/;            \
    	",
    	Number, Operator, Expr, Lispy
    );

	// Print version and exit information
	puts("Lispy Version 0.0.0.0.1");
	puts("Press [Ctrl+c] to Exit\n");
	
	while(1) {
		// Output our prompt
		char* input = readline("lispy> ");
		add_history(input);
		
		// Echo input back to user
		//printf("You used command:%s\n", input);
		
		// Attempt to parse user input
		mpc_result_t r;
		if(mpc_parse("<stdin>", input, Lispy, &r)) {
			// ON SUCCESS
			mpc_ast_print(r.output);
			mpc_ast_delete(r.output);
		} else {
			// print error
			mpc_err_print(r.error);
			mpc_err_delete(r.error);
		}
		
		free(input);
	}
	
	// Undefine and delete parsers
    mpc_cleanup(4, Number, Operator, Expr, Lispy);

	return 0;
}