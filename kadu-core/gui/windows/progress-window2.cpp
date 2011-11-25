/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
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
#include <QtGui/QCloseEvent>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QLabel>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QStyle>
#include <QtGui/QVBoxLayout>

#include "progress-window2.h"

ProgressWindow2::ProgressWindow2(const QString &initText, QWidget *parent) :
		QDialog(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowRole("kadu-progress");
	createGui();

	TextLabel->setText(initText);

	setFixedWidth(width());
}

ProgressWindow2::~ProgressWindow2()
{
}

void ProgressWindow2::createGui()
{
	QVBoxLayout *mainLayout = new QVBoxLayout(this);

	TextLabel = new QLabel(this);
	TextLabel->setWordWrap(true);

	ProgressBar = new QProgressBar(this);
	ProgressBar->setMinimum(0);
	ProgressBar->setMaximum(0);

	QDialogButtonBox *buttons = new QDialogButtonBox(this);
	CloseButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCloseButton), tr("Close"));
	CloseButton->setEnabled(false);
	connect(CloseButton, SIGNAL(clicked(bool)), this, SLOT(close()));

	buttons->addButton(CloseButton, QDialogButtonBox::DestructiveRole);

	mainLayout->addWidget(TextLabel);
	mainLayout->addWidget(ProgressBar);
	mainLayout->addWidget(buttons);
}

void ProgressWindow2::enableClosing()
{
	ProgressBar->setMaximum(10);
	ProgressBar->setValue(10);

	CanClose = true;
	CloseButton->setEnabled(true);
	CloseButton->setDefault(true);
	CloseButton->setFocus();
}

void ProgressWindow2::closeEvent(QCloseEvent *e)
{
	if (!CanClose)
		e->ignore();
	else
		QDialog::closeEvent(e);
}

void ProgressWindow2::setText(const QString &text)
{
	TextLabel->setText(text);
}
