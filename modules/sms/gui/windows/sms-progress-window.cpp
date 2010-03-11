/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QApplication>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QMovie>
#include <QtGui/QPushButton>
#include <QtGui/QStyle>
#include <QtGui/QVBoxLayout>

#include "sms-sender.h"

#include "sms-progress-window.h"

SmsProgressWindow::SmsProgressWindow(SmsSender *sender, QWidget *parent) :
		QWidget(parent), Sender(sender)
{
	setAttribute(Qt::WA_DeleteOnClose);

	Sender->setParent(this);

	createGui();
}

SmsProgressWindow::~SmsProgressWindow()
{
}

void SmsProgressWindow::createGui()
{
	QVBoxLayout *mainLayout = new QVBoxLayout(this);

	QWidget *topWidget = new QWidget(this);
	mainLayout->addWidget(topWidget);

	QHBoxLayout *topWidgetLayout = new QHBoxLayout(topWidget);

	IconLabel = new QLabel(topWidget);
	WaitMovie = new QMovie(IconsManager::instance()->iconPath("kadu_icons/please-wait.gif"));
	WaitMovie->start();
	IconLabel->setMovie(WaitMovie);

	topWidgetLayout->addWidget(IconLabel);

	MessageLabel = new QLabel(tr("Sending SMS in progress."), topWidget);
	topWidgetLayout->addWidget(MessageLabel, 0, Qt::AlignTop);

	QDialogButtonBox *buttons = new QDialogButtonBox(this);
	CloseButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCloseButton), tr("Close"));
	CloseButton->setEnabled(false);
	connect(CloseButton, SIGNAL(clicked(bool)), this, SLOT(close()));

	buttons->addButton(CloseButton, QDialogButtonBox::DestructiveRole);

	mainLayout->addWidget(buttons);
}
