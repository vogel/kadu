
#ifndef UPDATES_H
#define UPDATES_H

#include <qurloperator.h>
#include <qstring.h>

#include "libgadu.h"

class UpdatesClass
{

	public:
		UpdatesClass(uin_t uin);
		bool ifNewerVersion(QString &newestversion);
		void run();

		QUrlOperator *op;

	private:
		QString query;
};

#endif
