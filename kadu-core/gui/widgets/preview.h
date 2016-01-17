/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010, 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QPointer>
#include <QtWidgets/QFrame>
#include <injeqt/injeqt.h>

class BuddyDummyFactory;
class KaduWebView;

class Preview : public QFrame
{
	Q_OBJECT

public:
	explicit Preview(QWidget *parent = 0);
	virtual ~Preview();

	KaduWebView * webView() const;

public slots:
	void syntaxChanged(const QString &content);

signals:
	void needFixup(QString &syntax);

private:
	QPointer<BuddyDummyFactory> m_buddyDummyFactory;

	KaduWebView *m_webView;

private slots:
	INJEQT_SET void setBuddyDummyFactory(BuddyDummyFactory *buddyDummyFactory);

};
