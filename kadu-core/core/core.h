/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2010 Maciej Płaza (plaza.maciej@gmail.com)
 * Copyright 2010 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2010, 2011, 2012, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "accounts/accounts-aware-object.h"
#include "configuration/configuration-aware-object.h"
#include "exports.h"

#include <QtCore/QObject>
#include <injeqt/injector.h>

template<class T>
class DefaultProvider;

class ChatManager;
class ChatTypeManager;
class ChatWidgetRepository;
class ConfigurationManager;
class Configuration;
class ContactManager;
class FileTransferManager;
class IconsManager;
class InjectedFactory;
class KaduIcon;
class KaduWindowService;
class Myself;
class PathsProvider;
class PluginActivationService;
class PluginStateService;
class Roster;
class StatusContainerManager;
class StatusTypeManager;
class StoragePointFactory;
class SystemInfo;
class TalkableConverter;
class UnreadMessageRepository;

class KADUAPI Core : public QObject, private AccountsAwareObject, public ConfigurationAwareObject
{
	Q_OBJECT

public:
	static void createInstance(injeqt::v1::injector &&injector);
	static Core * instance();

	static QString name();
	static QString version();
	static QString nameWithVersion();

	bool isClosing() { return m_isClosing; }

	void createGui();
	void runServices();
	void runGuiServices();
	void stopServices();

	void activatePlugins();

	ChatManager * chatManager() const;
	ChatTypeManager * chatTypeManager() const;
	ChatWidgetRepository * chatWidgetRepository() const;
	ConfigurationManager * configurationManager() const;
	Configuration * configuration() const;
	ContactManager * contactManager() const;
	FileTransferManager * fileTransferManager() const;
	IconsManager * iconsManager() const;
	InjectedFactory * injectedFactory() const;
	KaduWindowService * kaduWindowService() const;
	Myself * myself() const;
	PathsProvider * pathsProvider() const;
	PluginActivationService * pluginActivationService() const;
	PluginStateService * pluginStateService() const;
	Roster * roster() const;
	UnreadMessageRepository * unreadMessageRepository() const;
	StatusContainerManager * statusContainerManager() const;
	StatusTypeManager * statusTypeManager() const;
	StoragePointFactory * storagePointFactory() const;
	SystemInfo * systemInfo() const;
	TalkableConverter * talkableConverter() const;

	void setIcon(const KaduIcon &icon);

public slots:
	void executeRemoteCommand(const QString &signal);

	void quit();

signals:
	void connecting();
	void connected();
	void disconnected();

	//TODO:
	void searchingForTrayPosition(QPoint &);

protected:
	virtual void accountRegistered(Account account);
	virtual void accountUnregistered(Account account);
	virtual void configurationUpdated();

private:
	static Core *m_instance;

	mutable injeqt::v1::injector m_injector;
	bool m_isClosing;

	Core(injeqt::v1::injector &&injector);
	virtual ~Core();

	void createDefaultConfiguration();
	void createAllDefaultToolbars();

	void init();
	void loadDefaultStatus();

private slots:
	void updateIcon();

	void deleteOldConfigurationFiles();

};
