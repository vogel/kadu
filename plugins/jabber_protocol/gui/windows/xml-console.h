/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

#ifndef XML_CONSOLE_H
#define XML_CONSOLE_H

#include <QtGui/QWidget>

#include "accounts/account.h"

class QTextEdit;

class XmlConsole : public QWidget
{
	Q_OBJECT

	Account WatchedAccount;

	QTextEdit *Viewer;

	void createGui();

private slots:
	void xmlIncomingSlot(const QString &str);
	void xmlOutgoingSlot(const QString &str);

public:
	explicit XmlConsole(Account account);
};

#endif // XML_CONSOLE_H
