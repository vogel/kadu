#include <qlineedit.h>
#include <qpushbutton.h>

#include "icons_manager.h"
#include "misc.h"

#include "select_file.h"

SelectFile::SelectFile(const QString &type, QWidget *parent, char *name)
	: QHBox(parent, name), Type(type)
{
	LineEdit = new QLineEdit(this);

	QPushButton *selectFile = new QPushButton(QIconSet(icons_manager->loadIcon("OpenFile")), "", this);
	connect(selectFile, SIGNAL(clicked()), this, SLOT(selectFileClicked()));
}

SelectFile::SelectFile(QWidget *parent, char *name)
	: QHBox(parent, name)
{
	LineEdit = new QLineEdit(this);

	QPushButton *selectFile = new QPushButton(QIconSet(icons_manager->loadIcon("OpenFile")), "", this);
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
}

QString SelectFile::file() const
{
	return LineEdit->text();
}

void SelectFile::setFile(const QString &file)
{
	LineEdit->setText(file);
}
