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
	virtual ~NotificationWindow();

};

NotificationWindow::NotificationWindow(Notification *notification)
	: QDialog(NULL, NULL, false, WType_TopLevel | WStyle_Customize | WStyle_DialogBorder | WStyle_Title | WStyle_SysMenu | WDestructiveClose),
	  notification(notification)
{
	kdebugf();

	notification->acquire();

	setCaption(notification->title());

	QVBoxLayout* vbox = new QVBoxLayout(this, 0);
	vbox->setMargin(10);
	vbox->setSpacing(10);

	QHBox* labels = new QHBox(this);
	labels->setSpacing(10);
	vbox->addWidget(labels, 0, AlignCenter);

	if (!notification->icon().isNull())
	{
		QLabel *i = new QLabel(labels);
		i->setPixmap(icons_manager->loadIcon(notification->icon()));
	}

	QLabel* l = new QLabel(labels);
	l->setText(notification->text());

	QHBox* buttons = new QHBox(this);
	buttons->setSpacing(20);
	vbox->addWidget(buttons, 0, AlignCenter);

	const QValueList<QPair<QString, const char *> > callbacks = notification->getCallbacks();

	if (callbacks.size())
	{
		FOREACH (i, callbacks)
			addButton(buttons, tr((*i).first), (*i).second);
	}
	else
		addButton(buttons, tr("Ok"), SLOT(callbackAccept()));

	connect(notification, SIGNAL(closed(Notification *)), this, SLOT(close()));

	buttons->setMaximumSize(buttons->sizeHint());
	kdebugf2();
}

NotificationWindow::~NotificationWindow()
{
	notification->release();
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

	notification_manager->registerNotifier(QT_TRANSLATE_NOOP("@default", "Window"), this);

	config_file.addVariable("Notify", "ConnectionError_Window", true);
	config_file.addVariable("Notify", "FileTransfer/IncomingFile_Window", true);

	kdebugf2();
}

WindowNotify::~WindowNotify()
{
	kdebugf();
	notification_manager->unregisterNotifier("Window");
	kdebugf2();
}

void WindowNotify::notify(Notification *notification)
{
	kdebugf();

	NotificationWindow *nw = new NotificationWindow(notification);
	nw->show();

	kdebugf2();
}

WindowNotify *window_notify=NULL;

/** @} */

