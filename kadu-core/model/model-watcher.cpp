/*
 * %kadu copyright begin%
 * Copyright 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <stdio.h>

#include "model-watcher.h"

ModelWatcher::ModelWatcher(const QString &name, QAbstractItemModel *model) :
		QObject(model), Name(name)
{
	Q_ASSERT(model);

	connect(model, SIGNAL(columnsAboutToBeInserted(QModelIndex,int,int)), this, SLOT(columnsAboutToBeInserted(QModelIndex,int,int)), Qt::DirectConnection);
	connect(model, SIGNAL(columnsAboutToBeMoved(QModelIndex,int,int,QModelIndex,int)), this, SLOT(columnsAboutToBeMoved(QModelIndex,int,int,QModelIndex,int)), Qt::DirectConnection);
	connect(model, SIGNAL(columnsAboutToBeRemoved(QModelIndex,int,int)), this, SLOT(columnsAboutToBeRemoved(QModelIndex,int,int)), Qt::DirectConnection);
	connect(model, SIGNAL(columnsInserted(QModelIndex,int,int)), this, SLOT(columnsInserted(QModelIndex,int,int)), Qt::DirectConnection);
	connect(model, SIGNAL(columnsMoved(QModelIndex,int,int,QModelIndex,int)), this, SLOT(columnsMoved(QModelIndex,int,int,QModelIndex,int)), Qt::DirectConnection);
	connect(model, SIGNAL(columnsRemoved(QModelIndex,int,int)), this, SLOT(columnsRemoved(QModelIndex,int,int)), Qt::DirectConnection);
	connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(dataChanged(QModelIndex,QModelIndex)), Qt::DirectConnection);
	connect(model, SIGNAL(headerDataChanged(Qt::Orientation,int, int)), this, SLOT(headerDataChanged(Qt::Orientation,int, int)), Qt::DirectConnection);
	connect(model, SIGNAL(layoutAboutToBeChanged()), this, SLOT(layoutAboutToBeChanged()), Qt::DirectConnection);
	connect(model, SIGNAL(layoutChanged()), this, SLOT(layoutChanged()), Qt::DirectConnection);
	connect(model, SIGNAL(modelAboutToBeReset()), this, SLOT(modelAboutToBeReset()), Qt::DirectConnection);
	connect(model, SIGNAL(modelReset()), this, SLOT(modelReset()), Qt::DirectConnection);
	connect(model, SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)), this, SLOT(rowsAboutToBeInserted(QModelIndex,int,int)), Qt::DirectConnection);
	connect(model, SIGNAL(rowsAboutToBeMoved(QModelIndex,int,int,QModelIndex,int)), this, SLOT(rowsAboutToBeMoved(QModelIndex,int,int,QModelIndex,int)), Qt::DirectConnection);
	connect(model, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)), this, SLOT(rowsAboutToBeRemoved(QModelIndex,int,int)), Qt::DirectConnection);
	connect(model, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(rowsInserted(QModelIndex,int,int)), Qt::DirectConnection);
	connect(model, SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)), this, SLOT(rowsMoved(QModelIndex,int,int,QModelIndex,int)), Qt::DirectConnection);
	connect(model, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(rowsRemoved(QModelIndex,int,int)), Qt::DirectConnection);
}

ModelWatcher::~ModelWatcher()
{
}

QString ModelWatcher::indexToString(const QModelIndex &index)
{
	if (!index.isValid())
		return QString("-");

	return QString("[%1,%2,%3]").arg(index.row()).arg(index.column()).arg(indexToString(index.parent()));
}

void ModelWatcher::columnsAboutToBeInserted(const QModelIndex &parent, int start, int end)
{
	printf("columnsAboutToBeInserted: %s\n", qPrintable(QString("%1 - %2 - %3 - %4").arg(Name).arg(indexToString(parent)).arg(start).arg(end)));
}

void ModelWatcher::columnsAboutToBeMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationColumn)
{
	printf("columnsAboutToBeMoved: %s\n", qPrintable(QString("%1 - %2 - %3 - %4 - %5").arg(Name).arg(indexToString(sourceParent)).arg(sourceStart).arg(sourceEnd).arg(indexToString(destinationParent)).arg(destinationColumn)));
}

void ModelWatcher::columnsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
	printf("columnsAboutToBeRemoved: %s\n", qPrintable(QString("%1 - %2 - %3 - %4").arg(Name).arg(indexToString(parent)).arg(start).arg(end)));
}

void ModelWatcher::columnsInserted(const QModelIndex &parent, int start, int end)
{
	printf("columnsInserted: %s\n", qPrintable(QString("%1 - %2 - %3 - %4").arg(Name).arg(indexToString(parent).arg(start).arg(end))));
}

void ModelWatcher::columnsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationColumn)
{
	printf("columnsAboutToBeMoved: %s\n", qPrintable(QString("%1 - %2 - %3 - %4 - %5").arg(Name).arg(indexToString(sourceParent)).arg(sourceStart).arg(sourceEnd).arg(indexToString(destinationParent)).arg(destinationColumn)));
}

void ModelWatcher::columnsRemoved(const QModelIndex &parent, int start, int end)
{
	printf("columnsRemoved: %s\n", qPrintable(QString("%1 - %2 - %3 - %4").arg(Name).arg(indexToString(parent).arg(start).arg(end))));
}

void ModelWatcher::dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
	printf("dataChanged: %s\n", qPrintable(QString("%1 - %2 - %3").arg(Name).arg(indexToString(topLeft)).arg(indexToString(bottomRight))));
}

void ModelWatcher::headerDataChanged(Qt::Orientation orientation, int first, int last)
{
	printf("headerDataChanged: %s\n", qPrintable(QString("%1 - %2 - %3 - %4").arg(Name).arg(orientation).arg(first).arg(last)));
}

void ModelWatcher::layoutAboutToBeChanged()
{
	printf("layoutAboutToBeChanged: %s\n", qPrintable(QString("%1").arg(Name)));
}

void ModelWatcher::layoutChanged()
{
	printf("layoutChanged: %s\n", qPrintable(QString("%1").arg(Name)));
}

void ModelWatcher::modelAboutToBeReset()
{
	printf("modelAboutToBeReset: %s\n", qPrintable(QString("%1").arg(Name)));
}

void ModelWatcher::modelReset()
{
	printf("modelReset: %s\n", qPrintable(QString("%1").arg(Name)));
}

void ModelWatcher::rowsAboutToBeInserted(const QModelIndex &parent, int start, int end)
{
	printf("rowsAboutToBeInserted: %s\n", qPrintable(QString("%1 - %2 - %3 - %4").arg(Name).arg(indexToString(parent).arg(start).arg(end))));
}

void ModelWatcher::rowsAboutToBeMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow)
{
	printf("rowsAboutToBeMoved: %s\n", qPrintable(QString("%1 - %2 - %3 - %4 - %5").arg(Name).arg(indexToString(sourceParent)).arg(sourceStart).arg(sourceEnd).arg(indexToString(destinationParent)).arg(destinationRow)));
}

void ModelWatcher::rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
	printf("rowsAboutToBeRemoved: %s\n", qPrintable(QString("%1 - %2 - %3 - %4").arg(Name).arg(indexToString(parent).arg(start).arg(end))));
}

void ModelWatcher::rowsInserted(const QModelIndex &parent, int start, int end)
{
	printf("rowsInserted: %s\n", qPrintable(QString("%1 - %2 - %3 - %4").arg(Name).arg(indexToString(parent).arg(start).arg(end))));
}

void ModelWatcher::rowsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow)
{
	printf("rowsMoved: %s\n", qPrintable(QString("%1 - %2 - %3 - %4 - %5").arg(Name).arg(indexToString(sourceParent)).arg(sourceStart).arg(sourceEnd).arg(indexToString(destinationParent)).arg(destinationRow)));
}

void ModelWatcher::rowsRemoved(const QModelIndex &parent, int start, int end)
{
	printf("rowsRemoved: %s\n", qPrintable(QString("%1 - %2 - %3 - %4").arg(Name).arg(indexToString(parent).arg(start).arg(end))));
}

#include "moc_model-watcher.cpp"
