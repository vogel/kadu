/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QHBoxLayout>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>

#include "path-edit.h"
#include <icons-manager.h>
#include <QFileDialog>

PathEdit::PathEdit(const QString &dialogTitle, QWidget *parent) :
		QWidget(parent), DialogTitle(dialogTitle)
{
	createGui();
}

PathEdit::~PathEdit()
{
}

void PathEdit::createGui()
{
	QHBoxLayout *layout = new QHBoxLayout(this);

	PathLineEdit = new QLineEdit(this);
	PathLineEdit->setMinimumWidth(200);
	layout->addWidget(PathLineEdit);

	QPushButton *selectButton = new QPushButton(IconsManager::instance()->iconByPath("document-open"), QString(), this);
	layout->addWidget(selectButton);

	connect(selectButton, SIGNAL(pressed()), this, SLOT(selectPath()));
	connect(PathLineEdit, SIGNAL(textChanged(QString)), this, SIGNAL(pathChanged(QString)));
}

QString PathEdit::path()
{
	return PathLineEdit->text();
}

void PathEdit::selectPath()
{
	QString directoryPath = QFileDialog::getExistingDirectory(this, DialogTitle, QString());
	if (!directoryPath.isEmpty())
		PathLineEdit->setText(directoryPath);
}
