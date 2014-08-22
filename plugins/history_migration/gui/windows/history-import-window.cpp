/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtGui/QFormLayout>
#include <QtGui/QLabel>
#include <QtGui/QProgressBar>

#include "history-import-window.h"

HistoryImportWindow::HistoryImportWindow(QWidget *parent) :
		QDialog(parent)
{
	createGui();
}

HistoryImportWindow::~HistoryImportWindow()
{
}

void HistoryImportWindow::createGui()
{
	QFormLayout *layout = new QFormLayout(this);

	QLabel *description = new QLabel(this);
	description->setText(tr("History is now being imported into new format. Please wait until this task is finished."));
	layout->addRow(description);

	ChatsProgressBar = new QProgressBar(this);
	layout->addRow(new QLabel(tr("Chats progress:"), this), ChatsProgressBar);

	MessagesProgressBar = new QProgressBar(this);
	layout->addRow(new QLabel(tr("Messages progress:"), this), MessagesProgressBar);

	setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
	setFixedHeight(layout->minimumSize().height());
}

void HistoryImportWindow::setChatsCount(int count)
{
	ChatsProgressBar->setMaximum(count);
}

void HistoryImportWindow::setChatsProgress(int progress)
{
	ChatsProgressBar->setValue(progress);
}

void HistoryImportWindow::setMessagesCount(int count)
{
	MessagesProgressBar->setMaximum(count);
}

void HistoryImportWindow::setMessagesProgress(int progress)
{
	MessagesProgressBar->setValue(progress);
}

#include "moc_history-import-window.cpp"
