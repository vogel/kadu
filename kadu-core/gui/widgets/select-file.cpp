/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2012 Jiri Zamazal (zamazal.jiri@gmail.com)
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

#include <QtGui/QFileDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>

#include "icons/kadu-icon.h"
#include "misc/misc.h"

#include "select-file.h"

SelectFile::SelectFile(const QString &type, QWidget *parent)
	: QWidget(parent), Type(type)
{
	QHBoxLayout *layout = new QHBoxLayout;

	LineEdit = new QLineEdit(this);
	connect(LineEdit, SIGNAL(editingFinished()), this, SLOT(fileEdited()));

	QPushButton *selectFile = new QPushButton(KaduIcon("document-open").icon(), QString(), this);
	connect(selectFile, SIGNAL(clicked()), this, SLOT(selectFileClicked()));

	layout->addWidget(LineEdit);
	layout->addWidget(selectFile);

	setLayout(layout);
}

SelectFile::SelectFile(QWidget *parent)
	: QWidget(parent)
{
	QHBoxLayout *layout = new QHBoxLayout;

	LineEdit = new QLineEdit(this);
	connect(LineEdit, SIGNAL(editingFinished()), this, SLOT(fileEdited()));

	QPushButton *selectFile = new QPushButton(KaduIcon("document-open").icon(), QString(), this);
	connect(selectFile, SIGNAL(clicked()), this, SLOT(selectFileClicked()));

	layout->addWidget(LineEdit);
	layout->addWidget(selectFile);

	setLayout(layout);
}

void SelectFile::selectFileClicked()
{
	if (Type == "image")
	{
		QString s(QFileDialog::getOpenFileName(this, tr("Insert image"), LineEdit->text(),
					tr("Images") + " (*.png *.PNG *.jpg *.JPG *.jpeg *.JPEG *.gif *.GIF *.bmp *.BMP)"));
		if (!s.isEmpty())
		{
			LineEdit->setText(s);
			emit fileChanged();
		}
	}
	else if (Type == "all")
	{
		QString s(QFileDialog::getOpenFileName(this, tr("Select File"), LineEdit->text(), tr("All Files (*)")));
		if (!s.isEmpty())
		{
			LineEdit->setText(s);
			emit fileChanged();
		}
	}
	else if (Type == "audio")
	{
		QString s(QFileDialog::getOpenFileName(this, tr("Select audio File"), LineEdit->text(), tr("Audio Files (*.wav *.au *.raw);;All Files (*)")));
		if (!s.isEmpty())
		{
			LineEdit->setText(s);
			emit fileChanged();
		}
	}
	else if (Type == "directory")
	{
		QString s(QFileDialog::getExistingDirectory(this, tr("Select folder"), LineEdit->text()));
		if (!s.isEmpty())
		{
			LineEdit->setText(s);
			emit fileChanged();
		}
	}
}

QString SelectFile::file() const
{
	return LineEdit->text();
}

void SelectFile::setFile(const QString &file)
{
	LineEdit->setText(file);
}

void SelectFile::fileEdited()
{
	if (LineEdit->isModified())
		emit fileChanged();
}
