/*
 * %kadu copyright begin%
 * Copyright 2012 Marcel Zięba (marseel@gmail.com)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * %kadu copyright end%
 *
 * This file is derived from part of the KDE project
 * Copyright (C) 2007, 2006 Rafael Fernández López <ereslibre@kde.org>
 * Copyright (C) 2002-2003 Matthias Kretz <kretz@kde.org>
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

#include "plugin-list-widget-item-delegate.h"

#include "configuration/configuration-manager.h"
#include "core/core.h"
#include "gui/widgets/categorized-list-view-painter.h"
#include "gui/widgets/categorized-list-view.h"
#include "gui/windows/message-dialog.h"
#include "icons/kadu-icon.h"
#include "model/categorized-sort-filter-proxy-model.h"
#include "plugin/gui/plugin-list/plugin-list-view-delegate.h"
#include "plugin/gui/plugin-list/plugin-list-widget.h"
#include "plugin/metadata/plugin-metadata.h"
#include "plugin/model/plugin-model.h"
#include "plugin/model/plugin-model.h"
#include "plugin/model/plugin-proxy-model.h"
#include "plugin/plugin-dependency-handler.h"

#include <QtGui/QApplication>
#include <QtGui/QBoxLayout>
#include <QtGui/QCheckBox>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPainter>
#include <QtGui/QPushButton>
#include <QtGui/QStyleOptionViewItemV4>

#define MARGIN 5

PluginListWidgetItemDelegate::PluginListWidgetItemDelegate(PluginListWidget *pluginSelector, QObject *parent) :
		PluginListWidgetDelegate{pluginSelector->m_listView, parent}, m_checkBox{make_unique<QCheckBox>()},
		m_pushButton{make_unique<QPushButton>()}, m_pluginSelector{pluginSelector}
{
	m_pushButton->setIcon(KaduIcon("preferences-other").icon()); // only for getting size matters
}

PluginListWidgetItemDelegate::~PluginListWidgetItemDelegate()
{
}

void PluginListWidgetItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	if (!index.isValid())
		return;

	auto xOffset = m_checkBox->sizeHint().width();

#ifdef Q_OS_WIN32
	xOffset += QApplication::style()->pixelMetric(QStyle::PM_CheckBoxLabelSpacing);
#endif

	painter->save();

	QApplication::style()->drawPrimitive(QStyle::PE_PanelItemViewItem, &option, painter, 0);

	auto contentsRect = QRect{m_pluginSelector->dependantLayoutValue(option.rect.left() + MARGIN + xOffset,
			option.rect.width() - MARGIN * 2 - xOffset, option.rect.width()),
			MARGIN + option.rect.top(), option.rect.width() - MARGIN * 2 - xOffset, option.rect.height() - MARGIN * 2};

	auto lessHorizontalSpace = MARGIN * 2 + m_pushButton->sizeHint().width();

	contentsRect.setWidth(contentsRect.width() - lessHorizontalSpace);

	if (option.state & QStyle::State_Selected)
		painter->setPen(option.palette.highlightedText().color());

	if (m_pluginSelector->m_listView->layoutDirection() == Qt::RightToLeft)
		contentsRect.translate(lessHorizontalSpace, 0);

	{
		painter->save();
		auto font = titleFont(option.font);
		auto fmTitle = QFontMetrics{font};
		painter->setFont(font);
		painter->drawText(contentsRect, Qt::AlignLeft | Qt::AlignTop, fmTitle.elidedText(index.model()->data(index, Qt::DisplayRole).toString(), Qt::ElideRight, contentsRect.width()));
		painter->restore();
	}

	painter->drawText(contentsRect, Qt::AlignLeft | Qt::AlignBottom, option.fontMetrics.elidedText(index.model()->data(index, PluginModel::CommentRole).toString(), Qt::ElideRight, contentsRect.width()));

	auto subfont = subtitleFont(option.font);
	auto fmSubtitle = QFontMetrics{subfont};
	painter->setFont(subfont);
	painter->drawText(contentsRect, Qt::AlignLeft | Qt::AlignVCenter, fmSubtitle.elidedText(index.model()->data(index, PluginModel::NameRole).toString(), Qt::ElideRight, contentsRect.width()));

	painter->restore();
}

QSize PluginListWidgetItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	auto i = 4;
	auto j = 1;

	auto font = titleFont(option.font);
	auto fmTitle = QFontMetrics{font};

	return QSize(qMax(fmTitle.width(index.model()->data(index, Qt::DisplayRole).toString()),
			option.fontMetrics.width(index.model()->data(index, PluginModel::CommentRole).toString())) +
			+ MARGIN * i + m_pushButton->sizeHint().width() * j,
			qMax(MARGIN * 2, fmTitle.height() * 2 + option.fontMetrics.height() + MARGIN * 2));
}

QList<QWidget *> PluginListWidgetItemDelegate::createItemWidgets() const
{
	auto enabledCheckBox = new QCheckBox;
	connect(enabledCheckBox, SIGNAL(clicked(bool)), this, SLOT(slotStateChanged(bool)));

	auto aboutPushButton = new QPushButton;
	aboutPushButton->setIcon(KaduIcon("help-contents").icon());
	connect(aboutPushButton, SIGNAL(clicked(bool)), this, SLOT(slotAboutClicked()));

	setBlockedEventTypes(enabledCheckBox, QList<QEvent::Type>() << QEvent::MouseButtonPress
			<< QEvent::MouseButtonRelease << QEvent::MouseButtonDblClick
			<< QEvent::KeyPress << QEvent::KeyRelease);

	setBlockedEventTypes(aboutPushButton, QList<QEvent::Type>() << QEvent::MouseButtonPress
			<< QEvent::MouseButtonRelease << QEvent::MouseButtonDblClick
			<< QEvent::KeyPress << QEvent::KeyRelease);

	return {enabledCheckBox, aboutPushButton};
}

void PluginListWidgetItemDelegate::updateItemWidgets(const QList<QWidget *> widgets, const QStyleOptionViewItem &option,
		const QPersistentModelIndex &index) const
{
	auto checkBox = static_cast<QCheckBox*>(widgets[0]);
	checkBox->resize(checkBox->sizeHint());
	checkBox->move(m_pluginSelector->dependantLayoutValue(MARGIN, checkBox->sizeHint().width(), option.rect.width()), option.rect.height() / 2 - checkBox->sizeHint().height() / 2);

	auto aboutPushButton = static_cast<QPushButton*>(widgets[1]);
	auto aboutPushButtonSizeHint = aboutPushButton->sizeHint();
	aboutPushButton->resize(aboutPushButtonSizeHint);
	aboutPushButton->move(m_pluginSelector->dependantLayoutValue(option.rect.width() - MARGIN - aboutPushButtonSizeHint.width(), aboutPushButtonSizeHint.width(), option.rect.width()), option.rect.height() / 2 - aboutPushButtonSizeHint.height() / 2);

	if (!index.isValid() || !index.internalPointer())
	{
		checkBox->setVisible(false);
		aboutPushButton->setVisible(false);
	}
	else
		checkBox->setChecked(index.model()->data(index, Qt::CheckStateRole).toBool());
}

void PluginListWidgetItemDelegate::slotStateChanged(bool state)
{
	if (!focusedIndex().isValid())
		return;

	const_cast<QAbstractItemModel *>(focusedIndex().model())->setData(focusedIndex(), state, Qt::CheckStateRole);
}

void PluginListWidgetItemDelegate::slotAboutClicked()
{
	auto const index = focusedIndex();
	auto const model = index.model();

	auto info = QString{};
	info += tr("Plugin name: %1").arg(model->data(index, PluginModel::NameRole).toString()) + "\n";

	auto pluginMetadata = model->data(index, PluginModel::MetadataRole).value<PluginMetadata>();
	info += tr("Author: %1").arg(pluginMetadata.author()) + "\n";
	info += tr("Version: %1").arg(pluginMetadata.version()) + "\n";
	info += tr("Description: %1").arg(pluginMetadata.description()) + "\n";
	info += tr("Dependencies: %1").arg(pluginMetadata.dependencies().join(", ")) + "\n";
	info += tr("Provides: %1").arg(pluginMetadata.provides());

	MessageDialog::show(KaduIcon("dialog-information"), tr("Plugin information"), info, QMessageBox::Ok, itemView());
}

QFont PluginListWidgetItemDelegate::titleFont(const QFont &baseFont) const
{
	auto retFont = baseFont;
	retFont.setBold(true);

	return retFont;
}

QFont PluginListWidgetItemDelegate::subtitleFont(const QFont &baseFont) const
{
	auto retFont = baseFont;
	retFont.setItalic(true);
	retFont.setPointSize(baseFont.pointSize()-2);

	return retFont;
}

#include "moc_plugin-list-widget-item-delegate.cpp"
