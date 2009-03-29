/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CORE_H
#define CORE_H

#include <QtCore/QObject>

#include "accounts/accounts_aware_object.h"
#include "chat/chat.h"
#include "contacts/contact.h"
#include "contacts/contact-list.h"
#include "misc/token-reader.h"
#include "protocols/status.h"

#include "configuration_aware_object.h"
#include "exports.h"

class KaduWindow;
class UserStatusChanger;

class KADUAPI Core : public QObject, private AccountsAwareObject, public ConfigurationAwareObject, public TokenReader
{
	Q_OBJECT
	Q_DISABLE_COPY(Core)

	static Core *Instance;

	Contact Myself;
	KaduWindow *Window;
	bool ShowMainWindowOnStart; // TODO: 0.7.1, it is a hack

	Status NextStatus;
	UserStatusChanger *StatusChanger;

	Core();
	virtual ~Core();

	void createDefaultConfiguration();
	void createAllDefaultToolbars();

	void init();
	void loadDefaultStatus();

	void loadConfiguration();
	void storeConfiguration();

private slots:
	void changeStatus(Status status);
	void deleteOldConfigurationFiles();
	void kaduWindowDestroyed();

protected:
	virtual void accountRegistered(Account* account);
	virtual void accountUnregistered(Account* account);
	virtual void configurationUpdated();

public:
	static Core * instance();

	virtual QString readToken(const QPixmap &tokenPixmap);

	Contact myself() { return Myself; }
	Status status();

	void createGui();
	void setShowMainWindowOnStart(bool show);
	KaduWindow * kaduWindow();

	void setIcon(const QPixmap &icon);

public slots:
	void setStatus(const Status &status);
	void setOnline(const QString &description = QString::null);
	void setBusy(const QString &description = QString::null);
	void setInvisible(const QString &description = QString::null);
	void setOffline(const QString &description = QString::null);

	void quit();

signals:
	void connecting();
	void connected();
	void disconnected();

	void messageReceived(Chat *chat, Contact sender, const QString &message);

	// TODO: remove
	void settingMainIconBlocked(bool &blocked);

};

#endif // CORE_H
