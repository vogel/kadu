/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat/chat.h"
#include "misc/memory.h"
#include "notification/notifier.h"
#include "hint.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class QHBoxLayout;
class QFrame;

class BuddyList;
class ChatWidgetManager;
class ChatWidget;
class Configuration;
class HintRepository;
class HintsConfiguration;
class HintsWidget;
class InjectedFactory;
class NotifierRepository;
class Parser;

class HintManager : public QObject, public Notifier
{
	Q_OBJECT

	QPointer<ChatManager> m_chatManager;
	QPointer<ChatWidgetManager> m_chatWidgetManager;
	QPointer<Configuration> m_configuration;
	QPointer<HintRepository> m_hintRepository;
	QPointer<HintsConfiguration> m_hintsConfiguration;
	QPointer<HintsWidget> m_hintsWidget;
	QPointer<InjectedFactory> m_injectedFactory;
	QPointer<NotifierRepository> m_notifierRepository;
	QPointer<Parser> m_parser;

	QPointer<QTimer> hint_timer;

	void showNewMessage(const QString &configurationDirective, const QString &title, const QString &contentTitle, const BuddyList &buddies, const QString &msg);

private slots:
	INJEQT_SET void setChatWidgetManager(ChatWidgetManager *chatWidgetManager);
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setHintRepository(HintRepository *hintRepository);
	INJEQT_SET void setHintsConfiguration(HintsConfiguration *hintsConfiguration);
	INJEQT_SET void setHintsWidget(HintsWidget *hintsWidget);
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_SET void setNotifierRepository(NotifierRepository *notifierRepository);
	INJEQT_SET void setParser(Parser *parser);
	INJEQT_INIT void init();
	INJEQT_DONE void done();

	void oneSecond();
	void setHint();

	void leftButtonSlot(Hint *hint);
	void rightButtonSlot(Hint *hint);
	void midButtonSlot(Hint *hint);

	void deleteHint(Hint *hint);
	void deleteHintAndUpdate(Hint *hint);

	Hint *addHint(const Notification &notification);

	void deleteAllHints();

public:
	Q_INVOKABLE explicit HintManager(QObject *parent = nullptr);
	virtual ~HintManager();

	virtual void notify(const Notification &notification);

	virtual NotifierConfigurationWidget * createConfigurationWidget(QWidget *parent = nullptr);

};
