
#include <qthread.h>
#include <qnetwork.h>
#include <qurloperator.h>
#include <qnetworkprotocol.h>
#include <qcstring.h>
#include "updates.h"
#include "misc.h"

UpdatesThread::UpdatesThread(uin_t uin, QString version) : QThread(), actversion(version), uin(uin) {
}

void UpdatesThread::run() {
	QString query;

	qInitNetworkProtocols();
	
	query = QString("stats.php?uin=%1&version=%2").arg(uin).arg(actversion);
	op = new QUrlOperator("http://www.kadu.net");
//	connect(op, SIGNAL(data(const QByteArray &, QNetworkOperation *)),
//		this, SLOT(gotData(const QByteArray &, QNetworkOperation *)));
	op->get(query);

}

//void UpdatesThread::data(const QByteArray &data, QNetworkOperation *op) {
//	newversion = QString(data);
//}

#include "updates.moc"
