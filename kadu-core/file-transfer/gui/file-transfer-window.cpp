/*
 * %kadu copyright begin%
 * Copyright 2008, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "file-transfer-window.h"

#include "configuration/config-file-variant-wrapper.h"
#include "file-transfer/file-transfer-manager.h"
#include "file-transfer/file-transfer.h"
#include "file-transfer/gui/file-transfer-widget.h"
#include "os/generic/window-geometry-manager.h"

#include <QtGui/QKeyEvent>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QStyle>
#include <QtWidgets/QVBoxLayout>

FileTransferWindow::FileTransferWindow(QWidget *parent) :
		QFrame{parent},
		DesktopAwareObject{this}
{
	setWindowRole("kadu-file-transfer");

	createGui();
	new WindowGeometryManager{new ConfigFileVariantWrapper{"General", "TransferWindowGeometry"}, QRect(200, 200, 500, 300), this};

	for (auto &&fileTransfer : FileTransferManager::instance()->items())
		fileTransferAdded(fileTransfer);

	connect(FileTransferManager::instance(), SIGNAL(fileTransferAdded(FileTransfer)),
			this, SLOT(fileTransferAdded(FileTransfer)));
	connect(FileTransferManager::instance(), SIGNAL(fileTransferRemoved(FileTransfer)),
			this, SLOT(fileTransferRemoved(FileTransfer)));

	contentsChanged();
}

FileTransferWindow::~FileTransferWindow()
{
	disconnect(FileTransferManager::instance(), 0, this, 0);
}

void FileTransferWindow::createGui()
{
	setWindowTitle(tr("Kadu - file transfers"));
	setMinimumSize(QSize(100, 100));

	setAttribute(Qt::WA_DeleteOnClose);

	auto layout = new QVBoxLayout{this};

	m_scrollView = new QScrollArea{this};

	layout->addWidget(m_scrollView.get());
	m_scrollView->move(0, 0);

	m_innerFrame = new QFrame{this};
	m_innerFrame->setAutoFillBackground(true);
	m_innerFrame->setBackgroundRole(QPalette::Base);
	m_innerFrame->setFrameStyle(QFrame::NoFrame);
 	m_innerFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

	m_transfersLayout = new QVBoxLayout{m_innerFrame.get()};
	m_transfersLayout->setDirection(QBoxLayout::Up);
	m_transfersLayout->setMargin(0);
	m_transfersLayout->setSpacing(0);

 	m_scrollView->setWidget(m_innerFrame.get());
	m_scrollView->setWidgetResizable(true);

	auto buttons = new QDialogButtonBox{Qt::Horizontal, this};

	auto clearButton = new QPushButton{qApp->style()->standardIcon(QStyle::SP_DialogResetButton), tr("Clear"), buttons};
	connect(clearButton, SIGNAL(clicked(bool)), this, SLOT(clearClicked()));

	auto closeButton = new QPushButton{qApp->style()->standardIcon(QStyle::SP_DialogCloseButton), tr("Close"), this};
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
	auto ftm = new FileTransferWidget{fileTransfer, m_innerFrame.get()};
	m_transfersLayout->addWidget(ftm);
	m_widgets.append(ftm);

	contentsChanged();
}

void FileTransferWindow::fileTransferRemoved(FileTransfer fileTransfer)
{
	for (auto &&ftm : m_widgets)
		if (ftm->fileTransfer() == fileTransfer)
		{
			ftm->deleteLater();
			m_widgets.removeAll(ftm);
			contentsChanged();
			return;
		}
}

void FileTransferWindow::clearClicked()
{
	FileTransferManager::instance()->cleanUp();
}

void FileTransferWindow::contentsChanged()
{
	auto alternate = false;
	for (auto &&widget : m_widgets)
	{
		widget->setBackgroundRole(alternate ? QPalette::AlternateBase : QPalette::Base);
		alternate = !alternate;
	}

	m_transfersLayout->invalidate();
}

#include "moc_file-transfer-window.cpp"
