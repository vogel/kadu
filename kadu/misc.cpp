
#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
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

void cp_to_iso(unsigned char *buf)
{
	while (*buf)
	{
		if (*buf == (unsigned char)'¥') *buf = '¡';
		if (*buf == (unsigned char)'¹') *buf = '±';
		if (*buf == 140) *buf = '¦';
		if (*buf == 156) *buf = '¶';
		if (*buf == 143) *buf = '¬';
		if (*buf == 159) *buf = '¼';
        	buf++;
        };
};

void iso_to_cp(unsigned char *buf)
{
	while (*buf)
	{
		if (*buf == (unsigned char)'¡') *buf = '¥';
		if (*buf == (unsigned char)'±') *buf = '¹';
		if (*buf == (unsigned char)'¦') *buf = 140;
		if (*buf == (unsigned char)'¶') *buf = 156;
		if (*buf == (unsigned char)'¬') *buf = 143;
		if (*buf == (unsigned char)'¼') *buf = 159;
		buf++;
	};
};

char *timestamp(time_t customtime)
{
	static char buf[100];

	time_t t;
	struct tm *tm;

	time(&t);

	tm = localtime(&t);
	strftime(buf, sizeof(buf), ":: %d %m %Y, (%T", tm);

	if (customtime) {
		char buf2[20];
		struct tm *tm2;
		tm2 = localtime(&customtime);
		strftime(buf2, sizeof(buf2), " / S %T)", tm2);
		strncat(buf, buf2, sizeof(buf2));
		
/*		int j = 0;
		while(buf[j++] != "\0");
		
		int i = -1;
		while(buf2[++i] != "\0") {
       buf[j+i] = buf2[i];
			}
		buf[j + ++i] = "\0"; */
	
		return buf;

		}

	strftime(buf, sizeof(buf), ":: %d %m %Y, (%T)", tm);

	return buf;
}

char *pwHash(const char *tekst) {
    char *nowytekst;
    nowytekst = strdup(tekst);
    int ile, znak;
    for (ile = 0; ile < strlen(tekst); ile++) {
	znak = nowytekst[ile]^ile^1;
	nowytekst[ile] = znak;
	}
    return nowytekst;
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
