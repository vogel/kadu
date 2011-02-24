/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
