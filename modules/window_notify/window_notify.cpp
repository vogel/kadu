/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qapplication.h>
#include <qhbox.h>
#include <qlayout.h>
#include <qmap.h>
#include <qpushbutton.h>
#include <qstylesheet.h>

#include "window_notify.h"
#include "debug.h"
#include "config_file.h"
#include "icons_manager.h"
#include "../notify/notify.h"
#include "../notify/notification.h"
#include "message_box.h"
#include "userlist.h"
#include "misc.h"

/**
 * @ingroup window_notify
 * @{
 */
extern "C" int window_notify_init()
{
	kdebugf();
	window_notify=new WindowNotify(NULL, "window_notify");
	kdebugf2();
	return 0;
}

extern "C" void window_notify_close()
{
	kdebugf();
	delete window_notify;
	window_notify=NULL;
	kdebugf2();
}

class NotificationWindow : public QDialog {

	Notification *notification;

	void addButton(QWidget *parent, const QString &caption, const QString &slot);

public:

	NotificationWindow(Notification *notification);

};

NotificationWindow::NotificationWindow(Notification *notification)
	: QDialog(NULL, NULL, false, WType_TopLevel | WStyle_Customize | WStyle_DialogBorder | WStyle_Title | WStyle_SysMenu | WDestructiveClose),
	  notification(notification)
{
	kdebugf();

	setCaption(notification->title());

	QVBoxLayout* vbox = new QVBoxLayout(this, 0);
	vbox->setMargin(20);
	vbox->setSpacing(20);

	QHBoxLayout* hboxlabels = new QHBoxLayout(vbox);
	QHBox* labels = new QHBox(this);
	labels->setSpacing(10);
	hboxlabels->addWidget(labels, 0, AlignCenter);

	if (!notification->icon().isNull())
	{
		QLabel *i = new QLabel(labels);
		i->setPixmap(icons_manager->loadIcon(notification->icon()));
	}

	QLabel* l = new QLabel(labels);
	l->setText(notification->text());
	vbox->addWidget(l, 0, AlignCenter);

	QHBoxLayout* hboxbuttons = new QHBoxLayout(vbox);
	QHBox* buttons = new QHBox(this);
	buttons->setSpacing(20);
	hboxbuttons->addWidget(buttons, 0, AlignCenter);

	const QValueList<QPair<QString, const char *> > callbacks = notification->getCallbacks();

	if (callbacks.size())
	{
		FOREACH (i, callbacks)
			addButton(buttons, tr((*i).first), (*i).second);
	}
	else
		addButton(buttons, tr("Ok"), SLOT(callbackAccept()));

	connect(notification, SIGNAL(closed()), this, SLOT(close()));

	buttons->setMaximumSize(buttons->sizeHint());
	kdebugf2();
}

void NotificationWindow::addButton(QWidget *parent, const QString &caption, const QString &slot)
{
	QPushButton *button = new QPushButton(parent);
	button->setText(caption);
	connect(button, SIGNAL(clicked()), notification, slot);
	connect(button, SIGNAL(clicked()), notification, SLOT(clearDefaultCallback()));
}

WindowNotify::WindowNotify(QObject *parent, const char *name) : Notifier(parent, name)
{
	kdebugf();

	QMap<QString, QString> s;
	s["NewChat"]=SLOT(newChat(Protocol *, UserListElements, const QString &, time_t));
	s["NewMessage"]=SLOT(newMessage(Protocol *, UserListElements, const QString &, time_t, bool &));
	s["ConnError"]=SLOT(connectionError(Protocol *, const QString &));
	s["toAvailable"]=SLOT(userChangedStatusToAvailable(const QString &, UserListElement));
	s["toBusy"]=SLOT(userChangedStatusToBusy(const QString &, UserListElement));
	s["toInvisible"]=SLOT(userChangedStatusToInvisible(const QString &, UserListElement));
	s["toNotAvailable"]=SLOT(userChangedStatusToNotAvailable(const QString &, UserListElement));

	s["StatusChanged"]=SLOT(userStatusChanged(UserListElement, QString, const UserStatus &));
	s["Message"]=SLOT(message(const QString &, const QString &, const QMap<QString, QVariant> *, const UserListElement *));

	config_file.addVariable("Notify", "NewChat_Window", false);
	config_file.addVariable("Notify", "NewMessage_Window", false);
	config_file.addVariable("Notify", "ConnError_Window", false);
	config_file.addVariable("Notify", "ChangingStatus_Window", false);
	config_file.addVariable("Notify", "toAvailable_Window", false);
	config_file.addVariable("Notify", "toBusy_Window", false);
	config_file.addVariable("Notify", "toInvisible_Window", false);
	config_file.addVariable("Notify", "toNotAvailable_Window", false);
	config_file.addVariable("Notify", "Message_Window", false);

	notify->registerNotifier(QT_TRANSLATE_NOOP("@default", "Window"), this, s);
	kdebugf2();
}

WindowNotify::~WindowNotify()
{
	kdebugf();
	notify->unregisterNotifier("Window");
	kdebugf2();
}

void WindowNotify::newChat(Protocol * /*protocol*/, UserListElements senders, const QString &msg, time_t /*t*/)
{
	kdebugf();
	MessageBox::msg(narg(tr("Chat with <b>%1</b><br/> <small>%2</small>"),
			senders[0].altNick(),msg));
	kdebugf2();
}

void WindowNotify::newMessage(Protocol * /*protocol*/, UserListElements senders, const QString &msg, time_t /*t*/, bool &/*grab*/)
{
	kdebugf();
	MessageBox::msg(narg(tr("New message from <b>%1</b><br/> <small>%2</small>"),
			senders[0].altNick(), msg));
	kdebugf2();
}

void WindowNotify::connectionError(Protocol *, const QString &message)
{
	kdebugf();
	MessageBox::msg(tr("<b>Error:</b> %1").arg(message));
	kdebugf2();
}

void WindowNotify::userStatusChanged(UserListElement ule, QString /*protocolName*/, const UserStatus &oldStatus)
{
	kdebugf();

	MessageBox::msg(narg(tr("<b>%1</b> changed status from <i>%2</i> to <i>%3</i>"),
					ule.altNick(),
					qApp->translate("@default", oldStatus.name().ascii()),
					qApp->translate("@default", ule.status("Gadu").name().ascii())));
	kdebugf2();
}

void WindowNotify::userChangedStatusToAvailable(const QString &/*protocolName*/, UserListElement ule)
{
	kdebugf();

	MessageBox::msg(narg(tr("<b>%1</b> changed status to <i>%2</i><br/> <small>%3</small>"),
					ule.altNick(),
					qApp->translate("@default", ule.status("Gadu").name().ascii()),
					QStyleSheet::escape(ule.status("Gadu").description())));
	kdebugf2();
}

void WindowNotify::userChangedStatusToBusy(const QString &protocolName, UserListElement ule)
{
	kdebugf();
	userChangedStatusToAvailable(protocolName, ule);
	kdebugf2();
}

void WindowNotify::userChangedStatusToInvisible(const QString &protocolName, UserListElement ule)
{
	kdebugf();
	userChangedStatusToAvailable(protocolName, ule);
	kdebugf2();
}

void WindowNotify::userChangedStatusToNotAvailable(const QString &protocolName, UserListElement ule)
{
	kdebugf();
	userChangedStatusToAvailable(protocolName, ule);
	kdebugf2();
}

void WindowNotify::message(const QString &from, const QString &message, const QMap<QString, QVariant> * /*parameters*/, const UserListElement *ule)
{
	kdebugf();
	if (ule==NULL)
		MessageBox::msg(narg(tr("<b>From %1:</b>%2"), from, message));
	else
	{
		MessageBox::msg(narg(tr("<b>From %1: %2</b> changed status to <i>%3</i>"),
						from,
						ule->altNick(),
						qApp->translate("@default", ule->status("Gadu").name().ascii())));
	}
	kdebugf2();
}

void WindowNotify::externalEvent(Notification *notification)
{
	kdebugf();

	NotificationWindow *nw = new NotificationWindow(notification);
	nw->show();

	kdebugf2();
}

WindowNotify *window_notify=NULL;

/** @} */

