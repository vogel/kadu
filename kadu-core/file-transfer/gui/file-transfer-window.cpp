/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "core/injected-factory.h"
#include "file-transfer/file-transfer-manager.h"
#include "file-transfer/file-transfer.h"
#include "file-transfer/gui/file-transfer-widget.h"
#include "gui/windows/main-window-repository.h"
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
}

FileTransferWindow::~FileTransferWindow()
{
}

void FileTransferWindow::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void FileTransferWindow::setFileTransferManager(FileTransferManager *fileTransferManager)
{
	m_fileTransferManager = fileTransferManager;
}

void FileTransferWindow::setInjectedFactory(InjectedFactory *injectedFactory)
{
	m_injectedFactory = injectedFactory;
}

void FileTransferWindow::setMainWindowRepository(MainWindowRepository *mainWindowRepository)
{
	m_mainWindowRepository = mainWindowRepository;
}

void FileTransferWindow::init()
{
	setWindowRole(QStringLiteral("kadu-file-transfer"));

	createGui();
	new WindowGeometryManager{new ConfigFileVariantWrapper{m_configuration, QStringLiteral("General"), QStringLiteral("TransferWindowGeometry")}, QRect(200, 200, 500, 300), this};

	for (auto &&fileTransfer : m_fileTransferManager->items())
		fileTransferAdded(fileTransfer);

	connect(m_fileTransferManager, SIGNAL(fileTransferAdded(FileTransfer)),
			this, SLOT(fileTransferAdded(FileTransfer)));
	connect(m_fileTransferManager, SIGNAL(fileTransferRemoved(FileTransfer)),
			this, SLOT(fileTransferRemoved(FileTransfer)));

	contentsChanged();
	m_mainWindowRepository->addMainWindow(this);
}

void FileTransferWindow::done()
{
	m_mainWindowRepository->removeMainWindow(this);
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
	auto ftm = m_injectedFactory->makeInjected<FileTransferWidget>(fileTransfer, m_innerFrame.get());
	m_transfersLayout->addWidget(ftm);
	m_widgets.push_back(ftm);

	contentsChanged();
}

void FileTransferWindow::fileTransferRemoved(FileTransfer fileTransfer)
{
	for (auto &&ftm : m_widgets)
		if (ftm->fileTransfer() == fileTransfer)
		{
			ftm->deleteLater();
			auto it = std::find(std::begin(m_widgets), std::end(m_widgets), ftm);
			if (it != std::end(m_widgets))
				m_widgets.erase(it);
			contentsChanged();
			return;
		}
}

void FileTransferWindow::clearClicked()
{
	m_fileTransferManager->cleanUp();
}

void FileTransferWindow::contentsChanged()
{
	auto alternate = m_widgets.size() % 2 == 0;
	for (auto &&widget : m_widgets)
	{
		widget->setBackgroundRole(alternate ? QPalette::AlternateBase : QPalette::Base);
		alternate = !alternate;
	}

	m_transfersLayout->invalidate();
}

#include "moc_file-transfer-window.cpp"
