
#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
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

UinsList::UinsList() {
}

bool UinsList::equals(UinsList &uins) {
	if (count() != uins.count())
		return false;
	for (UinsList::iterator i = begin(); i != end(); i++)
		if(!uins.contains(*i))
			return false;
	return true;
}

void UinsList::sort() {
	bool stop;
	int i;
	uin_t uin;
	
	if (count() < 2)
		return;

	do {
		stop = true;
		for (i = 0; i < count() - 1; i++)
			if (this->operator[](i) > this->operator[](i+1)) {
				uin = this->operator[](i);
				this->operator[](i) = this->operator[](i+1);
				this->operator[](i+1) = uin;
				stop = false;
				}
	} while (!stop);	
}
