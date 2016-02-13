/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QPointer>
#include <QtWidgets/QFrame>
#include <QtWidgets/QVBoxLayout>
#include <injeqt/injeqt.h>

#include "chat/chat.h"

#include "configuration/configuration-aware-object.h"

class QLabel;
class QVBoxLayout;

class Chat;
class Configuration;
class IconsManager;
class NotificationCallbackRepository;
class NotificationConfiguration;
class NotificationService;
class Notification;
class Parser;

// Hint needs to inherit QFrame (not just QWidget) for better stylesheets support.
class Hint : public QFrame, ConfigurationAwareObject
{
	Q_OBJECT

	QPointer<Configuration> m_configuration;
	QPointer<IconsManager> m_iconsManager;
	QPointer<NotificationCallbackRepository> m_notificationCallbackRepository;
	QPointer<NotificationConfiguration> m_notificationConfiguration;
	QPointer<NotificationService> m_notificationService;
	QPointer<Parser> m_parser;

	QVBoxLayout *vbox;

	QHBoxLayout *labels;
	QHBoxLayout *callbacksBox;

	QLabel *icon;
	QLabel *label;
	QColor bcolor; //kolor t�a
	QColor fcolor;
	int secs;
	int startSecs;

	const Notification &notification;
	QStringList details;

	void createLabels(const QPixmap &pixmap);
	void updateText();

	void resetTimeout();

	Chat CurrentChat;

private slots:
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setIconsManager(IconsManager *IconsManager);
	INJEQT_SET void setNotificationCallbackRepository(NotificationCallbackRepository *notificationCallbackRepository);
	INJEQT_SET void setNotificationConfiguration(NotificationConfiguration *notificationConfiguration);
	INJEQT_SET void setNotificationService(NotificationService *notificationService);
	INJEQT_SET void setParser(Parser *parser);
	INJEQT_INIT void init();

	void buttonClicked();

protected:
	virtual void mouseReleaseEvent(QMouseEvent * event);
	virtual void enterEvent(QEvent *event);
	virtual void leaveEvent(QEvent *event);

	virtual void configurationUpdated();

public:
	Hint(QWidget *parent, const Notification &notification);
	virtual ~Hint();

	void mouseOver();
	void mouseOut();
	void notificationUpdated();

	void getData(QString &text, QPixmap &pixmap, int &timeout, QFont &font, QColor &fgcolor, QColor &bgcolor);
	bool isDeprecated();

	Chat  chat() { return CurrentChat; }

	const Notification & getNotification() { return notification; }

public slots:
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
