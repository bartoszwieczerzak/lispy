#include "mpc.h"
#include <math.h>

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

/* Use operator string to see which operation to perform */
long eval_op(long x, char* op, long y) {
  if (strcmp(op, "+") == 0) { return x + y; }
  if (strcmp(op, "-") == 0) { return x - y; }
  if (strcmp(op, "*") == 0) { return x * y; }
  if (strcmp(op, "/") == 0) { return x / y; }
  if (strcmp(op, "%") == 0) { return x % y; }
  if (strcmp(op, "^") == 0) { return pow(x,y); }
  if (strcmp(op, "m") == 0) { return fmin(x,y); }
  if (strcmp(op, "x") == 0) { return fmax(x,y); }
  return 0;
}

long eval(mpc_ast_t* t) {
  
  /* If tagged as number return it directly. */ 
  if (strstr(t->tag, "number")) {
    return atoi(t->contents);
  }
  
  /* The operator is always second child. */
  char* op = t->children[1]->contents;
  
  /* We store the third child in `x` */
  long x = eval(t->children[2]);
  
  /* Iterate the remaining children and combining. */
  int i = 3;
  while (strstr(t->children[i]->tag, "expr")) {
    x = eval_op(x, op, eval(t->children[i]));
    i++;
  }
  
  return x;  
}

int main(int argc, char** argv) {
  
  mpc_parser_t* Number = mpc_new("number");
  mpc_parser_t* Operator = mpc_new("operator");
  mpc_parser_t* Expr = mpc_new("expr");
  mpc_parser_t* Lispy = mpc_new("lispy");
  
  mpca_lang(MPCA_LANG_DEFAULT,
    "                                                               \
      number   : /-?[0-9]+/ ;                                       \
      operator : '+' | '-' | '*' | '/' | '%' | '^' | 'm' | 'x';     \
      expr     : <number> | '(' <operator> <expr>+ ')' ;            \
      lispy    : /^/ <operator> <expr>+ /$/ ;                       \
    ",
    Number, Operator, Expr, Lispy);
  
  puts("Lispy Version 0.0.0.0.3");
  puts("Press Ctrl+c to Exit\n");
  
  while (1) {
  
    char* input = readline("lispy> ");
    add_history(input);
    
    mpc_result_t r;
    if (mpc_parse("<stdin>", input, Lispy, &r)) {
      
      long result = eval(r.output);
      printf("%li\n", result);
      mpc_ast_delete(r.output);
      
    } else {    
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }
    
    free(input);
    
  }
  
  mpc_cleanup(4, Number, Operator, Expr, Lispy);
  
  return 0;
}