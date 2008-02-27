#ifndef KADU_UPDATES_H
#define KADU_UPDATES_H

#include <qglobal.h>

#include <qstring.h>
#include <qdatetime.h>
#include <qobject.h>

#include "gadu.h"

class Q3UrlOperator;
class Q3NetworkOperation;

class Updates : public QObject
{
	Q_OBJECT
	public:
		static void initModule();
		static void closeModule();

	private slots:
		void gotUpdatesInfo(const QByteArray &data, Q3NetworkOperation *op);
		void run();

	private:
		static bool UpdateChecked;
		static Updates *instance;
		static QDateTime LastUpdateCheck;
		
		QString query;
		Q3UrlOperator *op;

		Updates(UinType uin);
		virtual ~Updates();
		static bool ifNewerVersion(const QString &newestversion);
};

#endif
