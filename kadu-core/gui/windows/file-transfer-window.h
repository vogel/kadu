/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef FILE_TRANSFER_WINDOW
#define FILE_TRANSFER_WINDOW

#include <QtGui/QFrame>

#include "file-transfer/file-transfer.h"

class QScrollArea;
class QVBoxLayout;

class FileTransferWidget;

class FileTransferWindow : public QFrame
{
	Q_OBJECT

	QList<FileTransferWidget *> Widgets;

	QScrollArea *ScrollView;

	QFrame *InnerFrame;
	QVBoxLayout *TransfersLayout;

	void createGui();

private slots:
	void fileTransferAdded(FileTransfer fileTransfer);
	void fileTransferRemoved(FileTransfer fileTransfer);

	void clearClicked();

protected:
	virtual void keyPressEvent(QKeyEvent *e);

	void contentsChanged();

public:
	FileTransferWindow(QWidget *parent = 0);
	virtual ~FileTransferWindow();

};

#endif
