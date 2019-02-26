#include <stdio.h>
#include <stdlib.h>

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

int main(int argc, char** argv) {
	
	// Print version and exit information
	puts("Lispy Version 0.0.0.0.1");
	puts("Press [Ctrl+c] to Exit\n");
	
	while(1) {
		// Output our prompt
		char* input = readline("lispy> ");
		add_history(input);
		
		// Echo input back to user
		printf("You used command:%s\n", input);
		free(input);
	}
	
	return 0;
}