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

#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QScrollBar>
#include <QtGui/QStackedWidget>

#include "buddies/filter/anonymous-without-messages-buddy-filter.h"
#include "buddies/filter/buddy-name-filter.h"
#include "buddies/filter/group-buddy-filter.h"
#include "buddies/filter/pending-messages-filter.h"
#include "buddies/model/buddies-model.h"
#include "buddies/model/buddies-model-proxy.h"
#include "chat/filter/chat-named-filter.h"
#include "chat/model/chats-model.h"
#include "chat/model/chats-proxy-model.h"
#include "configuration/configuration-file.h"
#include "gui/widgets/filter-widget.h"
#include "gui/widgets/filtered-tree-view.h"
#include "gui/widgets/group-tab-bar.h"
#include "gui/windows/proxy-action-context.h"
#include "gui/widgets/talkable-tree-view.h"
#include "model/model-chain.h"

#include "roster-widget.h"

RosterWidget::RosterWidget(QWidget *parent) :
		QWidget(parent), CompositingEnabled(false)
{
	Context = new ProxyActionContext();
	createGui();
	Context->setForwardActionContext(BuddiesTree->actionContext());

	configurationUpdated();
}

RosterWidget::~RosterWidget()
{
	delete Context;
	Context = 0;
}

void RosterWidget::createGui()
{
	QVBoxLayout *layout = new QVBoxLayout(this);

	QWidget *selectViewButtons = new QWidget(this);
	QHBoxLayout *selectViewButtonsLayout = new QHBoxLayout(selectViewButtons);

	ViewButtonGroup = new QButtonGroup(this);

	BuddiesViewButton = new QPushButton(tr("Buddies"), this);
	BuddiesViewButton->setCheckable(true);

	ChatsViewButton = new QPushButton(tr("Chats"), this);
	ChatsViewButton->setCheckable(true);

	ViewButtonGroup->addButton(BuddiesViewButton);
	ViewButtonGroup->addButton(ChatsViewButton);
	BuddiesViewButton->setChecked(true);
	connect(ViewButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(viewButtonClicked()));

	selectViewButtonsLayout->setContentsMargins(0, 0, 0, 0);
	selectViewButtonsLayout->setSpacing(0);
	selectViewButtonsLayout->addWidget(BuddiesViewButton);
	selectViewButtonsLayout->addWidget(ChatsViewButton);
	selectViewButtonsLayout->addStretch(1);

	GroupBar = new GroupTabBar(this);

	TalkableViews = new QStackedWidget(this);
	TalkableViews->addWidget(createBuddiesWidget(TalkableViews));
	TalkableViews->addWidget(createChatsWidget(TalkableViews));

	QWidget* hbox = new QWidget(this);
	QHBoxLayout *hboxLayout = new QHBoxLayout(hbox);
	hboxLayout->setMargin(0);
	hboxLayout->setSpacing(0);

	hboxLayout->addWidget(GroupBar);
	hboxLayout->setStretchFactor(GroupBar, 1);
	hboxLayout->addWidget(TalkableViews);
	hboxLayout->setStretchFactor(TalkableViews, 100);

	layout->addWidget(selectViewButtons);
	layout->addWidget(hbox);

	viewButtonClicked();
}

void RosterWidget::viewButtonClicked()
{
	if (ViewButtonGroup->checkedButton() == ChatsViewButton)
	{
		TalkableViews->setCurrentIndex(1);
		Context->setForwardActionContext(ChatsTree->actionContext());
	}
	else
	{
		TalkableViews->setCurrentIndex(0);
		Context->setForwardActionContext(BuddiesTree->actionContext());
	}
}

void RosterWidget::configurationUpdated()
{
	QString bgColor = config_file.readColorEntry("Look","UserboxBgColor").name();
	QString alternateBgColor = config_file.readColorEntry("Look","UserboxAlternateBgColor").name();

	if (CompositingEnabled && config_file.readBoolEntry("Look", "UserboxTransparency"))
	{
		int alpha = config_file.readNumEntry("Look", "UserboxAlpha");

		QColor color(bgColor);
		bgColor = QString("rgba(%1,%2,%3,%4)").arg(color.red()).arg(color.green()).arg(color.blue()).arg(alpha);

		color = QColor(alternateBgColor);
		alternateBgColor = QString("rgba(%1,%2,%3,%4)").arg(color.red()).arg(color.green()).arg(color.blue()).arg(alpha);

		if (!bgColor.compare(alternateBgColor))
			alternateBgColor = QString("transparent");
	}

	if (config_file.readBoolEntry("Look", "UseUserboxBackground", true))
	{
		QString typeName = config_file.readEntry("Look", "UserboxBackgroundDisplayStyle");

		KaduTreeView::BackgroundMode type;
		if (typeName == "Centered")
			type = KaduTreeView::BackgroundCentered;
		else if (typeName == "Tiled")
			type = KaduTreeView::BackgroundTiled;
		else if (typeName == "Stretched")
			type = KaduTreeView::BackgroundStretched;
		else if (typeName == "TiledAndCentered")
			type = KaduTreeView::BackgroundTiledAndCentered;
		else
			type = KaduTreeView::BackgroundNone;

		BuddiesTree->setBackground(bgColor, alternateBgColor, config_file.readEntry("Look", "UserboxBackground"), type);
		ChatsTree->setBackground(bgColor, alternateBgColor, config_file.readEntry("Look", "UserboxBackground"), type);
	}
	else
	{
		BuddiesTree->setBackground(bgColor, alternateBgColor);
		ChatsTree->setBackground(bgColor, alternateBgColor);
	}

	triggerCompositingStateChanged();

	if (config_file.readBoolEntry("Look", "DisplayGroupTabs", true))
		GroupFilter->setAllGroupShown(config_file.readBoolEntry("Look", "ShowGroupAll", true));
	else
		GroupFilter->setAllGroupShown(true);
}

void RosterWidget::compositingEnabled()
{
	if (!config_file.readBoolEntry("Look", "UserboxTransparency"))
	{
		compositingDisabled();
		return;
	}

	if (CompositingEnabled)
		return;

	CompositingEnabled = true;
	GroupBar->setAutoFillBackground(true);
	BuddiesWidget->filterWidget()->setAutoFillBackground(true);
	BuddiesTree->verticalScrollBar()->setAutoFillBackground(true);

	// TODO: find a way to paint this QFrame outside its viewport still allowing the viewport to be transparent
	BuddiesTree->setFrameShape(QFrame::NoFrame);

	configurationUpdated();
}

void RosterWidget::compositingDisabled()
{
	if (!CompositingEnabled)
		return;

	CompositingEnabled = false;
	GroupBar->setAutoFillBackground(false);
	BuddiesWidget->filterWidget()->setAutoFillBackground(false);
	BuddiesTree->verticalScrollBar()->setAutoFillBackground(false);
	BuddiesTree->setFrameShape(QFrame::StyledPanel);

	configurationUpdated();
}

ModelChain * RosterWidget::createBuddiesModelChain()
{
	ModelChain *chain = new ModelChain(new BuddiesModel(this), this);
	ProxyModel = new BuddiesModelProxy(chain);
	ProxyModel->addFilter(new PendingMessagesFilter(ProxyModel));

	AnonymousWithoutMessagesBuddyFilter *anonymousFilter = new AnonymousWithoutMessagesBuddyFilter(ProxyModel);
	anonymousFilter->setEnabled(true);
	ProxyModel->addFilter(anonymousFilter);

	BuddyNameFilter *nameFilter = new BuddyNameFilter(ProxyModel);
	connect(BuddiesWidget, SIGNAL(filterChanged(QString)), nameFilter, SLOT(setName(QString)));
	ProxyModel->addFilter(nameFilter);

	GroupFilter = new GroupBuddyFilter(ProxyModel);
	connect(GroupBar, SIGNAL(currentGroupChanged(Group)), GroupFilter, SLOT(setGroup(Group)));
	ProxyModel->addFilter(GroupFilter);

	chain->addProxyModel(ProxyModel);

	return chain;
}

ModelChain * RosterWidget::createChatsModelChain()
{
	ModelChain *chain = new ModelChain(new ChatsModel(ChatsTree), ChatsTree);

	ChatsProxyModel *chatsProxyModel = new ChatsProxyModel(chain);

	ChatNamedFilter *chatNamedFilter = new ChatNamedFilter(chatsProxyModel);
	chatNamedFilter->setEnabled(true);
	chatsProxyModel->addFilter(chatNamedFilter);

	chain->addProxyModel(chatsProxyModel);

	return chain;
}

QWidget * RosterWidget::createBuddiesWidget(QWidget *parent)
{
	BuddiesWidget = new FilteredTreeView(FilteredTreeView::FilterAtTop, parent);
	BuddiesTree = new TalkableTreeView(BuddiesWidget);
	BuddiesWidget->setTreeView(BuddiesTree);

	BuddiesTree->useConfigurationColors(true);
	BuddiesTree->setChain(createBuddiesModelChain());
	BuddiesTree->setContextMenuEnabled(true);

	connect(BuddiesTree, SIGNAL(talkableActivated(Talkable)), this, SIGNAL(talkableActivated(Talkable)));
	connect(BuddiesTree, SIGNAL(currentChanged(Talkable)), this, SIGNAL(currentChanged(Talkable)));

	return BuddiesWidget;
}

QWidget * RosterWidget::createChatsWidget(QWidget *parent)
{
	ChatsTree = new TalkableTreeView(parent);
	ChatsTree->setContextMenuEnabled(true);
	ChatsTree->setChain(createChatsModelChain());

	connect(ChatsTree, SIGNAL(talkableActivated(Talkable)), this, SIGNAL(talkableActivated(Talkable)));
	connect(ChatsTree, SIGNAL(currentChanged(Talkable)), this, SIGNAL(currentChanged(Talkable)));

	return ChatsTree;
}

TalkableTreeView * RosterWidget::talkableTreeView()
{
	return BuddiesTree;
}

BuddiesModelProxy * RosterWidget::buddiesProxyModel()
{
	return ProxyModel;
}

ActionContext * RosterWidget::actionContext()
{
	return Context;
}

void RosterWidget::clearFilter()
{
	BuddiesWidget->filterWidget()->setFilter(QString());
}
