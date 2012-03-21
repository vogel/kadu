/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QApplication>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLayout>
#include <QtGui/QLineEdit>
#include <QtGui/QMovie>
#include <QtGui/QPushButton>
#include <QtGui/QStyle>
#include <QtGui/QVBoxLayout>

#include "gui/widgets/progress-label.h"

#include "plugins/history/history.h"

#include "sms-internal-sender.h"

#include "sms-progress-window.h"

SmsProgressWindow::SmsProgressWindow(SmsSender *sender, QWidget *parent) :
		ProgressWindow(parent), Sender(sender)
{
	connect(Sender, SIGNAL(failed(const QString &)), this, SLOT(sendingFailed(const QString &)));
	connect(Sender, SIGNAL(succeed(const QString &)), this, SLOT(sendingSucceed(const QString &)));

	Sender->setParent(this);

	setState(ProgressIcon::StateInProgress, tr("Sending SMS in progress."), true);
}

SmsProgressWindow::~SmsProgressWindow()
{
}

void SmsProgressWindow::sendingFailed(const QString &errorMessage)
{
	setState(ProgressIcon::StateFailed, errorMessage);
}

void SmsProgressWindow::sendingSucceed(const QString &message)
{
	if (History::instance()->currentStorage())
		History::instance()->currentStorage()->appendSms(Sender->number(), message);

	setState(ProgressIcon::StateFinished, tr("SMS sent successfully"));
}
