#ifndef MISC_H
#define MISC_H

#include <qvaluelist.h>
#include "../libgadu/lib/libgadu.h"

#define __c2q(__char_pointer__) QString::fromLocal8Bit(__char_pointer__)

class UinsList : public QValueList<uin_t>
{
	public:
		UinsList();
		bool equals(UinsList &uins);
		void sort();
};

char* preparePath(char* filename);

#endif
