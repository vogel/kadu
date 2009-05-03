/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QFileDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>

#include "icons-manager.h"
#include "misc/misc.h"

#include "select_file.h"

SelectFile::SelectFile(const QString &type, QWidget *parent)
	: QWidget(parent), Type(type)
{
	QHBoxLayout *layout = new QHBoxLayout;

	LineEdit = new QLineEdit(this);

	QPushButton *selectFile = new QPushButton(IconsManager::instance()->loadIcon("OpenFile"), "", this);
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

	QPushButton *selectFile = new QPushButton(IconsManager::instance()->loadIcon("OpenFile"), "", this);
	connect(selectFile, SIGNAL(clicked()), this, SLOT(selectFileClicked()));

	layout->addWidget(LineEdit);
	layout->addWidget(selectFile);

	setLayout(layout);
}

void SelectFile::selectFileClicked()
{
	if (Type == "image")
	{
		ImageDialog imageDialog(this);
		imageDialog.setDirectory(LineEdit->text());
		imageDialog.setWindowTitle(tr("Insert image"));
		if (imageDialog.exec() == QDialog::Accepted && 1 == imageDialog.selectedFiles().count())
			LineEdit->setText(imageDialog.selectedFiles()[0]);
	}
	else if (Type == "all")
	{
		QString s(QFileDialog::getOpenFileName(this, tr("Select File"), LineEdit->text(), "All Files (*)"));
		if (!s.isEmpty())
			LineEdit->setText(s);
	}
	else if (Type == "audio")
	{
		QString s(QFileDialog::getOpenFileName(this, tr("Select audio File"), LineEdit->text(), "Audio Files (*.wav *.au *.raw)"));
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
