/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#include <QtCore/QDir>
#include <QtCore/QTemporaryFile>
#include <QtGui/QDrag>

#include "configuration/configuration-file.h"
#include "icons/kadu-icon.h"

#include "kadu-tree-view.h"

KaduTreeView::KaduTreeView(QWidget *parent) :
		QTreeView(parent),
		BackgroundImageMode(BackgroundNone), BackgroundTemporaryFile(0)
{
	setAnimated(true);
#ifndef Q_WS_MAEMO_5
	/* Disable as we use kinetic scrolling by default */
	setDragEnabled(true);
	setDragDropMode(DragOnly);
#endif
	setItemsExpandable(true);
	setExpandsOnDoubleClick(false);
	setHeaderHidden(true);
	setMouseTracking(true);
	setSelectionMode(QAbstractItemView::ExtendedSelection);
#ifndef Q_WS_MAEMO_5
	setUniformRowHeights(false);
#endif
	setWordWrap(true);

	configurationUpdated();
}

KaduTreeView::~KaduTreeView()
{
}

void KaduTreeView::configurationUpdated()
{
	bool showExpandingControl = config_file.readBoolEntry("Look", "ShowExpandingControl", false);

	if (rootIsDecorated() && !showExpandingControl)
		collapseAll();
	setRootIsDecorated(showExpandingControl);
}

void KaduTreeView::setBackground(const QString &backgroundColor, const QString &alternateColor, const QString &file, BackgroundMode mode)
{
	BackgroundColor = backgroundColor;
	AlternateBackgroundColor = alternateColor;
	setAnimated(mode == BackgroundNone);
	BackgroundImageMode = mode;
	BackgroundImageFile = file;
	updateBackground();
}

void KaduTreeView::updateBackground()
{
	// TODO fix image "Stretched" + update on resize event - write image into resource tree
	QString style;
	style.append("QTreeView::branch:has-siblings:!adjoins-item { border-image: none; image: none }");
	style.append("QTreeView::branch:has-siblings:adjoins-item { border-image: none; image: none }");
	style.append("QTreeView::branch:has-childres:!has-siblings:adjoins-item { border-image: none; image: none }");
	if (config_file.readBoolEntry("Look", "AlignUserboxIconsTop"))
	{
		style.append("QTreeView::branch:has-children:!has-siblings:closed, QTreeView::branch:closed:has-children:has-siblings "
		     "{ border-image: none; image: url(" + KaduIcon("kadu_icons/stylesheet-branch-closed", "16x16").fullPath() + "); margin-top: 4px; image-position: top }");
		style.append("QTreeView::branch:open:has-children:!has-siblings, QTreeView::branch:open:has-children:has-siblings "
			"{ border-image: none; image: url(" + KaduIcon("kadu_icons/stylesheet-branch-open", "16x16").fullPath() + "); image-position: top; margin-top: 8px }");
	}
	else
	{
 		style.append("QTreeView::branch:has-children:!has-siblings:closed, QTreeView::branch:closed:has-children:has-siblings "
		     "{ border-image: none; image: url(" + KaduIcon("kadu_icons/stylesheet-branch-closed", "16x16").fullPath() + ") }");
		style.append("QTreeView::branch:open:has-children:!has-siblings, QTreeView::branch:open:has-children:has-siblings "
			"{ border-image: none; image: url(" + KaduIcon("kadu_icons/stylesheet-branch-open", "16x16").fullPath() + ") }");
	}

	style.append("QTreeView { background-color: transparent;");

	QString viewportStyle(QString("QWidget { background-color: %1;").arg(BackgroundColor));

	if (BackgroundImageMode == BackgroundNone)
	{
		setAlternatingRowColors(true);
		style.append(QString("alternate-background-color: %1;").arg(AlternateBackgroundColor));
	}
	else
	{
		setAlternatingRowColors(false);

		if (BackgroundImageMode != BackgroundTiled && BackgroundImageMode != BackgroundTiledAndCentered)
			viewportStyle.append("background-repeat: no-repeat;");

		if (BackgroundImageMode == BackgroundCentered || BackgroundImageMode == BackgroundTiledAndCentered)
			viewportStyle.append("background-position: center;");

		if (BackgroundImageMode == BackgroundStretched)
		{
			// style.append("background-size: 100% 100%;"); will work in 4.6 maybe?
			QImage image(BackgroundImageFile);
			if (!image.isNull())
			{
				delete BackgroundTemporaryFile;
				BackgroundTemporaryFile = new QTemporaryFile(QDir::tempPath() + "/kadu_background_XXXXXX.png", this);

				if (BackgroundTemporaryFile->open())
				{
					QImage stretched = image.scaled(viewport()->width(), viewport()->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
					if (stretched.save(BackgroundTemporaryFile, "PNG"))
						viewportStyle.append(QString("background-image: url(%1);").arg(BackgroundTemporaryFile->fileName()));
					BackgroundTemporaryFile->close();
				}
			}
		}
		else
			viewportStyle.append(QString("background-image: url(%1);").arg(BackgroundImageFile));

		viewportStyle.append("background-attachment: fixed;");
	}

	style.append("}");
	viewportStyle.append("}");

	setStyleSheet(style);
	viewport()->setStyleSheet(viewportStyle);
}

void KaduTreeView::resizeEvent(QResizeEvent *event)
{
	Q_UNUSED(event);
	// QTreeView::resizeEvent(event);

	if (BackgroundImageMode == BackgroundStretched)
		updateBackground();

	scheduleDelayedItemsLayout();
}

void KaduTreeView::startDrag(Qt::DropActions supportedActions)
{
	const QModelIndexList &indexes = selectedIndexes();
	if (indexes.isEmpty())
		return;

	QMimeData *data = model()->mimeData(indexes);
	if (!data)
		return;

	QDrag *drag = new QDrag(this);
	drag->setMimeData(data);

	drag->exec(supportedActions, Qt::LinkAction);
}
