/*
 * %kadu copyright begin%
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
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
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QKeyEvent>
#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>

#include "debug.h"
#include "icons-manager.h"
#include "misc/misc.h"

#include "path-list-edit.h"

PathListEdit::PathListEdit(QWidget *parent)
	: QPushButton(tr("Select"), parent), Dialog(0)
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

PathListEditWindow::PathListEditWindow(const QStringList &pathList, QWidget *parent)
	: QWidget(parent)
{
	kdebugf();

	setWindowTitle(tr("Select paths"));
	setAttribute(Qt::WA_DeleteOnClose);

	QGridLayout *Layout = new QGridLayout(this);
	Layout->setMargin(5);
	Layout->setSpacing(5);

	PathListWidget = new QListWidget(this);
	Layout->addWidget(PathListWidget, 0, 0, 4, 1);


	QPushButton *add = new QPushButton(IconsManager::instance()->iconByPath("list-add"), tr("Add"), this);
	QPushButton *change = new QPushButton(IconsManager::instance()->iconByPath("view-refresh"), tr("Change"), this);
	QPushButton *remove = new QPushButton(IconsManager::instance()->iconByPath("list-remove"), tr("Remove"), this);

	Layout->addWidget(add, 0, 1);
	Layout->addWidget(change, 1, 1);
	Layout->addWidget(remove, 2, 1);

	connect(add, SIGNAL(clicked()), this, SLOT(addPathClicked()));
	connect(change, SIGNAL(clicked()), this, SLOT(changePathClicked()));
	connect(remove, SIGNAL(clicked()), this, SLOT(deletePathClicked()));

	QWidget* bottom = new QWidget;
	bottom->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed));
	QHBoxLayout* bottom_layout = new QHBoxLayout;
	bottom_layout->setSpacing(5);

	QWidget *hm = new QWidget;
	hm->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));
	QPushButton *ok = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogOkButton), tr("OK"), this);
	QPushButton *cancel = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("&Cancel"), this);
	bottom_layout->addWidget(hm);
	bottom_layout->addWidget(ok);
	bottom_layout->addWidget(cancel);
	bottom->setLayout(bottom_layout);

	Layout->addWidget(bottom, 5, 0, 1, 2);
	connect(ok, SIGNAL(clicked()), this, SLOT(okClicked()));
	connect(cancel, SIGNAL(clicked()), this, SLOT(close()));

	loadWindowGeometry(this, "General", "SelectPathDialogGeometry", 0, 50, 330, 330);
	setPathList(pathList);
}

PathListEditWindow::~PathListEditWindow()
{
}

void PathListEditWindow::setPathList(const QStringList &list)
{
	PathListWidget->clear();
	PathListWidget->insertItems(0, list);
	if (PathListWidget->item(0))
		PathListWidget->item(0)->setSelected(true);
}

bool PathListEditWindow::validatePath(QString &path)
{
	if (path.isEmpty())
		return false;

	QDir dir(path);
	if (!dir.isReadable())
		return false;

	if (!path.endsWith('/'))
		path += '/';

	if (!(PathListWidget->findItems(path, Qt::MatchExactly).isEmpty()))
		return false;

	return true;
}

void PathListEditWindow::addPathClicked()
{
	QString path = QFileDialog::getExistingDirectory(this, tr("Choose a directory"));

	if (!validatePath(path))
		return;

	PathListWidget->insertItem(0, path);
}

void PathListEditWindow::changePathClicked()
{
	if(!PathListWidget->currentItem())
		return;

	if (!PathListWidget->currentItem()->isSelected())
		return;

	QString path = QFileDialog::getExistingDirectory(this, tr("Choose a directory"),
			PathListWidget->currentItem()->text());

	if (!validatePath(path))
		return;

	QListWidgetItem *pathh = PathListWidget->currentItem();
	pathh->setText(path);
}

void PathListEditWindow::deletePathClicked()
{
	if(!PathListWidget->currentItem())
		return;

	if (!PathListWidget->currentItem()->isSelected())
		return;

	delete PathListWidget->takeItem(PathListWidget->currentRow());
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
 	saveWindowGeometry(this, "General", "SelectPathDialogGeometry");
	QWidget::closeEvent(e);
}

void PathListEditWindow::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Delete)
	{
		e->accept();
		deletePathClicked();
	}
	else if (e->key() == Qt::Key_Escape)
	{
		e->accept();
		close();
	}
	else
		QWidget::keyPressEvent(e);
}
