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
#include <QtGui/QLayout>
#include <QtGui/QLineEdit>
#include <QtGui/QMovie>
#include <QtGui/QPushButton>
#include <QtGui/QStyle>
#include <QtGui/QVBoxLayout>

#include "gui/widgets/progress-label.h"

#include "modules/history/history.h"

#include "sms-internal-sender.h"

#include "sms-progress-window.h"

SmsProgressWindow::SmsProgressWindow(SmsSender *sender, QWidget *parent) :
		ProgressWindow(parent), TokenLabel(0), TokenEdit(0), TokenAcceptButton(0), Sender(sender)
{
	connect(Sender, SIGNAL(failed(const QString &)), this, SLOT(sendingFailed(const QString &)));
	connect(Sender, SIGNAL(succeed(const QString &)), this, SLOT(sendingSucceed(const QString &)));

	Sender->setParent(this);
	Sender->setTokenReader(this);

	setState(ProgressIcon::StateInProgress, tr("Sending SMS in progress."));
}

SmsProgressWindow::~SmsProgressWindow()
{
}

QString SmsProgressWindow::readToken(const QPixmap &tokenPixmap)
{
	Q_UNUSED(tokenPixmap);

	// ignore
	return QString();
}

void SmsProgressWindow::readTokenAsync(const QPixmap &tokenPixmap, TokenAcceptor *acceptor)
{
	Q_UNUSED(acceptor);

	setState(ProgressIcon::StateInProgress, tr("Enter text from the picture:"));

	TokenLabel = new QLabel(container());
	TokenLabel->setPixmap(tokenPixmap);

	container()->layout()->addWidget(TokenLabel);

	QWidget *editWidget = new QWidget(container());
	QHBoxLayout *editLayout = new QHBoxLayout(editWidget);
	editLayout->setContentsMargins(0, 0, 0, 0);

	container()->layout()->addWidget(editWidget);

	TokenEdit = new QLineEdit(container());
	TokenEdit->setFocus();
	connect(TokenEdit, SIGNAL(returnPressed()), this, SLOT(tokenValueEntered()));

	editLayout->addWidget(TokenEdit);

	TokenAcceptButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogOkButton), tr("Ok"), this);
	TokenAcceptButton->setDefault(true);
	connect(TokenAcceptButton, SIGNAL(clicked(bool)), this, SLOT(tokenValueEntered()));

	editLayout->addWidget(TokenAcceptButton);
}

void SmsProgressWindow::tokenValueEntered()
{
	Sender->tokenRead(TokenEdit->text());

	delete TokenLabel;
	TokenLabel = 0;
	delete TokenEdit;
	TokenEdit = 0;
	delete TokenAcceptButton;
	TokenAcceptButton = 0;

	container()->layout()->invalidate();
}

void SmsProgressWindow::sendingFailed(const QString &errorMessage)
{
	setState(ProgressIcon::StateFailed, errorMessage);
}

void SmsProgressWindow::sendingSucceed(const QString &message)
{
	if (History::instance()->currentStorage())
		History::instance()->currentStorage()->appendSms(Sender->number(), message);

	setState(ProgressIcon::StateFinished, tr("Sms sent successfully"));
}
