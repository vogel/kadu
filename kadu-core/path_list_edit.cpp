/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qlayout.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qvbox.h>
#include <qvgroupbox.h>

#include "debug.h"
#include "icons_manager.h"
#include "misc.h"

#include "path_list_edit.h"

PathListEdit::PathListEdit(QWidget *parent, char *name)
	: QPushButton(tr("Select"), parent, name), Dialog(0)
{
	connect(this, SIGNAL(clicked()), this, SLOT(showDialog()));
}

void PathListEdit::showDialog()
{
	if (!Dialog)
	{
		Dialog = new PathListEditWindow(PathList);
		connect(Dialog, SIGNAL(destroyed()), this, SLOT(dialogDestroyed()));
		connect(Dialog, SIGNAL(changed(const QStringList &)), this, SLOT(pathListChanged(const QStringList &)));
	}
	Dialog->show();
}

void PathListEdit::dialogDestroyed()
{
	Dialog = 0;
}

void PathListEdit::pathListChanged(const QStringList &pathList)
{
	PathList = pathList;
	emit changed();
}

void PathListEdit::setPathList(const QStringList &pathList)
{
	PathList = pathList;

	if (Dialog)
		Dialog->setPathList(PathList);
}

PathListEditWindow::PathListEditWindow(const QStringList &pathList, QWidget *parent, const char *name)
	: QWidget(parent, name)
{
	kdebugf();

	setWFlags(Qt::WDestructiveClose);
	setCaption(tr("Select paths"));

	QGridLayout *Layout = new QGridLayout(this);
	Layout->setMargin(5);
	Layout->setSpacing(5);

	PathListBox = new QListBox(this);
	Layout->addMultiCellWidget(PathListBox, 0, 3, 0, 0);

	connect(PathListBox, SIGNAL(highlighted(const QString &)), this, SLOT(currentItemChanged(const QString &)));

	QPushButton *add = new QPushButton(icons_manager->loadIcon("AddSelectPathDialogButton"), tr("Add"), this);
	QPushButton *change = new QPushButton(icons_manager->loadIcon("ChangeSelectPathDialogButton"), tr("Change"), this);
	QPushButton *remove = new QPushButton(icons_manager->loadIcon("RemoveSelectPathDialogButton"), tr("Remove"), this);

	Layout->addWidget(add, 0, 1);
	Layout->addWidget(change, 1, 1);
	Layout->addWidget(remove, 2, 1);

	connect(add, SIGNAL(clicked()), this, SLOT(addPathClicked()));
	connect(change, SIGNAL(clicked()), this, SLOT(changePathClicked()));
	connect(remove, SIGNAL(clicked()), this, SLOT(deletePathClicked()));

	PathEdit = new QLineEdit(this);
	QPushButton *choose = new QPushButton(icons_manager->loadIcon("ChooseSelectPathDialogButton"), tr("Choose"), this);

	Layout->addWidget(PathEdit, 4, 0);
	Layout->addWidget(choose, 4, 1);

	connect(choose, SIGNAL(clicked()), this, SLOT(choosePathClicked()));

	QHBox *bottom = new QHBox(this);
	bottom->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
	bottom->setSpacing(5);
	Layout->addMultiCellWidget(bottom, 5, 5, 0, 1);

	(new QWidget(bottom))->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));
	QPushButton *ok = new QPushButton(icons_manager->loadIcon("OkWindowButton"), tr("OK"), bottom);
	QPushButton *cancel = new QPushButton(icons_manager->loadIcon("CancelWindowButton"), tr("&Cancel"), bottom);

	connect(ok, SIGNAL(clicked()), this, SLOT(okClicked()));
	connect(cancel, SIGNAL(clicked()), this, SLOT(close()));

	loadGeometry(this, "General", "SelectPathDialogGeometry", 0, 30, 330, 330);
	setPathList(pathList);
}

PathListEditWindow::~PathListEditWindow()
{
}

void PathListEditWindow::setPathList(const QStringList &list)
{
	PathListBox->clear();
	PathListBox->insertStringList(list);
	PathListBox->setSelected(0, true);
}

bool PathListEditWindow::validatePath(QString &path)
{
	if (path.isEmpty())
		return false;

	QDir dir(path);
	if (!dir.isReadable())
		return false;

	if (!path.endsWith("/"))
		path += '/';

	if (PathListBox->findItem(path, Qt::ExactMatch))
		return false;

	return true;
}

void PathListEditWindow::addPathClicked()
{
	QString path = PathEdit->text();
	if (!validatePath(path))
		return;

	PathListBox->insertItem(path);
	PathListBox->setSelected(PathListBox->currentItem(), true);
}

void PathListEditWindow::changePathClicked()
{
	if (!PathListBox->isSelected(PathListBox->currentItem()))
		return;

	QString path = PathEdit->text();
	if (!validatePath(path))
		return;

	PathListBox->changeItem(path, PathListBox->currentItem());
	PathListBox->setSelected(PathListBox->currentItem(), true);
}

void PathListEditWindow::deletePathClicked()
{
	if (!PathListBox->isSelected(PathListBox->currentItem()))
		return;

	PathListBox->removeItem(PathListBox->currentItem());
	PathListBox->setSelected(PathListBox->currentItem(), true);
}

void PathListEditWindow::choosePathClicked()
{
	QString path = PathEdit->text();
	if (!validatePath(path))
		path = "~";

	path = QFileDialog::getExistingDirectory(path, this, "getDirectory", tr("Choose a directory"));
	if (!path.isEmpty())
		PathEdit->setText(path);
}

void PathListEditWindow::okClicked()
{
	QStringList result;

	for (unsigned int i = 0, count = PathListBox->count(); i < count; i++)
		result.append(PathListBox->text(i));

	emit changed(result);
	close();
}

void PathListEditWindow::closeEvent(QCloseEvent *e)
{
	saveGeometry(this, "General", "SelectPathDialogGeometry");
	QWidget::closeEvent(e);
}

void PathListEditWindow::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Delete)
		deletePathClicked();
	else if (e->key() == Qt::Key_Escape)
		close();
}

void PathListEditWindow::currentItemChanged(const QString &newItem)
{
	PathEdit->setText(newItem);
}
