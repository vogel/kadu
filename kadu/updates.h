
#ifndef UPDATES_H
#define UPDATES_H

#include <qthread.h>
#include <qnetwork.h>
#include <qurloperator.h>
#include <qnetworkprotocol.h>
#include <qcstring.h>
#include "misc.h"

class UpdatesThread : public QThread
{

	public:
		UpdatesThread(uin_t uin, QString &actversion);
		bool ifNewerVersion(QString &newestversion);

		QUrlOperator *op;

	protected:
		void run();

	private:
		QString query;
		QString actversion;
};

#endif
