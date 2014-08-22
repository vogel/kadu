/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2004 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2002, 2003, 2004, 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2002, 2003, 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2009, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2005 Marcin Ślusarz (joi@kadu.net)
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

#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>

#include "screenshot-tool-box.h"

ScreenshotToolBox::ScreenshotToolBox(QWidget *parent) :
		QFrame(parent)
{
	setFrameShape(Box);

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setSpacing(5);

	GeometryLabel = new QLabel(this);
	GeometryLabel->setAlignment(Qt::AlignHCenter);
	QFont font = GeometryLabel->font();
	font.setBold(true);
	GeometryLabel->setFont(font);

	FileSizeLabel = new QLabel(tr("0 KiB"), this);
	FileSizeLabel->setAlignment(Qt::AlignHCenter);

	QPushButton *cropButton = new QPushButton(tr("Crop"), this);
	connect(cropButton, SIGNAL(clicked(bool)), this, SIGNAL(crop()));

	QPushButton *cancelButton = new QPushButton(tr("Cancel"), this);
	connect(cancelButton, SIGNAL(clicked(bool)), this, SIGNAL(cancel()));

	layout->addWidget(GeometryLabel);
	layout->addWidget(FileSizeLabel);
	layout->addWidget(cropButton);
	layout->addWidget(cancelButton);
}

ScreenshotToolBox::~ScreenshotToolBox()
{
}

void ScreenshotToolBox::setGeometry(const QString &geometry)
{
	GeometryLabel->setText(geometry);
}

void ScreenshotToolBox::setFileSize(const QString &fileSize)
{
	FileSizeLabel->setText(fileSize);
}

#include "moc_screenshot-tool-box.cpp"
