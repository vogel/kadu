#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libgadu.h"

int main(int argc, char **argv)
{
	char buf[100];
	int i;

	if (argc < 2 || argc > 10) {
		fprintf(stderr, "u¿ycie: %s <kolejne> [wyrazy] [do] [hasha]\n", argv[0]);
		return 1;
	}

	strcpy(buf, "");
	
	for (i = 1; i < argc; i++)
		strcat(buf, "s");

	printf("%s\n", buf);
	printf("%u\n", gg_http_hash("ss", "wojtekka@irc.pl", "dupa123"));
	
	printf("%u\n", gg_http_hash(buf, argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7], argv[8], argv[9], argv[10]));

	return 0;
}

