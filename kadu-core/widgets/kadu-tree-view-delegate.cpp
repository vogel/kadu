/*
 * %kadu copyright begin%
 * Copyright 2011, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtWidgets/QAbstractItemView>

#include "accounts/account-manager.h"
#include "core/injected-factory.h"
#include "identities/identity-manager.h"
#include "identities/identity.h"
#include "widgets/talkable-painter.h"
#include "widgets/talkable-tree-view.h"

#include "kadu-tree-view-delegate.h"

KaduTreeViewDelegate::KaduTreeViewDelegate(TalkableTreeView *parent) :
		QItemDelegate(parent)
{
}

KaduTreeViewDelegate::~KaduTreeViewDelegate()
{
}

void KaduTreeViewDelegate::setAccountManager(AccountManager *accountManager)
{
	m_accountManager = accountManager;
}

void KaduTreeViewDelegate::setIdentityManager(IdentityManager *identityManager)
{
	m_identityManager = identityManager;
}

void KaduTreeViewDelegate::setInjectedFactory(InjectedFactory *injectedFactory)
{
	m_injectedFactory = injectedFactory;
}

void KaduTreeViewDelegate::init()
{
	Configuration = m_injectedFactory->makeOwned<TalkableDelegateConfiguration>(static_cast<TalkableTreeView*>(parent()), this);
	// force initial signal/slot connection to happen
	ShowIdentityNameIfMany = false;
	setShowIdentityNameIfMany(true);
}

void KaduTreeViewDelegate::updateShowIdentityName()
{
	if (!ShowIdentityNameIfMany)
		return;

	int activeIdentitiesCount = 0;
	foreach (const Identity &identity, m_identityManager->items())
		if (identity.hasAnyAccountWithDetails())
			if (++activeIdentitiesCount > 1)
				break;

	Configuration->setShowIdentityName(activeIdentitiesCount > 1);
}

void KaduTreeViewDelegate::setShowIdentityNameIfMany(bool showIdentityNameIfMany)
{
	if (showIdentityNameIfMany == ShowIdentityNameIfMany)
		return;

	ShowIdentityNameIfMany = showIdentityNameIfMany;
	if (ShowIdentityNameIfMany)
	{
		connect(m_accountManager, SIGNAL(accountLoadedStateChanged(Account)), this, SLOT(updateShowIdentityName()));
		connect(m_accountManager, SIGNAL(accountUpdated(Account)), this, SLOT(updateShowIdentityName()));
		updateShowIdentityName();
	}
	else
	{
		disconnect(m_accountManager, 0, this, 0);
		Configuration->setShowIdentityName(false);
	}
}

void KaduTreeViewDelegate::setUseConfigurationColors(bool use)
{
	Configuration->setUseConfigurationColors(use);
}

QStyleOptionViewItemV4 KaduTreeViewDelegate::getOptions(const QModelIndex &index, const QStyleOptionViewItem &option) const
{
	QStyleOptionViewItemV4 opt = setOptions(index, option);

	const QStyleOptionViewItemV2 *v2 = qstyleoption_cast<const QStyleOptionViewItemV2 *>(&option);
	opt.features = v2
		? v2->features
		: QStyleOptionViewItemV2::ViewItemFeatures(QStyleOptionViewItemV2::None);
	const QStyleOptionViewItemV3 *v3 = qstyleoption_cast<const QStyleOptionViewItemV3 *>(&option);
	opt.locale = v3 ? v3->locale : QLocale();
	opt.widget = v3 ? v3->widget : 0;

	return opt;
}

QSize KaduTreeViewDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	TalkablePainter talkablePainter(Configuration, getOptions(index, option), index);
	return QSize(0, talkablePainter.height());
}

void KaduTreeViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QStyleOptionViewItemV4 options = getOptions(index, option);

	const QAbstractItemView *widget = qobject_cast<const QAbstractItemView *>(options.widget);
	if (!widget)
		return;

	QStyle *style = widget->style();
	style->drawControl(QStyle::CE_ItemViewItem, &options, painter, widget);

	TalkablePainter talkablePainter(Configuration, options, index);
	talkablePainter.paint(painter);
}

#include "moc_kadu-tree-view-delegate.cpp"
