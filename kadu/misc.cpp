
#include <stdlib.h>
#include <sys/stat.h>
#include "misc.h"

char* preparePath(char* filename)
{
	static char path[1023];
	char* home;
	struct passwd* pw;
	if (pw=getpwuid(getuid()))
		home=pw->pw_dir;
	else
		home=getenv("HOME");
	snprintf(path, sizeof(path), "%s/.gg/%s", home, filename);
	return path;
};
