
#include <qthread.h>
#include <qnetwork.h>
#include <qurloperator.h>
#include <qnetworkprotocol.h>
#include <qcstring.h>
#include "../config.h"
#include "updates.h"
#include "misc.h"
#include "kadu.h"

UpdatesThread::UpdatesThread(uin_t uin) : QThread() {
	qInitNetworkProtocols();
	query = QString("update.php?uin=%1&version=%2").arg(uin).arg(QString(VERSION));
	op = new QUrlOperator("http://www.kadu.net");
}

void UpdatesThread::run() {
	op->get(query);

}

bool UpdatesThread::ifNewerVersion(QString &newestversion) {
	return (newestversion != QString(VERSION));
}
