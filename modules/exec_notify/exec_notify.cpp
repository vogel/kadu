/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qapplication.h>
#include <qlineedit.h>
#include <qmap.h>
#include <qprocess.h>
#include <qtooltip.h>

#include "config_file.h"
#include "debug.h"
#include "kadu.h"
#include "kadu_parser.h"
#include "misc.h"

#include "exec_notify.h"

extern "C" int exec_notify_init()
{
	kdebugf();
	exec_notify = new ExecNotify();
	kdebugf2();
	return 0;
}

extern "C" void exec_notify_close()
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
}

ExecConfigurationWidget::~ExecConfigurationWidget()
{
}

void ExecConfigurationWidget::saveNotifyConfigurations()
{
	if (currentNotifyEvent != "")
		Commands[currentNotifyEvent] = commandLineEdit->text();

	CONST_FOREACH(Command, Commands)
	{
		const QString &eventName = Command.key();
		config_file.writeEntry("Exec Notify", eventName + "Cmd", *Command);
	}
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
	import_0_5_0_configuration();

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
				switch (str[idx + 1])
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
	
	const UserListElements &senders = notification->userListElements();
	UserListElement ule;

	if (senders.count())
		ule = notification->userListElements()[0];

	FOREACH(it, s)
	{
		if ((*it).contains("%ids"))
		{
			QStringList sndrs;
			CONST_FOREACH(sndr, senders)
				sndrs.append((*sndr).ID("Gadu"));
			(*it).replace("%ids", sndrs.join(","));
		}
		(*it) = KaduParser::parse((*it), ule, notification);
	}

	run(s, QString::null);
}

void ExecNotify::run(const QStringList &args, const QString &stdin)
{
#ifdef DEBUG_ENABLED
	CONST_FOREACH(arg, args)
		kdebugm(KDEBUG_INFO, "arg: '%s'\n", (*arg).local8Bit().data());
	kdebugm(KDEBUG_INFO, "stdin: %s\n", stdin.local8Bit().data());
#endif
	QProcess *p = new QProcess(args);
	connect(p, SIGNAL(processExited()), p, SLOT(deleteLater()));
	p->launch(stdin.local8Bit());
}

void ExecNotify::import_0_5_0_ConfigurationFromTo(const QString &from, const QString &to)
{
	QString syntax;
	syntax = config_file.readEntry("Exec Notify", from + "Cmd", "");
	if (!syntax.isEmpty())
	{
		syntax.replace("%id", "%u");
		syntax.replace("%status", "%s");
		syntax.replace("%action", "#{event}");
		syntax.replace("%protocol", "#{protocol}");
		config_file.addVariable("Exec Notify", to + "Cmd", syntax);
	}
	config_file.addVariable("Notify", to + "_Exec", config_file.readBoolEntry("Notify", from + "_Exec"));
	config_file.removeVariable("Exec Notify", from + "_Exec");
}

void ExecNotify::import_0_5_0_configuration()
{
	import_0_5_0_ConfigurationFromTo("toAvailable", "StatusChanged/ToOnline");
	import_0_5_0_ConfigurationFromTo("toInvisible", "StatusChanged/ToInvisible");
	import_0_5_0_ConfigurationFromTo("toNotAvailableCmd", "StatusChanged/ToOffline");
	import_0_5_0_ConfigurationFromTo("toBusy", "StatusChanged/ToBusy");
	import_0_5_0_ConfigurationFromTo("ConnError", "ConnectionError");
}

NotifierConfigurationWidget *ExecNotify::createConfigurationWidget(QWidget *parent , char *name )
{
	return new ExecConfigurationWidget(parent, name);
}

ExecNotify *exec_notify = NULL;
