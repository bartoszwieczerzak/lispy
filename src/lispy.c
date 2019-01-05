#include <stdio.h>

static char input[2048];

int main(int argc, char** argv) {
	puts("Lispy v.0.0.1");
	puts("Press ctrl+c to exit\n");
	
	while(1) {
		fputs("lispy> ", stdout);
		
		fgets(input, 2048, stdin);
		
		printf("You entered %s", input);
	}
	
	return 0;
}