/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004, 2005, 2006 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Kermit (plaza.maciej@gmail.com)
 * Copyright 2008, 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#ifndef HINT_MANAGER_H
#define HINT_MANAGER_H

#include "configuration/configuration-aware-object.h"
#include "gui/widgets/abstract-tool-tip.h"
#include "notify/notifier.h"
#include "hint.h"

class QHBoxLayout;
class QFrame;

class ChatWidget;
class BuddyList;
class HintsConfigurationUiHandler;
class HintsConfigurationWidget;
class HintOverUserConfigurationWindow;

class HintManager : public Notifier, public AbstractToolTip, public ConfigurationAwareObject
{
	Q_OBJECT

	QFrame *frame;
	QTimer *hint_timer;
	QFrame *tipFrame;
	QVBoxLayout *layout;
	QString Style;
	double Opacity;

	QList<Hint *> hints;
	QMap<QPair<Chat , QString>, Hint *> linkedHints;

	HintsConfigurationUiHandler *UiHandler;

	HintsConfigurationWidget *configurationWidget;

	void processButtonPress(const QString &buttonName, Hint *hint);

	void showNewMessage(const QString &configurationDirective, const QString &title, const QString &contentTitle, BuddyList buddies, const QString &msg);

	/**
		ustala r�g, od kt�rego b�dzie liczona pozycja grupy dymk�w
	**/
	void setLayoutDirection();

	// TODO: usun�� w 0.6
	void realCopyConfiguration(const QString &fromCategory, const QString &fromHint, const QString &toHint);
	void createDefaultConfiguration();

	void import_0_6_5_configuration();

private slots:
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

	void notificationClosed(Notification *notification);

	/**
		Pokazuje dymek zwi�zany z notyfikacj�.
	**/
	Hint *addHint(Notification *notification);

	/**
		je�eli dymek dotyczy� konkrentej osoby lub grupy os�b, to otwierane jest okno chatu
		dymek jest kasowany
	**/
	void openChat(Hint *hint);

	void chatWidgetActivated(ChatWidget *chat);

	/**
		usuwa wszystkie dymki
	**/
	void deleteAllHints();

	void hintUpdated();

protected:
	virtual void configurationUpdated();

signals:
	/**
		HintManager szuka pozycji traya, aby wy�wietli� w jego okolicy dymki
	**/
	void searchingForTrayPosition(QPoint& pos);

public:
	HintManager(QWidget *parent = 0);
	~HintManager();

	virtual CallbackCapacity callbackCapacity() { return CallbackSupported; }
	virtual void notify(Notification *notification);

	virtual void showToolTip(const QPoint &point, BuddyOrContact buddyOrContact);
	virtual void hideToolTip();

	virtual NotifierConfigurationWidget * createConfigurationWidget(QWidget *parent = 0);

	void prepareOverUserHint(QFrame *tipFrame, QLabel *tipLabel, BuddyOrContact buddyOrContact);

	const QString & style() const { return Style; }
	double opacity() { return Opacity; }
	HintsConfigurationUiHandler *uiHandler() { return UiHandler; }

};

extern HintManager *hint_manager;

#endif // HINT_MANAGER_H
