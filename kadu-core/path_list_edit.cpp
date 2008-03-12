/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QFileDialog>
#include <QLayout>
#include <QLineEdit>
#include <QListWidget>

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

	setWindowTitle(tr("Select paths"));
	setAttribute(Qt::WA_DeleteOnClose);

	QGridLayout *Layout = new QGridLayout(this);
	Layout->setMargin(5);
	Layout->setSpacing(5);

	PathListWidget = new QListWidget(this);
	Layout->addMultiCellWidget(PathListWidget, 0, 3, 0, 0);

	connect(PathListWidget, SIGNAL(currentTextChanged(const QString &)), this, SLOT(currentItemChanged(const QString &)));

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

	QWidget* bottom = new QWidget;
	bottom->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed));
	QHBoxLayout* bottom_layout = new QHBoxLayout;
	bottom_layout->setSpacing(5);

	QWidget *hm = new QWidget;
	hm->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));
	QPushButton *ok = new QPushButton(icons_manager->loadIcon("OkWindowButton"), tr("OK"), this);
	QPushButton *cancel = new QPushButton(icons_manager->loadIcon("CancelWindowButton"), tr("&Cancel"), this);
	bottom_layout->addWidget(hm);
	bottom_layout->addWidget(ok);
	bottom_layout->addWidget(cancel);
	bottom->setLayout(bottom_layout);

	Layout->addMultiCellWidget(bottom, 5, 5, 0, 1);
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
	PathListWidget->clear();
	PathListWidget->insertItems(0, list);
	PathListWidget->item(0)->setSelected(true);
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

	if (PathListWidget->findItems(path, Qt::MatchExactly).isEmpty())
		return false;

	return true;
}

void PathListEditWindow::addPathClicked()
{
	QString path = PathEdit->text();
	if (!validatePath(path))
		return;

	PathListWidget->insertItem(0, path);
	PathListWidget->currentItem()->setSelected(true);
}

void PathListEditWindow::changePathClicked()
{
	if (!PathListWidget->currentItem()->isSelected())
		return;

	QString path = PathEdit->text();
	if (!validatePath(path))
		return;

	QListWidgetItem *pathh = PathListWidget->takeItem(PathListWidget->currentRow());
	delete pathh;
	PathListWidget->currentItem()->setSelected(true);
}

void PathListEditWindow::deletePathClicked()
{
	if (!PathListWidget->currentItem()->isSelected())
		return;

	QListWidgetItem *path = PathListWidget->takeItem(PathListWidget->currentRow());
	delete path;
	PathListWidget->currentItem()->setSelected(true);
}

void PathListEditWindow::choosePathClicked()
{
	QString path = PathEdit->text();
	if (!validatePath(path))
		path = QDir::homeDirPath();

	path = QFileDialog::getExistingDirectory(path, this, "getDirectory", tr("Choose a directory"));
	if (!path.isEmpty())
		PathEdit->setText(path);
}

void PathListEditWindow::okClicked()
{
	QStringList result;

	for (unsigned int i = 0, count = PathListWidget->count(); i < count; i++)
		result.append(PathListWidget->item(i)->text());

	emit changed(result);
	close();
}

void PathListEditWindow::closeEvent(QCloseEvent *e)
{
// 	saveGeometry(this, "General", "SelectPathDialogGeometry");
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
