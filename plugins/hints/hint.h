/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2004, 2005 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2007, 2008, 2009 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef HINT_H
#define HINT_H

#include <QtGui/QFrame>
#include <QtGui/QVBoxLayout>

#include "chat/chat.h"

#include "configuration/configuration-aware-object.h"

class QLabel;
class QVBoxLayout;

class Chat;
class Notification;

// Hint needs to inherit QFrame (not just QWidget) for better stylesheets support.
class Hint : public QFrame, ConfigurationAwareObject
{
	Q_OBJECT

	QVBoxLayout *vbox;

	QHBoxLayout *labels;
	QHBoxLayout *callbacksBox;

	QLabel *icon;
	QLabel *label;
	QColor bcolor; //kolor t�a
	QColor fcolor;
	unsigned int secs;
	unsigned int startSecs;

	Notification *notification;
	QStringList details;

	bool requireCallbacks;

	void createLabels(const QPixmap &pixmap);
	void updateText();

	void resetTimeout();

	Chat CurrentChat;

private slots:
	void notificationClosed();

protected:
	virtual void mouseReleaseEvent(QMouseEvent * event);
	virtual void enterEvent(QEvent *event);
	virtual void leaveEvent(QEvent *event);

	virtual void configurationUpdated();

public:
	Hint(QWidget *parent, Notification *notification);
	virtual ~Hint();

	void mouseOver();
	void mouseOut();

	void getData(QString &text, QPixmap &pixmap, unsigned int &timeout, QFont &font, QColor &fgcolor, QColor &bgcolor);
	bool requireManualClosing();
	bool isDeprecated();

	void addDetail(const QString &detail);

	Chat  chat() { return CurrentChat; }

	Notification * getNotification() { return notification; }

public slots:
	/**
		min�a sekunda, zmniejsza licznik pozosta�ych sekund,
		zwraca true je�eli jeszcze pozosta� czas
		false, gdy czas si� sko�czy�
	**/
	void nextSecond();

	void acceptNotification();
	void discardNotification();

signals:
	void leftButtonClicked(Hint *hint);
	void rightButtonClicked(Hint *hint);
	void midButtonClicked(Hint *hint);
	void closing(Hint *hint);
	void updated(Hint *hint);
};

#endif // HINT_H
