/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QApplication>
#include <QtGui/QLineEdit>
#include <QtCore/QMap>
#include <QtCore/QProcess>
#include <QToolTip>
#include <QtGui/QLabel>
#include <QtGui/QGridLayout>

#include "config_file.h"
#include "debug.h"
#include "kadu.h"
#include "kadu_parser.h"
#include "misc.h"

#include "exec_notify.h"

extern "C" KADU_EXPORT int exec_notify_init(bool firstLoad)
{
	kdebugf();
	exec_notify = new ExecNotify();
	kdebugf2();
	return 0;
}

extern "C" KADU_EXPORT void exec_notify_close()
{
	kdebugf();
	delete exec_notify;
	exec_notify = 0;
	kdebugf2();
}

ExecConfigurationWidget::ExecConfigurationWidget(QWidget *parent, char *name)
	: NotifierConfigurationWidget(parent, name), currentNotifyEvent("")
{
	commandLineEdit = new QLineEdit(this);
	QToolTip::add(commandLineEdit, qApp->translate("@default", Kadu::SyntaxTextNotify));

	QGridLayout *gridLayout = new QGridLayout(this, 0, 0, 0, 3);
	gridLayout->addWidget(new QLabel(tr("Command") + ":", this), 0, 0, Qt::AlignRight);
	gridLayout->addWidget(commandLineEdit, 0, 1);

	parent->layout()->addWidget(this);
}

ExecConfigurationWidget::~ExecConfigurationWidget()
{
}

void ExecConfigurationWidget::saveNotifyConfigurations()
{
	if (currentNotifyEvent != "")
		Commands[currentNotifyEvent] = commandLineEdit->text();

	foreach(const QString &eventName, Commands.keys())
		config_file.writeEntry("Exec Notify", eventName + "Cmd", Commands[eventName]);
}

void ExecConfigurationWidget::switchToEvent(const QString &event)
{
	if (currentNotifyEvent != "")
		Commands[currentNotifyEvent] = commandLineEdit->text();
	currentNotifyEvent = event;

	if (Commands.contains(event))
		commandLineEdit->setText(Commands[event]);
	else
		commandLineEdit->setText(config_file.readEntry("Exec Notify", event + "Cmd"));
}


ExecNotify::ExecNotify(QObject *parent, const char *name) : Notifier(parent, name)
{
	kdebugf();

	config_file.addVariable("Exec Notify", "NewChatCmd", "Xdialog --msgbox \"#{protocol} %u %ids #{event}\" 10 100");
	config_file.addVariable("Exec Notify", "NewMessageCmd", "Xdialog --msgbox \"#{protocol} %u %ids #{event}\" 10 100");
	config_file.addVariable("Exec Notify", "ConnectionErrorCmd", "Xdialog --msgbox \"#{protocol} #{event}\" 10 100");
	config_file.addVariable("Exec Notify", "StatusChanged/ToOnlineCmd", "Xdialog --msgbox \"%protocol %u #{event}\" 10 100");
	config_file.addVariable("Exec Notify", "StatusChanged/ToBusyCmd", "Xdialog --msgbox \"#{protocol} %u #{event}\" 10 100");
	config_file.addVariable("Exec Notify", "StatusChanged/ToInvisibleCmd", "Xdialog --msgbox \"#{protocol} %u #{event}\" 10 100");
	config_file.addVariable("Exec Notify", "StatusChanged/ToOfflineCmd", "Xdialog --msgbox \"#{protocol} %u #{event}\" 10 100");

	notification_manager->registerNotifier(QT_TRANSLATE_NOOP("@default", "Exec"), this);

	kdebugf2();
}

ExecNotify::~ExecNotify()
{
	kdebugf();

	notification_manager->unregisterNotifier("Exec");

	kdebugf2();
}

// TODO: merge with HistoryManager version
QStringList mySplit(const QChar &sep, const QString &str)
{
	kdebugf();
	QStringList strlist;
	QString token;
	unsigned int idx = 0, strlength = str.length();
	bool inString = false;

	int pos1, pos2;
	while (idx < strlength)
	{
		const QChar &letter = str[idx];
		if (inString)
		{
			if (letter == '\\')
			{
				switch (str[idx + 1].digitValue())
				{
					case 'n':
						token.append('\n');
						break;
					case '\\':
						token.append('\\');
						break;
					case '\"':
						token.append('"');
						break;
					default:
						token.append('?');
				}
				idx += 2;
			}
			else if (letter == '"')
			{
				strlist.append(token);
				inString = false;
				++idx;
			}
			else
			{
				pos1 = str.find('\\', idx);
				if (pos1 == -1)
					pos1 = strlength;
				pos2 = str.find('"', idx);
				if (pos2 == -1)
					pos2 = strlength;
				if (pos1 < pos2)
				{
					token.append(str.mid(idx, pos1 - idx));
					idx = pos1;
				}
				else
				{
					token.append(str.mid(idx, pos2 - idx));
					idx = pos2;
				}
			}
		}
		else // out of the string
		{
			if (letter == sep)
			{
				if (!token.isEmpty())
					token = QString::null;
				else
					strlist.append(QString::null);
			}
			else if (letter == '"')
				inString = true;
			else
			{
				pos1 = str.find(sep, idx);
				if (pos1 == -1)
					pos1 = strlength;
				token.append(str.mid(idx, pos1 - idx));
				strlist.append(token);
				idx = pos1;
				continue;
			}
			++idx;
		}
	}

	kdebugf2();
	return strlist;
}

void ExecNotify::notify(Notification *notification)
{
	QString syntax = config_file.readEntry("Exec Notify", notification->type() + "Cmd");
	if (syntax.isEmpty())
		return;
	QStringList s = mySplit(' ', syntax);
	QStringList result;
	
	const UserListElements &senders = notification->userListElements();
	UserListElement ule;

	if (senders.count())
		ule = notification->userListElements()[0];

	QStringList sendersList;
	foreach(const UserListElement &sender, senders)
		sendersList.append(sender.ID("Gadu"));
	QString sendersString = sendersList.join(",");

	foreach(QString it, s)
		result.append(KaduParser::parse(it.replace("%ids", sendersString), ule, notification));

	run(result, QString::null);
}

void ExecNotify::run(const QStringList &args, const QString &in)
{
#ifdef DEBUG_ENABLED
	foreach(QString arg, args)
		kdebugm(KDEBUG_INFO, "arg: '%s'\n", qPrintable(arg));
	kdebugm(KDEBUG_INFO, "stdin: %s\n", qPrintable(in));
#endif
	QProcess *p = new QProcess();
	QString cmd = args.first();
	QStringList arguments = args;
	arguments.removeAt(0);
	connect(p, SIGNAL(finished(int, QProcess::ExitStatus)), p, SLOT(deleteLater()));
	p->start(cmd, arguments);
	//p->launch(stdin.local8Bit());
}

NotifierConfigurationWidget *ExecNotify::createConfigurationWidget(QWidget *parent , char *name )
{
	return new ExecConfigurationWidget(parent, name);
}

ExecNotify *exec_notify = NULL;
