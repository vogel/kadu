
#ifndef UPDATES_H
#define UPDATES_H

#include <qnetwork.h>
#include <qurloperator.h>
#include <qnetworkprotocol.h>
#include <qcstring.h>

#include "misc.h"

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
