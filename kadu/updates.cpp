#include <qnetwork.h>
#include <qmessagebox.h>
#include "kadu-config.h"
#include "updates.h"
#include "config_file.h"
#include "debug.h"
#include "kadu.h"
#include "gadu.h"

Updates *Updates::instance = NULL;
bool Updates::UpdateChecked = false;
QDateTime Updates::LastUpdateCheck;

Updates::Updates(UinType uin)
{
	kdebugf();
	qInitNetworkProtocols();
	query = QString("update.php?uin=%1&version=%2").arg(uin).arg(QString(VERSION));
	op = new QUrlOperator("http://www.kadu.net");
	kdebugf2();
}

Updates::~Updates()
{
	kdebugf();
	delete op;
	instance=NULL;
}

void Updates::run()
{
	kdebugf();
	op->get(query);
	kdebugf2();
}

bool Updates::ifNewerVersion(const QString &newestversion)
{
	return (newestversion != QString(VERSION));
}

void Updates::initModule()
{
	kdebugf();

	QDateTime actualtime = QDateTime::currentDateTime();
	LastUpdateCheck.setTime_t(config_file.readNumEntry("General", "LastUpdateCheck"));

	if (!UpdateChecked && LastUpdateCheck.secsTo(actualtime) >= 3600)
	{
		UinType myUin=(UinType)config_file.readNumEntry("General", "UIN");
		if (myUin)
		{
			instance = new Updates(myUin);
			connect(instance->op, SIGNAL(data(const QByteArray &, QNetworkOperation *)),
					instance, SLOT(gotUpdatesInfo(const QByteArray &, QNetworkOperation *)));
			connect(gadu, SIGNAL(connected()), instance, SLOT(run()));
		}
	}
	kdebugf2();
}

void Updates::deactivateModule()
{
	kdebugf();
	if (instance)
		delete instance;
	kdebugf2();
}

void Updates::gotUpdatesInfo(const QByteArray &data, QNetworkOperation * /*op*/)
{
	kdebugf();
	if (config_file.readBoolEntry("General", "CheckUpdates"))
	{
		if (data.size() > 31)
		{
			kdebugmf(KDEBUG_WARNING, "cannot obtain update info\n");
			disconnect(gadu, SIGNAL(connected()), this, SLOT(run()));
			deleteLater();
			kdebugf2();
			return;
		}
		
		char buf[32];
		for (unsigned i = 0; i < data.size(); ++i)
			buf[i] = data[i];
		buf[data.size()] = 0;
		QString newestversion = buf;

		kdebugmf(KDEBUG_INFO, "%s\n", buf);

		if (ifNewerVersion(newestversion))
			QMessageBox::information(kadu, tr("Update information"),
				tr("The newest Kadu version is %1").arg(newestversion), QMessageBox::Ok);
	}
	disconnect(gadu, SIGNAL(connected()), this, SLOT(run()));
	UpdateChecked = true;
	config_file.writeEntry("General", "LastUpdateCheck", QDateTime(QDate(1970, 1, 1)).secsTo(QDateTime::currentDateTime()));
	deleteLater();
	kdebugf2();
}
