/*
 * %kadu copyright begin%
 * Copyright 2008, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008 Dawid Stawiarski (neeo@kadu.net)
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
#include <QtGui/QKeyEvent>
#include <QtGui/QPushButton>
#include <QtGui/QScrollArea>
#include <QtGui/QStyle>
#include <QtGui/QVBoxLayout>

#include "file-transfer/file-transfer-manager.h"
#include "file-transfer/file-transfer.h"

#include "misc/misc.h"
#include "debug.h"

#include "gui/widgets/file-transfer-widget.h"

#include "file-transfer-window.h"

FileTransferWindow::FileTransferWindow(QWidget *parent) :
	QFrame(parent), DesktopAwareObject(this)
{
	kdebugf();

	setWindowRole("kadu-file-transfer");

	createGui();
	loadWindowGeometry(this, "General", "TransferWindowGeometry", 200, 200, 500, 300);

	foreach (FileTransfer fileTransfer, FileTransferManager::instance()->items())
			fileTransferAdded(fileTransfer);
	connect(FileTransferManager::instance(), SIGNAL(fileTransferAdded(FileTransfer)),
			this, SLOT(fileTransferAdded(FileTransfer)));
	connect(FileTransferManager::instance(), SIGNAL(fileTransferRemoved(FileTransfer)),
			this, SLOT(fileTransferRemoved(FileTransfer)));

	contentsChanged();

	kdebugf2();
}

FileTransferWindow::~FileTransferWindow()
{
	kdebugf();

	disconnect(FileTransferManager::instance(), 0, this, 0);

	saveWindowGeometry(this, "General", "TransferWindowGeometry");

	kdebugf2();
}

void FileTransferWindow::createGui()
{
	setWindowTitle(tr("Kadu - file transfers"));
	setMinimumSize(QSize(100, 100));

	setAttribute(Qt::WA_DeleteOnClose);

	QVBoxLayout *layout = new QVBoxLayout(this);

	ScrollView = new QScrollArea(this);

	layout->addWidget(ScrollView);
	ScrollView->move(0, 0);

	InnerFrame = new QFrame(this);
 	InnerFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

	TransfersLayout = new QVBoxLayout(InnerFrame);
	TransfersLayout->setDirection(QBoxLayout::Up);

 	ScrollView->setWidget(InnerFrame);
	ScrollView->setWidgetResizable(true);

	QDialogButtonBox *buttons = new QDialogButtonBox(Qt::Horizontal, this);

	QPushButton *clearButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogResetButton), tr("Clear"), buttons);
	connect(clearButton, SIGNAL(clicked(bool)), this, SLOT(clearClicked()));

	QPushButton *closeButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCloseButton), tr("Close"), this);
	connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

	buttons->addButton(closeButton, QDialogButtonBox::RejectRole);
	buttons->addButton(clearButton, QDialogButtonBox::DestructiveRole);

	layout->addSpacing(16);
	layout->addWidget(buttons);
}

void FileTransferWindow::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Escape)
	{
		close();
		e->accept();
	}
	else
		QFrame::keyPressEvent(e);
}

void FileTransferWindow::fileTransferAdded(FileTransfer fileTransfer)
{
	FileTransferWidget *ftm = new FileTransferWidget(fileTransfer, InnerFrame);
	TransfersLayout->addWidget(ftm);
	Widgets.append(ftm);

	contentsChanged();
}

void FileTransferWindow::fileTransferRemoved(FileTransfer fileTransfer)
{
	foreach (FileTransferWidget *ftm, Widgets)
		if (ftm->fileTransfer() == fileTransfer)
		{
			ftm->deleteLater();
			contentsChanged();
			Widgets.removeAll(ftm);
			return;
		}
}

void FileTransferWindow::clearClicked()
{
	FileTransferManager::instance()->cleanUp();
}

void FileTransferWindow::contentsChanged()
{
	kdebugf();

	TransfersLayout->invalidate();
}
