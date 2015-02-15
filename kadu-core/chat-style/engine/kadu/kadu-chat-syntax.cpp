/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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


#include "kadu-chat-syntax.h"

#include <QtCore/QRegExp>

KaduChatSyntax::KaduChatSyntax(const QString &syntax)
{
	setSyntax(syntax);
}

void KaduChatSyntax::setSyntax(const QString &syntax)
{
	Syntax = syntax;
	QString syntax2 = syntax;
	QRegExp topRegexp("<kadu:top>(.*)</kadu:top>");
	if (syntax2.contains(topRegexp))
	{
		Top = topRegexp.cap(1);
		syntax2.remove(topRegexp);
	}
	else
		Top = QString();
	WithHeader = syntax2;
	WithHeader.remove("<kadu:header>");
	WithHeader.remove("</kadu:header>");
	WithoutHeader = syntax2;
	WithoutHeader.remove(QRegExp("<kadu:header>.*</kadu:header>"));
}
