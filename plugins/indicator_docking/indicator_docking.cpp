/*
 * %kadu copyright begin%
 * Copyright 2011 Marcin Dawidziuk (cinekdawidziuk@gmail.com)
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

#include "core/core.h"
#include <QEvent>
#include <QPoint>
#include <QMouseEvent>

#include "indicator_docking.h"


IndicatorDocking * IndicatorDocking::Instance = 0;

void IndicatorDocking::createInstance()
{
	if (!Instance)
		Instance = new IndicatorDocking();
}

void IndicatorDocking::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

IndicatorDocking * IndicatorDocking::instance()
{ 
	return Instance;
}

IndicatorDocking::IndicatorDocking(QObject *parent) :
Notifier("ubumm_notify", QT_TRANSLATE_NOOP("@default", "Indicator"), KaduIcon("external_modules/mail-internet-mail"))
{
	Q_UNUSED(parent)

	server = QIndicate::Server::defaultInstance();
    	server->setDesktopFile(desktopFilePath()); 
  	server->setCount(0);
  	server->setType("message.im");
  	server->show();
	
	DockingManager::instance()->setDocker(this);
	NotificationManager::instance()->registerNotifier(this);
  	connect(server, SIGNAL(serverDisplay()), this, SLOT(showMainWindow()));
	connect(ChatWidgetManager::instance(), SIGNAL(chatWidgetActivated(ChatWidget *)), this, SLOT(chatWidgetActivated(ChatWidget *)));
}

void IndicatorDocking::showMainWindow(){
	QScopedPointer<QMouseEvent> event;
	event.reset(new QMouseEvent(QEvent::MouseButtonPress, QPoint(0,0), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier));
	if (!event.isNull())
		DockingManager::instance()->trayMousePressEvent(event.data());
}


void IndicatorDocking::notify(Notification *notification){
	if ((notification->type() == "NewMessage") || (notification->type() == "NewChat")){		
		notification->clearDefaultCallback();
		notification->acquire();
		ChatNotification *chatNotification = qobject_cast<ChatNotification *>(notification);
		if (!chatNotification)
			return;
		Chat chat = chatNotification ? chatNotification->chat() : Chat();
		Contact contact = *chat.contacts().constBegin();
		QString contactName = BuddyOrContact(contact).buddy().display();	
		if (contactName=="")
			return;

		QIndicate::Indicator* indicator;
		if (isThereAnyIndicatorNamed(contactName)){
			indicator = indicatorsMap[contactName];
		}else{
			indicator = new QIndicate::Indicator(server);
			indicatorsMap[contactName] = indicator;
			chatsMap[contactName] = chat;
			contactsMap[contactName] = chat.contacts();		
			indicator->setNameProperty(contactName);	
			
			Avatar avatar = BuddyOrContact(contact).contact().contactAvatar();
			avatarsMap[contactName] = QImage(avatar.pixmap().toImage().scaled(20, 20));
			indicator->setIconProperty(avatarsMap[contactName]);
			
			connect(indicator, SIGNAL(display(QIndicate::Indicator*)), SLOT(displayIndicator(QIndicate::Indicator*))); 

		}
		connect(notification, SIGNAL(closed(Notification*)), this, SLOT(notificationClosed(Notification*)));
		indicator->setTimeProperty(QDateTime::currentDateTime());
		indicator->setDrawAttentionProperty(true);
		indicator->show();
		indicatorsVisible[contactName] = true;
	}
}

void IndicatorDocking::notificationClosed(Notification* notification){
	ChatNotification *chatNotification = qobject_cast<ChatNotification *>(notification);
	if (!chatNotification)
		return;
	Chat chat = chatNotification->chat();
	Contact contact = *chatNotification->chat().contacts().begin();
	QString contactName = BuddyOrContact(contact).buddy().display();
	if (contactName=="")
			return;
	deleteIndicatorAndChat(contactName);
}

void IndicatorDocking::chatWidgetActivated(ChatWidget *){
	QMapIterator<QString, QIndicate::Indicator*> i(indicatorsMap);
 	while (i.hasNext()) {
 	    i.next();
	    i.value()->hide();
	 }
}

void IndicatorDocking::displayIndicator(QIndicate::Indicator* indicator){
	QString indicatorName = indicator->nameProperty();
	if (isThereAnyChatNamed(indicatorName)){
		ContactSet contacts = contactsMap[indicatorName];
		Chat chat = ChatManager::instance()->findChat(contacts);		
		ChatWidgetManager::instance()->openPendingMessages(chat);
	}
}

QString IndicatorDocking::getContactsFromChat(Chat chat){
	QString contacts = "";
	 QSet<Contact>::iterator i;
	i = chat.contacts().begin();
	contacts = BuddyOrContact((*i)).buddy().display();
	for (i = chat.contacts().begin(); i != chat.contacts().end(); ++i)
    		contacts = contacts + ", " + BuddyOrContact((*i)).buddy().display();
	contacts = contacts.mid(0, contacts.size()-2);
	return contacts;
}

bool IndicatorDocking::isThereAnyIndicatorNamed(QString indicatorName){
	if (indicatorsMap.count(indicatorName) > 0)
		return true;
	else
		return false;
}

bool IndicatorDocking::isThereAnyChatNamed(QString name){
	if (chatsMap.count(name) > 0)
		return true;
	else
		return false;
}

void IndicatorDocking::deleteIndicatorAndChat(QString name){
	if (isThereAnyIndicatorNamed(name)){
		if (isIndicatorVisible(name)){
			indicatorsVisible[name] = false;
			indicatorsMap[name]->hide();
		}
	}
	
}

bool IndicatorDocking::isIndicatorVisible(QString name){
	if (indicatorsVisible.count(name) > 0)
		return indicatorsVisible[name];
	else
		return false;
}

IndicatorDocking::~IndicatorDocking(){
	NotificationManager::instance()->unregisterNotifier(this);
	disconnect(ChatWidgetManager::instance(), SIGNAL(chatWidgetActivated(ChatWidget *)), this, SLOT(chatWidgetActivated(ChatWidget *)));
	server->hide();
	delete server;
}


