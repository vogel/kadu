/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qapplication.h>
#include <qlayout.h>
#include <qmap.h>
#include <qpushbutton.h>
#include <QVBoxLayout>
#include <QLabel>
#include <QList>

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
	: QDialog(NULL, NULL, false, Qt::WType_TopLevel | Qt::WStyle_Customize | Qt::WStyle_DialogBorder | Qt::WStyle_Title | Qt::WStyle_SysMenu | Qt::WDestructiveClose),
	  notification(notification)
{
	kdebugf();

	notification->acquire();

	setCaption(notification->title());

	QVBoxLayout* vbox = new QVBoxLayout(this, 0);
	vbox->setMargin(10);
	vbox->setSpacing(10);
	
	QWidget* labels = new QWidget;
	QHBoxLayout* labels_layout = new QHBoxLayout;
	labels_layout->setSpacing(10);
	
	if (!notification->icon().isNull())
	{
		QLabel *i = new QLabel;
		i->setPixmap(icons_manager->loadIcon(notification->icon()).pixmap());
		labels_layout->addWidget(i);
	}

	QLabel* l = new QLabel;
	l->setText(notification->text());

	labels_layout->addWidget(l);
	labels->setLayout(labels_layout);
	vbox->addWidget(labels, 0, Qt::AlignCenter);

	QWidget* buttons = new QWidget;
	QHBoxLayout* buttons_layout = new QHBoxLayout;
	buttons_layout->setSpacing(20);
	buttons->setLayout(buttons_layout);
	vbox->addWidget(buttons, 0, Qt::AlignCenter);

	const QList<QPair<QString, const char *> > callbacks = notification->getCallbacks();

	if (callbacks.size())
	{
		FOREACH (i, callbacks)
			addButton(buttons, (*i).first, (*i).second);
	}
	else
		addButton(buttons, tr("OK"), SLOT(callbackAccept()));

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
	QPushButton *button = new QPushButton();
	parent->layout()->addWidget(button);
	button->setText(caption);
	connect(button, SIGNAL(clicked()), notification, slot);
	connect(button, SIGNAL(clicked()), notification, SLOT(clearDefaultCallback()));
}

WindowNotify::WindowNotify(QObject *parent, const char *name) : Notifier(parent, name)
{
	kdebugf();

	createDefaultConfiguration();
	notification_manager->registerNotifier(QT_TRANSLATE_NOOP("@default", "Window"), this);

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

void WindowNotify::createDefaultConfiguration()
{
	config_file.addVariable("Notify", "FileTransfer/IncomingFile_Window", true);
}

WindowNotify *window_notify=NULL;

/** @} */

