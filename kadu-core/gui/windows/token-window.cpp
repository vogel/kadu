/*
 * %kadu copyright begin%
 * Copyright 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtWidgets/QApplication>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStyle>

#include "token-window.h"

TokenWindow::TokenWindow(const QPixmap &tokenPixmap, QWidget *parent) :
		QDialog(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(tr("Enter Token Value"));

	createGui(tokenPixmap);
}

TokenWindow::~TokenWindow()
{
}

void TokenWindow::createGui(const QPixmap &tokenPixmap)
{
	QVBoxLayout *layout = new QVBoxLayout(this);

	QLabel *imageLabel = new QLabel(this);
	imageLabel->setPixmap(tokenPixmap);

	TokenValue = new QLineEdit(this);

	QWidget *formWidget = new QWidget(this);
	layout->addWidget(formWidget);

	QFormLayout *formLayout = new QFormLayout(formWidget);
	formLayout->setRowWrapPolicy(QFormLayout::WrapAllRows);
	formLayout->addRow(0, imageLabel);
	formLayout->addRow(tr("Enter text from the picture:"), TokenValue);

	QDialogButtonBox *buttons = new QDialogButtonBox(this);

	QPushButton *okButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogOkButton), tr("Ok"), buttons);
	QPushButton *cancelButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Cancel"), buttons);

	buttons->addButton(okButton, QDialogButtonBox::AcceptRole);
	buttons->addButton(cancelButton, QDialogButtonBox::DestructiveRole);

	layout->addSpacing(16);
	layout->addWidget(buttons);

	connect(TokenValue, SIGNAL(returnPressed()), this, SLOT(accept()));
	connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}

void TokenWindow::accept()
{
	emit tokenValueEntered(TokenValue->text());
	QDialog::accept();
}

void TokenWindow::reject()
{
	emit tokenValueEntered(QString());
	QDialog::reject();
}

#include "moc_token-window.cpp"
