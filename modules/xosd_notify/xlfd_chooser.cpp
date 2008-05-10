/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "xlfd_chooser.h"
#include "debug.h"
#include "misc.h"

#define USE_GTKFONTDIALOG

void XLFDChooser::processExited()
{
	kdebugf();

	FOREACH(it, queries)
	{
		QProcess *proc = (*it).proc;
		if (proc->state() != QProcess::Running)
		{
			char line[80];
			proc->readLine(line, sizeof(line));
			kdebugm(KDEBUG_INFO, "font: '%s'\n", line);

			connect(this, SIGNAL(fontSelected(const QString &)), (*it).receiver, (*it).slot);
			emit fontSelected(line);
			disconnect(this, SIGNAL(fontSelected(const QString &)), (*it).receiver, (*it).slot);

			queries.remove(it);
			delete proc;
			kdebugf2();
			return;
		}
	}

	kdebugmf(KDEBUG_FUNCTION_END|KDEBUG_WARNING, "can't find process\n");
}

XLFDChooser::SearchPosition::SearchPosition() : proc(0), initial(), pattern(), receiver(0), slot(0)
{
}

XLFDChooser::XLFDChooser(QObject *parent, const char *name) : QObject(parent, name), queries()
{
//	kdebugf();
//	kdebugf2();
}

XLFDChooser::~XLFDChooser()
{
	kdebugf();
	while (!queries.empty())
	{
		QProcess *proc = queries.first().proc;
		disconnect(proc, SIGNAL(processExited()), this, SLOT(processExited()));
		delete proc;
		queries.pop_front();
	}
	kdebugf2();
}

void XLFDChooser::getFont(QObject *receiver, char *slot, const QString &initial, const QString &pattern)
{
	kdebugf();
	SearchPosition pos;
	pos.receiver = receiver;
	pos.slot = slot;
	pos.pattern = pattern;
	pos.initial = initial;
	QString cmd;
	QStringList args;
#ifdef USE_GTKFONTDIALOG
	cmd = libPath("kadu/modules/bin/xosd_notify/gtkfontdialog");
	args = toStringList(initial, pattern);
#else
	cmd = "xfontsel";
	if (pattern.isEmpty())
		args = toStringList("-print");
	else
		args = toStringList("-print", "-pattern", pattern);
#endif
	pos.proc = new QProcess();
	queries.append(pos);
	connect(pos.proc, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processExited()));
	pos.proc->start(cmd, args);
	kdebugf2();
}

