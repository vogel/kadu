/*
 * %kadu copyright begin%
 * Copyright 2016 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include <QtCore/QObject>
#include <QtCore/QRegularExpression>

class Q_DECL_EXPORT GaduReceivedHtmlFixupService : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit GaduReceivedHtmlFixupService(QObject *parent = nullptr);
	virtual ~GaduReceivedHtmlFixupService();

	/*
	 * This is a hack, we get <img name= from GG servers, but FormattedStringFactory requires <img src= as it
	 * cannot parse name= attribute this is because of QTextDocument usage.
	 */
	QString htmlFixup(QString html) const;

private:
	QRegularExpression m_imageRegExp;

};
