
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
//	Q_OBJECT

	public:
		UpdatesThread(uin_t uin, QString version);

		QString newversion;
		QUrlOperator *op;

	protected:
		void run();

	private:
		uin_t uin;
		QString actversion;

	public slots:
//		void data(const QByteArray &data, QNetworkOperation *op);
};

#endif
