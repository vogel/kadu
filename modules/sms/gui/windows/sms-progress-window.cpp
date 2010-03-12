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

#include "gui/widgets/progress-icon.h"

#include "sms-sender.h"

#include "sms-progress-window.h"

SmsProgressWindow::SmsProgressWindow(SmsSender *sender, QWidget *parent) :
		QWidget(parent), TokenLabel(0), TokenEdit(0), TokenAcceptButton(0), Sender(sender)
{
	setAttribute(Qt::WA_DeleteOnClose);

	Sender->setParent(this);
	Sender->setTokenReader(this);

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

	Progress = new ProgressIcon(topWidget);

	topWidgetLayout->addWidget(Progress, 0, Qt::AlignTop);

	MessageLabel = new QLabel(tr("Sending SMS in progress."), topWidget);

	QWidget *rightWidget = new QWidget(this);
	Layout = new QVBoxLayout(rightWidget);
	Layout->addWidget(MessageLabel, 0, Qt::AlignLeft);

	topWidgetLayout->addWidget(rightWidget, 0, Qt::AlignTop);
/*
	QDialogButtonBox *buttons = new QDialogButtonBox(this);
	CloseButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCloseButton), tr("Close"));
	CloseButton->setEnabled(false);
	connect(CloseButton, SIGNAL(clicked(bool)), this, SLOT(close()));

	buttons->addButton(CloseButton, QDialogButtonBox::DestructiveRole);*/

// 	mainLayout->addWidget(buttons);
}

QString SmsProgressWindow::readToken(const QPixmap& tokenPixmap)
{
	Q_UNUSED(tokenPixmap);

	// ignore
	return QString::null;
}

void SmsProgressWindow::readTokenAsync(const QPixmap &tokenPixmap, TokenAcceptor *acceptor)
{
	Q_UNUSED(acceptor);

	MessageLabel->setText(tr("Enter text from the picture:"));

	TokenLabel = new QLabel(this);
	TokenLabel->setPixmap(tokenPixmap);

	Layout->addWidget(TokenLabel);

	QWidget *editWidget = new QWidget(this);
	QHBoxLayout *editLayout = new QHBoxLayout(editWidget);

	Layout->addWidget(editWidget);

	TokenEdit = new QLineEdit(this);
	connect(TokenEdit, SIGNAL(returnPressed()), this, SLOT(tokenValueEntered()));

	editLayout->addWidget(TokenEdit);

	TokenAcceptButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogOkButton), tr("Ok"), this);
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
}
