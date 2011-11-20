/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>

#include "icons/icons-manager.h"
#include "notify/notification.h"
#include "debug.h"

#include "window-notifier-window.h"

WindowNotifierWindow::WindowNotifierWindow(Notification *notification, QWidget *parent) :
		QDialog(parent, Qt::Window | Qt::MSWindowsFixedSizeDialogHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint),
		DesktopAwareObject(this), CurrentNotification(notification)
{
	kdebugf();

	setWindowRole("kadu-window-notifier");

	CurrentNotification->acquire();

	setWindowTitle(CurrentNotification->title());
	setAttribute(Qt::WA_DeleteOnClose);

	createGui();
}

WindowNotifierWindow::~WindowNotifierWindow()
{
	CurrentNotification->release();
}

void WindowNotifierWindow::createGui()
{
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setContentsMargins(10, 10, 10, 10);
	layout->setSpacing(10);

	QWidget* labels = new QWidget();
	QHBoxLayout* labelsLayout = new QHBoxLayout(labels);
	labelsLayout->setSpacing(10);

	if (!CurrentNotification->icon().icon().isNull())
	{
		QLabel *iconLabel = new QLabel;
		iconLabel->setPixmap(CurrentNotification->icon().icon().pixmap(64, 64));
		labelsLayout->addWidget(iconLabel);
	}

	QLabel *textLabel = new QLabel;
	QString text = CurrentNotification->text();
	if (!CurrentNotification->details().isEmpty())
		text += "<br/> <small>" + CurrentNotification->details() + "</small>";
	textLabel->setText(text);

	labelsLayout->addWidget(textLabel);

	layout->addWidget(labels, 0, Qt::AlignCenter);

	QWidget *buttons = new QWidget;
	QHBoxLayout *buttonsLayout = new QHBoxLayout(buttons);
	buttonsLayout->setSpacing(20);

	layout->addWidget(buttons, 0, Qt::AlignCenter);

	const QList<Notification::Callback> callbacks = CurrentNotification->getCallbacks();

	if (!callbacks.isEmpty())
		foreach(const Notification::Callback &i, callbacks)
			addButton(buttons, i.Caption, i.Slot);
	else
		addButton(buttons, tr("OK"), SLOT(callbackAccept()));

	connect(CurrentNotification, SIGNAL(closed(Notification *)), this, SLOT(close()));

	buttons->setMaximumSize(buttons->sizeHint());
}

void WindowNotifierWindow::addButton(QWidget *parent, const QString &caption, const char *slot)
{
	QPushButton *button = new QPushButton();
	parent->layout()->addWidget(button);
	button->setText(caption);
	connect(button, SIGNAL(clicked()), CurrentNotification, slot);
	connect(button, SIGNAL(clicked()), CurrentNotification, SLOT(clearDefaultCallback()));
}
