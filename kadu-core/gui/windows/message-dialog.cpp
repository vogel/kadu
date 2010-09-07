/*
 * %kadu copyright begin%
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2004 Tomasz Jarzynka (tomee@cpi.pl)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2003, 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2003 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008, 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include <QtCore/QMap>
#include <QtGui/QApplication>
#include <QtGui/QCloseEvent>
#include <QtGui/QDesktopWidget>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>

#include "debug.h"
#include "icons-manager.h"

#include "message-dialog.h"

const int MessageDialog::OK       = 1;  // 00001
const int MessageDialog::CANCEL   = 2;  // 00010
const int MessageDialog::YES      = 4;  // 00100
const int MessageDialog::NO       = 8;  // 01000
const int MessageDialog::RETRY    = 16; // 10000

MessageDialog::MessageDialog(const QString &message, int components, bool modal, const QString &iconPath, QWidget *parent)
	: QDialog(parent, Qt::Window | Qt::MSWindowsFixedSizeDialogHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint),
	message(message)
{
	kdebugf();

	setWindowRole("kadu-message-dialog");

	setWindowTitle("Kadu");
	setAttribute(Qt::WA_DeleteOnClose);

	setModal(modal);

	QVBoxLayout *vbox = new QVBoxLayout(this);
	vbox->setContentsMargins(10, 10, 10, 10);
	vbox->setSpacing(10);

	QHBoxLayout *hboxlabels = new QHBoxLayout();
	hboxlabels->addStretch(1);
	vbox->addLayout(hboxlabels);

	if (!iconPath.isEmpty())
	{
		icon = new QLabel(this);
		icon->setPixmap(IconsManager::instance()->iconByPath(iconPath).pixmap(32, 32));
		hboxlabels->addWidget(icon, 0, Qt::AlignCenter);
	}

	QLabel *label = new QLabel(this);
	if (!message.isEmpty())
		label->setText(message);

	hboxlabels->addWidget(label, 0, Qt::AlignCenter);
	hboxlabels->addStretch(1);

	QHBoxLayout *hboxbuttons = new QHBoxLayout();
	vbox->addLayout(hboxbuttons);

	QWidget *buttons = new QWidget;
	QHBoxLayout *buttons_layout = new QHBoxLayout(buttons);
	buttons_layout->setSpacing(20);

	if (components & OK)
		addButton(buttons_layout, tr("&OK"), SLOT(okClicked()));

	if (components & YES)
		addButton(buttons_layout, tr("&Yes"), SLOT(yesClicked()));

	if (components & NO)
		addButton(buttons_layout, tr("&No"), SLOT(noClicked()));

	if (components & CANCEL)
		addButton(buttons_layout, tr("&Cancel"), SLOT(cancelClicked()));

	if (components & RETRY)
		addButton(buttons_layout, tr("&Try Again"), SLOT(okClicked()));

	hboxbuttons->addWidget(buttons, 0, Qt::AlignCenter);
 	buttons->setMaximumSize(buttons_layout->sizeHint());

	/* center on screen */
        resize(sizeHint());
	move(QApplication::desktop()->screenGeometry().center() - geometry().center());

	kdebugf2();
}

MessageDialog::~MessageDialog()
{
	if (Boxes.contains(message))
		Boxes.remove(message);
}

void MessageDialog::addButton(QBoxLayout *parent, const QString &caption, const char *slot)
{
	QPushButton *b = new QPushButton;
	b->setText(caption);
	parent->addWidget(b);
	connect(b, SIGNAL(clicked()), this, slot);
}

void MessageDialog::closeEvent(QCloseEvent *e)
{
	e->ignore();
}

void MessageDialog::okClicked()
{
	emit okPressed();
	accept();
}

void MessageDialog::cancelClicked()
{
	emit cancelPressed();
	reject();
}

void MessageDialog::yesClicked()
{
	emit yesPressed();
	accept();
}

void MessageDialog::noClicked()
{
	emit noPressed();
	reject();
}

void MessageDialog::status(const QString &message)
{
	MessageDialog *m = new MessageDialog(message);
	m->show();
	Boxes.insert(message,m);
	qApp->processEvents();
}

void MessageDialog::msg(const QString &message, bool modal, const QString &iconPath, QWidget *parent)
{
	MessageDialog *m = new MessageDialog(message, OK, modal, iconPath, parent);

	if (modal)
		m->exec();
	else
		m->show();
}

bool MessageDialog::ask(const QString &message, const QString &iconPath, QWidget *parent)
{
	MessageDialog *m = new MessageDialog(message, YES|NO, true, iconPath, parent);
	return (m->exec() == Accepted);
}

void MessageDialog::close(const QString &message)
{
	if (Boxes.contains(message))
	{
		Boxes[message]->accept();
		Boxes.remove(message);
	}
}

QMap<QString,MessageDialog*> MessageDialog::Boxes;
