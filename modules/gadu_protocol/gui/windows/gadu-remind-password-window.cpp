/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtGui/QDialogButtonBox>
#include <QtGui/QFormLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QKeyEvent>
#include <QtGui/QPushButton>
#include <QtGui/QStyle>
#include <QtGui/QVBoxLayout>

#include "gui/windows/message-dialog.h"
#include "misc/misc.h"
#include "url-handlers/url-handler-manager.h"
#include "icons-manager.h"

#include "gui/widgets/token-widget.h"
#include "server/gadu-server-remind-password.h"

#include "gadu-remind-password-window.h"

GaduRemindPasswordWindow::GaduRemindPasswordWindow(UinType uin, QWidget *parent) :
		QWidget(parent, Qt::Window), Uin(uin)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(tr("Remind password"));

	createGui();

	dataChanged();

	loadWindowGeometry(this, "General", "GaduRemindPasswordGeometry", 0, 50, 500, 275);
}

GaduRemindPasswordWindow::~GaduRemindPasswordWindow()
{
	saveWindowGeometry(this, "General", "GaduRemindPasswordGeometry");
}

void GaduRemindPasswordWindow::createGui()
{
	QVBoxLayout *mainLayout = new QVBoxLayout(this);

	QLabel *infoLabel = new QLabel;
	infoLabel->setText(tr("This dialog box allows you to ask server to remind your current password."));
	infoLabel->setWordWrap(true);
	infoLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	infoLabel->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));

	mainLayout->addWidget(infoLabel);

	QWidget *formWidget = new QWidget(this);
	mainLayout->addWidget(formWidget);

	QFormLayout *layout = new QFormLayout(formWidget);

	EMail = new QLineEdit(this);
	connect(EMail, SIGNAL(textChanged(const QString &)), this, SLOT(dataChanged()));
	layout->addRow(tr("E-Mail Address") + ':', EMail);

	infoLabel = new QLabel(tr("<font size='-1'><i>Type E-Mail Address used during registration.</i></font>"), this);
	infoLabel->setWordWrap(true);
	infoLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	infoLabel->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
	layout->addRow(0, infoLabel);

	MyTokenWidget = new TokenWidget(this);
	connect(MyTokenWidget, SIGNAL(modified()), this, SLOT(dataChanged()));
	layout->addRow(tr("Characters") + ':', MyTokenWidget);

	infoLabel = new QLabel(tr("<font size='-1'><i>For verification purposes, please type the characters above.</i></font>"), this);
	infoLabel->setWordWrap(true);
	infoLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	infoLabel->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
	layout->addRow(0, infoLabel);

	mainLayout->addStretch(100);

	QDialogButtonBox *buttons = new QDialogButtonBox(Qt::Horizontal, this);
	mainLayout->addWidget(buttons);

	SendPasswordButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogApplyButton), tr("Send Password"), this);
	QPushButton *cancelButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Cancel"), this);

	connect(SendPasswordButton, SIGNAL(clicked(bool)), this, SLOT(sendPassword()));
	connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(close()));

	buttons->addButton(SendPasswordButton, QDialogButtonBox::ApplyRole);
	buttons->addButton(cancelButton, QDialogButtonBox::RejectRole);
}

void GaduRemindPasswordWindow::dataChanged()
{
	bool disable = EMail->text().indexOf(UrlHandlerManager::instance()->mailRegExp()) < 0
			|| MyTokenWidget->tokenValue().isEmpty();

	SendPasswordButton->setEnabled(!disable);
}

void GaduRemindPasswordWindow::sendPassword()
{
	GaduServerRemindPassword *gsrp = new GaduServerRemindPassword(Uin, EMail->text(),
			MyTokenWidget->tokenId(), MyTokenWidget->tokenValue());
	connect(gsrp, SIGNAL(finished(GaduServerRemindPassword *)),
			this, SLOT(remindPasswordFinished(GaduServerRemindPassword *)));

	gsrp->performAction();
}


void GaduRemindPasswordWindow::remindPasswordFinished(GaduServerRemindPassword *gsrp)
{
	bool result = false;
	if (gsrp)
	{
		result = gsrp->result();
		delete gsrp;
	}

	if (result)
	{
		MessageDialog::show("dialog-information", tr("Kadu"), tr("Your password has been send on your email"), QMessageBox::Ok, parentWidget());
		close();
	}
	else
		MessageDialog::show("dialog-error", tr("Kadu"), tr("Error during remind password"), QMessageBox::Ok, parentWidget());
}

void GaduRemindPasswordWindow::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Escape)
	{
		e->accept();
		close();
	}
	else
		QWidget::keyPressEvent(e);
}
