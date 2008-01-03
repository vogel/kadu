/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qlineedit.h>
#include <qpushbutton.h>

#include "icons_manager.h"
#include "misc.h"

#include "select_file.h"

SelectFile::SelectFile(const QString &type, QWidget *parent, char *name)
	: QHBox(parent, name), Type(type)
{
	LineEdit = new QLineEdit(this);

	QPushButton *selectFile = new QPushButton(icons_manager->loadIconSet("OpenFile"), "", this);
	connect(selectFile, SIGNAL(clicked()), this, SLOT(selectFileClicked()));
}

SelectFile::SelectFile(QWidget *parent, char *name)
	: QHBox(parent, name)
{
	LineEdit = new QLineEdit(this);

	QPushButton *selectFile = new QPushButton(icons_manager->loadIconSet("OpenFile"), "", this);
	connect(selectFile, SIGNAL(clicked()), this, SLOT(selectFileClicked()));
}

void SelectFile::selectFileClicked()
{
	if (Type == "image")
	{
		ImageDialog imageDialog(this);
		imageDialog.setDir(LineEdit->text());
		imageDialog.setCaption(tr("Insert image"));
		if (imageDialog.exec() == QDialog::Accepted)
			LineEdit->setText(imageDialog.selectedFile());
	}
	else if (Type == "all")
	{
		QString s(QFileDialog::getOpenFileName(LineEdit->text(), "All Files (*)", this));
		if (!s.isEmpty())
			LineEdit->setText(s);
	}
	else if (Type == "audio")
	{
		QString s(QFileDialog::getOpenFileName(LineEdit->text(), "Audio Files (*.wav *.au *.raw)", this));
		if (!s.isEmpty())
			LineEdit->setText(s);
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
