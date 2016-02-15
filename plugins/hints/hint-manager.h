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
#include "configuration/configuration-aware-object.h"
#include "gui/widgets/abstract-tool-tip.h"
#include "notification/notifier.h"
#include "hint.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class QHBoxLayout;
class QFrame;

class BuddyList;
class ChatManager;
class ChatWidgetManager;
class ChatWidget;
class Configuration;
class HintsConfigurationUiHandler;
class HintsConfigurationWidget;
class HintOverUserConfigurationWindow;
class InjectedFactory;
class NotifierRepository;
class Parser;
class ToolTipClassManager;
class TrayService;

class HintManager : public QObject, public Notifier, public AbstractToolTip, public ConfigurationAwareObject
{
	Q_OBJECT

	QPointer<ChatManager> m_chatManager;
	QPointer<ChatWidgetManager> m_chatWidgetManager;
	QPointer<Configuration> m_configuration;
	QPointer<InjectedFactory> m_injectedFactory;
	QPointer<NotifierRepository> m_notifierRepository;
	QPointer<Parser> m_parser;
	QPointer<ToolTipClassManager> m_toolTipClassManager;
	QPointer<TrayService> m_trayService;

	QPointer<QFrame> frame;
	QPointer<QTimer> hint_timer;
	QPointer<QFrame> tipFrame;
	QVBoxLayout *layout;
	QString Style;
	double Opacity;

	QList<Hint *> hints;

	HintsConfigurationWidget *configurationWidget;

	void processButtonPress(const QString &buttonName, Hint *hint);

	void showNewMessage(const QString &configurationDirective, const QString &title, const QString &contentTitle, const BuddyList &buddies, const QString &msg);

	/**
		ustala r�g, od kt�rego b�dzie liczona pozycja grupy dymk�w
	**/
	void setLayoutDirection();

	void createDefaultConfiguration();

private slots:
	INJEQT_SET void setChatManager(ChatManager *chatManager);
	INJEQT_SET void setChatWidgetManager(ChatWidgetManager *chatWidgetManager);
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_SET void setNotifierRepository(NotifierRepository *notifierRepository);
	INJEQT_SET void setParser(Parser *parser);
	INJEQT_SET void setToolTipClassManager(ToolTipClassManager *toolTipClassManager);
	INJEQT_SET void setTrayService(TrayService *trayService);
	INJEQT_INIT void init();
	INJEQT_DONE void done();

	/**
		min��a sekunda: aktualizuje czasy wszystkich dymk�w
		usuwa te, kt�rym min�� czas
	**/
	void oneSecond();

	/**
		przesuwa ca�� grup� hint�w zgodnie z preferowanym rozmiarem grupy
		i wykryt�/okre�lon� w konfiguracji pozycj� traya
	**/
	void setHint();

	/**
		na dymku o numerze id klikni�to lewym przyciskiem myszy
		w zale�no�ci od konfiguracji otwierana jest nowa rozmowa, dymek jest kasowany lub kasowane s� wszystkie
	**/
	void leftButtonSlot(Hint *hint);

	/**
		na dymku o numerze id klikni�to prawym przyciskiem myszy
		w zale�no�ci od konfiguracji otwierana jest nowa rozmowa, dymek jest kasowany lub kasowane s� wszystkie
	**/
	void rightButtonSlot(Hint *hint);

	/**
		na dymku o numerze id klikni�to �rodkowym przyciskiem myszy
		w zale�no�ci od konfiguracji otwierana jest nowa rozmowa, dymek jest kasowany lub kasowane s� wszystkie
	**/
	void midButtonSlot(Hint *hint);

	void deleteHint(Hint *hint);
	void deleteHintAndUpdate(Hint *hint);

	/**
		Pokazuje dymek zwi�zany z notyfikacj�.
	**/
	Hint *addHint(const Notification &notification);

	/**
		je�eli dymek dotyczy� konkrentej osoby lub grupy os�b, to otwierane jest okno chatu
		dymek jest kasowany
	**/
	void openChat(Hint *hint);

	void chatUpdated(const Chat &chat);

	/**
		usuwa wszystkie dymki
	**/
	void deleteAllHints();

	void hintUpdated();

protected:
	virtual void configurationUpdated();

public:
	Q_INVOKABLE explicit HintManager(QObject *parent = nullptr);
	virtual ~HintManager();

	virtual void notify(const Notification &notification);

	virtual void showToolTip(const QPoint &point, Talkable talkable);
	virtual void hideToolTip();

	virtual NotifierConfigurationWidget * createConfigurationWidget(QWidget *parent = nullptr);

	void prepareOverUserHint(QFrame *tipFrame, QLabel *tipLabel, Talkable talkable);

	const QString & style() const { return Style; }
	double opacity() { return Opacity; }

};
