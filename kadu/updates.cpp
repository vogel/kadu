#include <qnetwork.h>
#include <qurloperator.h>
#include <qnetworkprotocol.h>
#include <qcstring.h>

#include "../config.h"
#include "updates.h"
#include "misc.h"
#include "kadu.h"

UpdatesClass::UpdatesClass(uin_t uin) {
	qInitNetworkProtocols();
	query = QString("update.php?uin=%1&version=%2").arg(uin).arg(QString(VERSION));
	op = new QUrlOperator("http://www.kadu.net");
}

void UpdatesClass::run() {
	op->get(query);

}

bool UpdatesClass::ifNewerVersion(QString &newestversion) {
	return (newestversion != QString(VERSION));
}
