#ifndef UPDATES_H
#define UPDATES_H

#include <qurloperator.h>
#include <qstring.h>

#include "gadu.h"

class Updates : public QObject
{
	Q_OBJECT
	public:
		static void initModule();
		static void deactivateModule();

	private slots:
		void gotUpdatesInfo(const QByteArray &data, QNetworkOperation *op);
		void run();

	private:
		static bool UpdateChecked;
		static Updates *instance;
		
		QString query;
		QUrlOperator *op;

		Updates(UinType uin);
		virtual ~Updates();
		static bool ifNewerVersion(const QString &newestversion);
};

#endif
