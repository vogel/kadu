/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "tool-tip-manager.h"
#include "tool-tip-manager.moc"

#include "core/injected-factory.h"
#include "talkable/talkable.h"
#include "widgets/tool-tip-widget.h"

#include <QtWidgets/QApplication>
#include <QtWidgets/QDesktopWidget>

ToolTipManager::ToolTipManager(QObject *parent) : QObject{parent}
{
}

ToolTipManager::~ToolTipManager()
{
}

void ToolTipManager::setInjectedFactory(InjectedFactory *injectedFactory)
{
    m_injectedFactory = injectedFactory;
}

void ToolTipManager::showToolTip(const QPoint &where, const Talkable &talkable)
{
    m_toolTipWidget = m_injectedFactory->makeNotOwned<ToolTipWidget>(talkable);

    auto pos = where + QPoint{5, 5};
    auto preferredSize = m_toolTipWidget->sizeHint();
    auto desktopSize = QApplication::desktop()->screenGeometry(m_toolTipWidget).size();
    if (pos.x() + preferredSize.width() > desktopSize.width())
        pos.setX(pos.x() - preferredSize.width() - 10);
    if (pos.y() + preferredSize.height() > desktopSize.height())
        pos.setY(pos.y() - preferredSize.height() - 10);

    m_toolTipWidget->move(pos);
    m_toolTipWidget->show();
}

void ToolTipManager::hideToolTip()
{
    m_toolTipWidget.reset();
}
