#include <stdio.h>
#include <string.h>

int main(int argc, char** argv) {
	char str[80];
	strcpy(str, "Hello world program");
	strcat(str, " written by: ");
	strcat(str, argv[1]);
	puts(str);
	
	return 0;
}