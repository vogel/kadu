/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012 Marcel Zięba (marseel@gmail.com)
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

#define MARGIN 5

#include <QtGui/QApplication>
#include <QtGui/QBoxLayout>
#include <QtGui/QCheckBox>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPainter>
#include <QtGui/QPushButton>
#include <QtGui/QStyleOptionViewItemV4>

#include "configuration/configuration-manager.h"
#include "gui/widgets/categorized-list-view.h"
#include "gui/widgets/categorized-list-view-painter.h"
#include "gui/widgets/plugin-list-view-delegate.h"
#include "gui/widgets/plugin-list-widget.h"
#include "gui/windows/message-dialog.h"
#include "icons/kadu-icon.h"
#include "model/categorized-sort-filter-proxy-model.h"
#include "plugins/model/plugin-model.h"
#include "plugins/model/plugin-proxy-model.h"
#include "plugins/plugin.h"
#include "plugins/plugin-info.h"
#include "plugins/plugins-manager.h"
#include "plugins/model/plugin-model.h"

#include "plugin-list-widget-item-delegate.h"

PluginListWidgetItemDelegate::PluginListWidgetItemDelegate(PluginListWidget *pluginSelector_d, QObject *parent)
                : PluginListWidgetDelegate(pluginSelector_d->ListView, parent)
                , checkBox(new QCheckBox)
                , pushButton(new QPushButton)
                , pluginSelector_d(pluginSelector_d)
{
        pushButton->setIcon(KaduIcon("preferences-other").icon()); // only for getting size matters
}

PluginListWidgetItemDelegate::~PluginListWidgetItemDelegate()
{
        delete checkBox;
        delete pushButton;
}

void PluginListWidgetItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
        if (!index.isValid())
        {
                return;
        }

        int xOffset = checkBox->sizeHint().width();
#ifdef Q_WS_WIN
        xOffset += QApplication::style()->pixelMetric(QStyle::PM_CheckBoxLabelSpacing);
#endif

        bool disabled = !index.model()->data(index, PluginModel::IsCheckableRole).toBool();

        painter->save();

        QApplication::style()->drawPrimitive(QStyle::PE_PanelItemViewItem, &option, painter, 0);

        int iconSize = option.rect.height() - MARGIN * 2;

        if (pluginSelector_d->ShowIcons)
        {
//         QPixmap pixmap = KIconLoader::global()->loadIcon(index.model()->data(index, Qt::DecorationRole).toString(),
//                                                          KIconLoader::Desktop, iconSize, disabled ? KIconLoader::DisabledState : KIconLoader::DefaultState);

//         painter->drawPixmap(QRect(pluginSelector_d->dependantLayoutValue(MARGIN + option.rect.left() + xOffset, iconSize, option.rect.width()), MARGIN + option.rect.top(), iconSize, iconSize), pixmap, QRect(0, 0, iconSize, iconSize));
        }
        else
        {
                iconSize = -MARGIN;
        }

        QRect contentsRect(pluginSelector_d->dependantLayoutValue(MARGIN * 2 + iconSize + option.rect.left() + xOffset, option.rect.width() - MARGIN * 3 - iconSize - xOffset, option.rect.width()), MARGIN + option.rect.top(), option.rect.width() - MARGIN * 3 - iconSize - xOffset, option.rect.height() - MARGIN * 2);

        int lessHorizontalSpace = MARGIN * 2 + pushButton->sizeHint().width();

        if (index.model()->data(index, PluginModel::ServicesCountRole).toBool())
        {
                lessHorizontalSpace += MARGIN + pushButton->sizeHint().width();
        }

        contentsRect.setWidth(contentsRect.width() - lessHorizontalSpace);

        if (option.state & QStyle::State_Selected)
        {
                painter->setPen(option.palette.highlightedText().color());
        }

        if (pluginSelector_d->ListView->layoutDirection() == Qt::RightToLeft)
        {
                contentsRect.translate(lessHorizontalSpace, 0);
        }

        painter->save();

        if (disabled)
        {
                QPalette pal(option.palette);
                pal.setCurrentColorGroup(QPalette::Disabled);
                painter->setPen(pal.text().color());
        }

        painter->save();

        QFont font = titleFont(option.font);
        QFontMetrics fmTitle(font);
        painter->setFont(font);
        painter->drawText(contentsRect, Qt::AlignLeft | Qt::AlignTop, fmTitle.elidedText(index.model()->data(index, Qt::DisplayRole).toString(), Qt::ElideRight, contentsRect.width()));
        painter->restore();
        painter->drawText(contentsRect, Qt::AlignLeft | Qt::AlignBottom, option.fontMetrics.elidedText(index.model()->data(index, PluginModel::CommentRole).toString(), Qt::ElideRight, contentsRect.width()));

        QFont subfont = subtitleFont(option.font);
        QFontMetrics fmSubtitle(subfont);
        painter->setFont(subfont);
        painter->drawText(contentsRect, Qt::AlignLeft | Qt::AlignVCenter, fmSubtitle.elidedText(index.model()->data(index, PluginModel::NameRole).toString(), Qt::ElideRight, contentsRect.width()));

        painter->restore();
        painter->restore();
}

QSize PluginListWidgetItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
        int i = 5;
        int j = 1;

        if (index.model()->data(index, PluginModel::ServicesCountRole).toBool())
        {
                i = 6;
                j = 2;
        }

        if (!pluginSelector_d->ShowIcons)
        {
                i--;
        }

        QFont font = titleFont(option.font);

        QFontMetrics fmTitle(font);

        return QSize(qMax(fmTitle.width(index.model()->data(index, Qt::DisplayRole).toString()),
                          option.fontMetrics.width(index.model()->data(index, PluginModel::CommentRole).toString())) +
			  + MARGIN * i + pushButton->sizeHint().width() * j,
                     qMax(MARGIN * 2, fmTitle.height() * 2 + option.fontMetrics.height() + MARGIN * 2));
}

QList<QWidget*> PluginListWidgetItemDelegate::createItemWidgets() const
{
        QList<QWidget*> widgetList;

        QCheckBox *enabledCheckBox = new QCheckBox;
        connect(enabledCheckBox, SIGNAL(clicked(bool)), this, SLOT(slotStateChanged(bool)));
        connect(enabledCheckBox, SIGNAL(clicked(bool)), this, SLOT(emitChanged()));

        QPushButton *aboutPushButton = new QPushButton;
        aboutPushButton->setIcon(KaduIcon("help-contents").icon());
        connect(aboutPushButton, SIGNAL(clicked(bool)), this, SLOT(slotAboutClicked()));

        QPushButton *configurePushButton = new QPushButton;
        configurePushButton->setIcon(KaduIcon("preferences-other").icon());
        connect(configurePushButton, SIGNAL(clicked(bool)), this, SLOT(slotConfigureClicked()));

        setBlockedEventTypes(enabledCheckBox, QList<QEvent::Type>() << QEvent::MouseButtonPress
                             << QEvent::MouseButtonRelease << QEvent::MouseButtonDblClick
                             << QEvent::KeyPress << QEvent::KeyRelease);

        setBlockedEventTypes(aboutPushButton, QList<QEvent::Type>() << QEvent::MouseButtonPress
                             << QEvent::MouseButtonRelease << QEvent::MouseButtonDblClick
                             << QEvent::KeyPress << QEvent::KeyRelease);

        setBlockedEventTypes(configurePushButton, QList<QEvent::Type>() << QEvent::MouseButtonPress
                             << QEvent::MouseButtonRelease << QEvent::MouseButtonDblClick
                             << QEvent::KeyPress << QEvent::KeyRelease);

        widgetList << enabledCheckBox << configurePushButton << aboutPushButton;

        return widgetList;
}

void PluginListWidgetItemDelegate::updateItemWidgets(const QList<QWidget*> widgets,
                const QStyleOptionViewItem &option,
                const QPersistentModelIndex &index) const
{
        QCheckBox *checkBox = static_cast<QCheckBox*>(widgets[0]);
        checkBox->resize(checkBox->sizeHint());
        checkBox->move(pluginSelector_d->dependantLayoutValue(MARGIN, checkBox->sizeHint().width(), option.rect.width()), option.rect.height() / 2 - checkBox->sizeHint().height() / 2);

        QPushButton *aboutPushButton = static_cast<QPushButton*>(widgets[2]);
        QSize aboutPushButtonSizeHint = aboutPushButton->sizeHint();
        aboutPushButton->resize(aboutPushButtonSizeHint);
        aboutPushButton->move(pluginSelector_d->dependantLayoutValue(option.rect.width() - MARGIN - aboutPushButtonSizeHint.width(), aboutPushButtonSizeHint.width(), option.rect.width()), option.rect.height() / 2 - aboutPushButtonSizeHint.height() / 2);

        QPushButton *configurePushButton = static_cast<QPushButton*>(widgets[1]);
        QSize configurePushButtonSizeHint = configurePushButton->sizeHint();
        configurePushButton->resize(configurePushButtonSizeHint);
        configurePushButton->move(pluginSelector_d->dependantLayoutValue(option.rect.width() - MARGIN * 2 - configurePushButtonSizeHint.width() - aboutPushButtonSizeHint.width(), configurePushButtonSizeHint.width(), option.rect.width()), option.rect.height() / 2 - configurePushButtonSizeHint.height() / 2);

        if (!index.isValid() || !index.internalPointer())
        {
                checkBox->setVisible(false);
                aboutPushButton->setVisible(false);
                configurePushButton->setVisible(false);
        }
        else
        {
                checkBox->setChecked(index.model()->data(index, Qt::CheckStateRole).toBool());
                checkBox->setEnabled(index.model()->data(index, PluginModel::IsCheckableRole).toBool());
				configurePushButton->setVisible(false);
                //configurePushButton->setVisible(index.model()->data(index, PluginModel::ServicesCountRole).toBool());
                //configurePushButton->setEnabled(index.model()->data(index, Qt::CheckStateRole).toBool());
        }
}

void PluginListWidgetItemDelegate::slotStateChanged(bool state)
{
        if (!focusedIndex().isValid())
                return;

        const_cast<QAbstractItemModel*>(focusedIndex().model())->setData(focusedIndex(), state, Qt::CheckStateRole);
}

void PluginListWidgetItemDelegate::emitChanged()
{
        emit changed(true);
}

void PluginListWidgetItemDelegate::slotAboutClicked()
{
        const QModelIndex index = focusedIndex();
        const QAbstractItemModel *model = index.model();

        QString info;
        info += tr("Plugin name: %1").arg(model->data(index, PluginModel::NameRole).toString()) + "\n";

        PluginEntry *pluginEntry = model->data(index, PluginModel::PluginEntryRole).value<PluginEntry*>();
		Plugin *plugin = PluginsManager::instance()->plugins().value(pluginEntry->pluginName);

		if (plugin)
		{
        	PluginInfo *pluginInfo = plugin->info();

        	if (pluginInfo)
        	{
                	info += tr("Author: %1").arg(pluginInfo->author()) + "\n";
                	info += tr("Version: %1").arg(pluginInfo->version()) + "\n";
                	info += tr("Description: %1").arg(pluginInfo->description()) + "\n";
                	info += tr("Dependencies: %1").arg(pluginInfo->dependencies().join(", ")) + "\n";
                	info += tr("Conflicts: %1").arg(pluginInfo->conflicts().join(", "));
        	}
		}

        MessageDialog::show(KaduIcon("dialog-information"), tr("Plugin information"), info, QMessageBox::Ok, itemView());
}

void PluginListWidgetItemDelegate::slotConfigureClicked()
{
}

QFont PluginListWidgetItemDelegate::titleFont(const QFont &baseFont) const
{
        QFont retFont(baseFont);
        retFont.setBold(true);

        return retFont;
}

QFont PluginListWidgetItemDelegate::subtitleFont(const QFont &baseFont) const
{
        QFont retFont(baseFont);
        retFont.setItalic(true);
	retFont.setPointSize(baseFont.pointSize()-2);

        return retFont;
}