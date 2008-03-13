/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qlabel.h>
#include <qlayout.h>
#include <qprogressbar.h>
#include <qpushbutton.h>
#include <QScrollArea>
#include <QGridLayout>
#include <QFrame>
#include <QKeyEvent>
#include <QVBoxLayout>
#include <QUrl>

#include "debug.h"
#include "file_transfer.h"
#include "file_transfer_manager.h"
#include "icons_manager.h"
#include "message_box.h"
#include "misc.h"
#include "userlist.h"

#include "file_transfer_window.h"

FileTransferWidget::FileTransferWidget(QWidget *parent, FileTransfer *ft)
	: QFrame(parent), ft(ft)
{
	kdebugf();

	ft->addListener(this, true);

	setBackgroundMode(Qt::PaletteBase, Qt::PaletteBase);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

	setMinimumSize(QSize(100, 100));

	setFrameStyle(QFrame::Box | QFrame::Sunken);
	setLineWidth(1);

	QGridLayout *layout = new QGridLayout(this, 3, 4, 2);
	layout->setMargin(10);
	layout->setColStretch(0, 1);
	layout->setColStretch(1, 20);
	layout->setColStretch(2, 20);

	QLabel *icon = new QLabel(this);
	icon->setBackgroundMode(Qt::PaletteBase, Qt::PaletteBase);
	layout->addMultiCellWidget(icon, 0, 2, 0, 0);

	description = new QLabel(this);
	description->setBackgroundMode(Qt::PaletteBase, Qt::PaletteBase);
	description->setScaledContents(true);
	layout->addMultiCellWidget(description, 0, 0, 1, 2);

	progress = new QProgressBar;
	progress->setMinimum(0);
	progress->setMaximum(100);
	progress->setBackgroundMode(Qt::PaletteBase, Qt::PaletteBase);
	layout->addMultiCellWidget(progress, 1, 1, 1, 2);

	status = new QLabel(this);
	status->setBackgroundMode(Qt::PaletteBase, Qt::PaletteBase);
	layout->addWidget(status, 2, 1);

	QWidget *buttons = new QWidget;
	QHBoxLayout *buttons_layout = new QHBoxLayout;
	buttons->setBackgroundMode(Qt::PaletteBase, Qt::PaletteBase);
	buttons_layout->setSpacing(2);

	pauseButton = new QPushButton(tr("Pause"), this);
	pauseButton->hide();
	connect(pauseButton, SIGNAL(clicked()), this, SLOT(pauseTransfer()));

	continueButton = new QPushButton(tr("Continue"), this);
	continueButton->hide();
	connect(continueButton, SIGNAL(clicked()), this, SLOT(continueTransfer()));

	QPushButton *deleteThis = new QPushButton(tr("Remove"), this);
	connect(deleteThis, SIGNAL(clicked()), this, SLOT(remove()));

	buttons_layout->addWidget(pauseButton);
	buttons_layout->addWidget(continueButton);
	buttons_layout->addWidget(deleteThis);
	buttons->setLayout(buttons_layout);
// 	layout->addWidget(buttons, 2, 2, Qt::AlignRight);

	UserListElement ule = userlist->byID("Gadu", QString::number(ft->contact()));

	QUrl url(ft->fileName());

	if (ft->type() == FileTransfer::TypeSend)
	{
		icon->setPixmap(icons_manager->loadIcon("FileTransferSend").pixmap());
		description->setText(tr("<b>File</b> %1 <b>to</b> %2").arg(url.fileName()).arg(ule.altNick()));
	}
	else
	{
		icon->setPixmap(icons_manager->loadIcon("FileTransferReceive").pixmap());
		description->setText(tr("<b>File</b> %1 <b>from</b> %2").arg(url.fileName()).arg(ule.altNick()));
	}

	fileTransferStatusChanged(ft);

	show();
}

FileTransferWidget::~FileTransferWidget()
{
	kdebugf();

	if (ft)
		ft->removeListener(this, true);
}

FileTransfer * FileTransferWidget::fileTransfer()
{
	kdebugf();

	return ft;
}

void FileTransferWidget::remove()
{
	kdebugf();

	if (ft->status() != FileTransfer::StatusFinished)
		if (!MessageBox::ask(tr("Are you sure you want to remove this transfer?")))
			return;
		else
			ft->stop(FileTransfer::StopFinally);

	// it will destroy widget too, see FileTransferWidget::fileTransferDestroying
	delete ft;
}

void FileTransferWidget::pauseTransfer()
{
	ft->stop();
}

void FileTransferWidget::continueTransfer()
{
	ft->start(FileTransfer::StartRestore);
}

void FileTransferWidget::newFileTransfer(FileTransfer *)
{
	kdebugf();
}

void FileTransferWidget::fileTransferFailed(FileTransfer *, FileTransfer::FileTransferError error)
{
	kdebugf();

	status->setText(tr("<b>Error</b>"));

	pauseButton->hide();
	continueButton->show();
}

void FileTransferWidget::fileTransferStatusChanged(FileTransfer *ft)
{
	progress->setValue(ft->percent());

	switch (ft->status())
	{
		case FileTransfer::StatusFrozen:
			status->setText(tr("<b>Frozen</b>"));
			pauseButton->hide();
			continueButton->show();
			break;
		case FileTransfer::StatusWaitForConnection:
			status->setText(tr("<b>Wait for connection</b>"));
			break;
		case FileTransfer::StatusTransfer:
			status->setText(tr("<b>Transfer</b>: %1 kB/s").arg(QString::number(ft->speed())));
			pauseButton->show();
			continueButton->hide();
			break;
		case FileTransfer::StatusFinished:
			status->setText(tr("<b>Finished</b>"));
			break;
		case FileTransfer::StatusRejected:
			status->setText(tr("<b>Rejected</b>"));
			pauseButton->hide();
			continueButton->hide();
			break;

		default:
			pauseButton->hide();
			continueButton->hide();
	}
}

void FileTransferWidget::fileTransferFinished(FileTransfer *)
{
	kdebugf();

	progress->setValue(ft->percent());

	status->setText(tr("Finished"));

	pauseButton->hide();
	continueButton->hide();
}

void FileTransferWidget::fileTransferDestroying(FileTransfer *)
{
	kdebugf();

	ft = 0;
	deleteLater();
}

FileTransferWindow::FileTransferWindow(QWidget *parent, const char *name)
	: QFrame(parent, name)
{
	kdebugf();

	setMinimumSize(QSize(100, 100));

	setWindowFlags(Qt::WDestructiveClose);

	setCaption(tr("Kadu - file transfers"));

	QGridLayout *mainGrid = new QGridLayout(this, 1, 1);
	mainGrid->setSpacing(2);
	mainGrid->setMargin(2);

	scrollView = new QScrollArea(this);
// 	scrollView->setResizePolicy(QScrollArea::AutoOneFit);

	mainGrid->addWidget(scrollView, 0, 0);
	scrollView->move(0, 0);

	frame = new QFrame(scrollView->widget());

 	frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

	transfersLayout = new QVBoxLayout(frame, 0, 1);
	transfersLayout->setDirection(QBoxLayout::Up);

// 	scrollView->addChild(frame, 0, 0);

	QWidget *buttonBox = new QWidget;
	QHBoxLayout *buttonBox_layout = new QHBoxLayout;
	buttonBox_layout->setMargin(2);
	buttonBox_layout->setSpacing(2);

	QPushButton *cleanButton = new QPushButton(tr("Clean"), this);
	connect(cleanButton, SIGNAL(clicked()), this, SLOT(clearClicked()));

	QPushButton *hideButton = new QPushButton(tr("Hide"), this);
	connect(hideButton, SIGNAL(clicked()), this, SLOT(close()));
	buttonBox_layout->addWidget(cleanButton);	
	buttonBox_layout->addWidget(hideButton);
	buttonBox->setLayout(buttonBox_layout);		
	mainGrid->addWidget(buttonBox, 1, 0, Qt::AlignRight);

	loadGeometry(this, "General", "TransferWindowGeometry", 200, 200, 500, 300);

	CONST_FOREACH(i, file_transfer_manager->transfers())
	{
		(*i)->addListener(this, true);
		newFileTransfer(*i);
	}

	contentsChanged();

	kdebugf2();
}

FileTransferWindow::~FileTransferWindow()
{
	kdebugf();

	CONST_FOREACH(i, file_transfer_manager->transfers())
		(*i)->removeListener(this, true);

// 	saveGeometry(this, "General", "TransferWindowGeometry");

	kdebugf2();
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

void FileTransferWindow::clearClicked()
{
	FOREACH(i, file_transfer_manager->transfers())
		if ((*i)->status() == FileTransfer::StatusFinished)
			(*i)->deleteLater();
}

void FileTransferWindow::contentsChanged()
{
	kdebugf();

	QSize boxSize = frame->sizeHint();

	frame->setMinimumHeight(boxSize.height());
	frame->setMaximumHeight(boxSize.height());

	// workaround
	// without this sometimes this scroll to strange positions
// 	int y = scrollView->contentsY();
// 	scrollView->scrollBy(0, -y);
// 	frame->setGeometry(0, 0, frame->width(), boxSize.height());
// 	scrollView->scrollBy(0, y);
}

void FileTransferWindow::newFileTransfer(FileTransfer *ft)
{
	kdebugf();

	FileTransferWidget *ftm = new FileTransferWidget(frame, ft);
	transfersLayout->addWidget(ftm);
	map.insert(ft, ftm);

	contentsChanged();
}

void FileTransferWindow::fileTransferFailed(FileTransfer *, FileTransfer::FileTransferError)
{
}

void FileTransferWindow::fileTransferStatusChanged(FileTransfer *)
{
}

void FileTransferWindow::fileTransferFinished(FileTransfer *fileTransfer)
{
}

void FileTransferWindow::fileTransferDestroying(FileTransfer *ft)
{
	kdebugf();

	if (map.contains(ft))
	{
		transfersLayout->remove(map[ft]);
		map.remove(ft);

		contentsChanged();
	}
}
