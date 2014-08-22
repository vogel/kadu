/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009, 2010, 2010, 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2004 Tomasz Jarzynka (tomee@cpi.pl)
 * Copyright 2004, 2005, 2007, 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2002, 2003, 2004, 2005, 2006 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2003, 2004, 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2002, 2003, 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007, 2008, 2009 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2003 Dariusz Jagodzik (mast3r@kadu.net)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtCore/QFile>
#include <QtCore/QProcess>
#include <QtCore/QRegExp>
#include <QtCore/QUrl>
#include <QtGui/QDesktopServices>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDesktopWidget>

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "buddies/buddy-list.h"
#include "buddies/buddy-set.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "contacts/contact-manager.h"
#include "gui/widgets/chat-widget/chat-widget-manager.h"
#include "gui/windows/message-dialog.h"
#include "debug.h"

#include "misc.h"

QString replacedNewLine(const QString &text, const QString &newLineText)
{
	static const QRegExp newLineRegExp("(\r\n|\r|\n)");
	return QString(text).replace(newLineRegExp, newLineText);
}

QRect properGeometry(const QRect &rect)
{
	QRect geometry(rect.normalized());
	QRect availableGeometry = QApplication::desktop()->availableGeometry(geometry.center());

	// correct size
	if (geometry.width() > availableGeometry.width())
		geometry.setWidth(availableGeometry.width());
	if (geometry.height() > availableGeometry.height())
		geometry.setHeight(availableGeometry.height());

	// switch screen
	if (geometry.center().x() < availableGeometry.x())
		geometry.moveLeft(availableGeometry.x());
	else if (geometry.center().x() >= availableGeometry.x() + availableGeometry.width())
		geometry.moveLeft(availableGeometry.x() + availableGeometry.width() - geometry.width());
	if (geometry.center().y() < availableGeometry.y())
		geometry.moveTop(availableGeometry.y());
	else if (geometry.center().y() >= availableGeometry.y() + availableGeometry.height())
		geometry.moveTop(availableGeometry.y() + availableGeometry.height() - geometry.height());

	// move
	if (geometry.bottomRight().x() >= availableGeometry.x() + availableGeometry.width())
		geometry.moveLeft(availableGeometry.x() + availableGeometry.width() - geometry.width());
	if (geometry.bottomRight().y() >= availableGeometry.y() + availableGeometry.height())
		geometry.moveTop(availableGeometry.y() + availableGeometry.height() - geometry.height());
	if (geometry.topLeft().x() < availableGeometry.x())
		geometry.moveLeft(availableGeometry.x());
	if (geometry.topLeft().y() < availableGeometry.y())
		geometry.moveTop(availableGeometry.y());

#ifdef Q_OS_MAC
	// looks like QDesktopWidget::availableGeometry() does not work correctly on Mac OS X, so we need a workaround
	if (availableGeometry.top() == 0)
	{
		if (geometry.y() < 20)
			geometry.moveTop(20);
		if (geometry.height() > availableGeometry.height() - 20)
			geometry.setHeight(availableGeometry.height() - 20);
	}
#endif

	// done
	return geometry;
}

QString pwHash(const QString &text)
{
	QString newText = text;
	for (int i = 0, textLength = text.length(); i < textLength; ++i)
		newText[i] = QChar(text.at(i).unicode() ^ static_cast<uint>(i) ^ 1);
	return newText;
}

QList<int> stringToIntList(const QString &in)
{
	QList<int> out;
	foreach (const QString &it, in.split(';', QString::SkipEmptyParts))
		out.append(it.toInt());
	return out;
}

QString intListToString(const QList<int> &in)
{
	QStringList out;
	foreach (const int &it, in)
		out.append(QString::number(it));
	return out.join(";");
}

QRect stringToRect(const QString &value, const QRect *def)
{
	QStringList stringlist;
	QRect rect = def ? *def : QRect(0, 0, 0, 0);
	int l, t, w, h;
	bool ok;

	stringlist = value.split(',', QString::SkipEmptyParts);
	if (stringlist.count() != 4)
		return rect;
	l = stringlist.at(0).toInt(&ok);
	if (!ok)
		return rect;
	t = stringlist.at(1).toInt(&ok);
	if (!ok)
		return rect;
	w = stringlist.at(2).toInt(&ok);
	if (!ok)
		return rect;
	h = stringlist.at(3).toInt(&ok);
	if (!ok)
		return rect;
	rect.setRect(l, t, w, h);

	return rect;
}

QString rectToString(const QRect& rect)
{
	return QString("%1,%2,%3,%4").arg(rect.left()).arg(rect.top()).arg(rect.width()).arg(rect.height());
}

QString fixFileName(const QString &path, const QString &fn)
{
	// check if original path is ok
	if(QFile::exists(path + '/' + fn))
		return fn;
	// maybe all lowercase?
	if(QFile::exists(path + '/' + fn.toLower()))
		return fn.toLower();
	// split for name and extension
	QString name = fn.section('.', 0, 0);
	QString ext = fn.section('.', 1);
	// maybe extension uppercase?
	if(QFile::exists(path + '/' + name + '.' + ext.toUpper()))
		return name + '.' + ext.toUpper();
	// we cannot fix it, return original
	return fn;
}

QChar extractLetter(QChar c)
{
	QString decomposition = c.decomposition();
	if (decomposition.isEmpty())
		return c;

	int length = decomposition.length();
	for (int i = 0; i < length; i++)
		if (decomposition.at(i).isLetter())
			return decomposition.at(i);

	return c;
}
