/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Zamazal Jiri (zamazal.jiri@gmail.com)
 * Copyright 2011, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtWidgets/QFileDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QToolButton>

#include "icons/kadu-icon.h"
#include "misc/misc.h"

#include "select-file.h"

SelectFile::SelectFile(const QString &type, QWidget *parent)
	: QWidget(parent), Type(type)
{
	createGui();
}

SelectFile::SelectFile(QWidget *parent)
	: QWidget(parent)
{
	createGui();
}

SelectFile::~SelectFile()
{
}

void SelectFile::createGui()
{
	QHBoxLayout *layout = new QHBoxLayout;

	LineEdit = new QLineEdit(this);
	connect(LineEdit, SIGNAL(editingFinished()), this, SLOT(fileEdited()));

	QToolButton *selectFile = new QToolButton(this);
	selectFile->setAutoRaise(true);
	selectFile->setIcon(KaduIcon("document-open").icon());
	selectFile->setIconSize(QSize{14, 14});
	connect(selectFile, SIGNAL(clicked()), this, SLOT(selectFileClicked()));

	layout->addWidget(LineEdit);
	layout->addWidget(selectFile);

	setLayout(layout);
	layout->setMargin(0);
	layout->setSpacing(0);
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

#include "moc_select-file.cpp"
