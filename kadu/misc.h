#ifndef MISC_H
#define MISC_H

#include <qvaluelist.h>
#include "../libgadu/lib/libgadu.h"

//typedef QValueList<uin_t> UinsList;

class UinsList : public QValueList<uin_t>
{
	public:
		UinsList();
		bool equals(UinsList &uins);
};

char* preparePath(char* filename);

#endif
