/*
 * %kadu copyright begin%
 * Copyright 2010 Michał Obrembski (byku@byku.com.pl)
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

#ifndef IMAGELINK_H
#define IMAGELINK_H

#include <QtCore/QObject>
#include "configuration/configuration-aware-object.h"
#include "accounts/accounts-aware-object.h"

#include "contacts/contact.h"
#include "protocols/protocol.h"


class ContactSet;
class ChatWidget;
class ImageLink;
class ConfigurationUiHandler;



class ImageLink : public QObject, AccountsAwareObject, ConfigurationAwareObject 
{
	Q_OBJECT

	QMap<Account,Status> AccountStatus;

	
	void createDefaultConfiguration();
	
	bool config_show_yt;
	bool config_show_image;
	bool config_autostart;

private:

	void showObject(QString,int,ChatWidget *widget);
	
protected:
	virtual void accountRegistered(Account account);
	virtual void accountUnregistered(Account account);
	virtual void configurationUpdated();
	
public:

	ImageLink();

	~ImageLink();


public slots:

	void filterIncomingMessage(Chat chat, Contact sender, QString &msg, time_t time, bool &ignore);

};

extern ImageLink *imageLink;

#endif
