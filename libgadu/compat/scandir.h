/*
 * (C) Copyright 2002 Wojtek Kaniewski <wojtekka@irc.pl>
 * Released under terms of GPL v2.
 */

#ifndef __COMPAT_SCANDIR_H
#define __COMPAT_SCANDIR_H

#include <dirent.h>

int alphasort(const struct dirent **a, const struct dirent **b);
int scandir(const char *dir, struct dirent ***namelist, int (*select)(const struct dirent *), int (*compar)(const struct dirent **, const struct dirent **));

#endif
