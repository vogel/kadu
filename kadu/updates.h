
#ifndef UPDATES_H
#define UPDATES_H

#include <qurloperator.h>
#include <qstring.h>

#include "gadu.h"

class UpdatesClass
{

	public:
		UpdatesClass(UinType uin);
		bool ifNewerVersion(QString &newestversion);
		void run();

		QUrlOperator *op;

	private:
		QString query;
};

#endif
