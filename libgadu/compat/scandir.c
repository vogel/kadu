/*
 * (C) Copyright 2002 Wojtek Kaniewski <wojtekka@irc.pl>
 * Released under terms of GPL v2.
 */

#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>

int foo_alphasort(const struct dirent **a, const struct dirent **b)
{
	if (!a || !b || !*a || !*b || !(*a)->d_name || !(*b)->d_name)
		return 0;

	return strcmp((*a)->d_name, (*b)->d_name);
}

typedef int (*qsort_compar_t)(const void *a, const void *b);

int foo_scandir(const char *path, struct dirent ***namelist, int (*select)(const struct dirent *), int (*compar)(const struct dirent **, const struct dirent **))
{
	struct dirent **res, *tmp;
	int i, count = 0, my_errno = 0;
	DIR *dir;

	if (!(dir = opendir(path)))
		return -1;

	while ((tmp = readdir(dir)))
		if (!select || (*select)(tmp))
			count++;

	rewinddir(dir);

	if (!(res = malloc(count * sizeof(struct dirent*)))) {
		my_errno = ENOMEM;
		goto cleanup;
	}

	memset(res, 0, count * sizeof(struct dirent*));

	for (i = 0; i < count; i++) {
		if (!(tmp = readdir(dir))) {
			my_errno = errno;
			goto cleanup;
		}

		if (!(res[i] = malloc(sizeof(struct dirent)))) {
			my_errno = ENOMEM;
			goto cleanup;
		}

		memcpy(res[i], dir, sizeof(struct dirent));
	}

	closedir(dir);

	if (compar)
		qsort(res, count, sizeof(struct dirent*), (qsort_compar_t) compar);

	*namelist = res;

	return count;

cleanup:
	for (i = 0; res && res[i] && i < count; i++)
		free(res[i]);

	if (res)
		free(res);

	closedir(dir);

	errno = my_errno;

	return -1;
}
