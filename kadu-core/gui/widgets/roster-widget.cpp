/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "roster-widget.h"

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/injected-factory.h"
#include "gui/widgets/filter-widget.h"
#include "gui/widgets/filtered-tree-view.h"
#include "gui/widgets/group-tab-bar/group-tab-bar-configurator.h"
#include "gui/widgets/group-tab-bar/group-tab-bar.h"
#include "gui/widgets/talkable-tree-view.h"
#include "gui/windows/proxy-action-context.h"
#include "model/model-chain.h"
#include "talkable/filter/group-talkable-filter.h"
#include "talkable/filter/hide-anonymous-talkable-filter.h"
#include "talkable/filter/hide-contact-chats-talkable-filter.h"
#include "talkable/filter/hide-temporary-talkable-filter.h"
#include "talkable/filter/name-talkable-filter.h"
#include "talkable/filter/unread-messages-talkable-filter.h"
#include "talkable/model/talkable-model-factory.h"
#include "talkable/model/talkable-proxy-model.h"

#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QStackedWidget>

RosterWidget::RosterWidget(QWidget *parent) :
	QWidget(parent), CompositingEnabled(false)
{
}

RosterWidget::~RosterWidget()
{
	storeConfiguration();

	delete Context;
	Context = 0;
}

void RosterWidget::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void RosterWidget::setInjectedFactory(InjectedFactory *injectedFactory)
{
	m_injectedFactory = injectedFactory;
}

void RosterWidget::init()
{
	Context = new ProxyActionContext();
	createGui();

	Context->setForwardActionContext(TalkableTree->actionContext());
	MyGroupFilter->setGroupFilter(GroupBar->groupFilter());

	configurationUpdated();
}

void RosterWidget::createGui()
{
	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(0);

	GroupBar = m_injectedFactory->makeInjected<GroupTabBar>(this);
	connect(GroupBar, SIGNAL(currentChanged(int)), this, SLOT(storeConfiguration()));
	connect(GroupBar, SIGNAL(tabMoved(int,int)), this, SLOT(storeConfiguration()));

	TabBarConfigurator.reset(m_injectedFactory->makeInjected<GroupTabBarConfigurator>());
	TabBarConfigurator->setGroupTabBar(GroupBar);

	createTalkableWidget(this);

	layout->addWidget(GroupBar);
	layout->addWidget(TalkableWidget);

	layout->setStretchFactor(GroupBar, 1);
	layout->setStretchFactor(TalkableWidget, 100);

	setFocusProxy(TalkableWidget);
}

void RosterWidget::configurationUpdated()
{
	QString bgColor = m_configuration->deprecatedApi()->readColorEntry("Look","UserboxBgColor").name();
	QString alternateBgColor = m_configuration->deprecatedApi()->readColorEntry("Look","UserboxAlternateBgColor").name();

	if (CompositingEnabled && m_configuration->deprecatedApi()->readBoolEntry("Look", "UserboxTransparency"))
	{
		int alpha = m_configuration->deprecatedApi()->readNumEntry("Look", "UserboxAlpha");

		QColor color(bgColor);
		bgColor = QString("rgba(%1,%2,%3,%4)").arg(color.red()).arg(color.green()).arg(color.blue()).arg(alpha);

		color = QColor(alternateBgColor);
		alternateBgColor = QString("rgba(%1,%2,%3,%4)").arg(color.red()).arg(color.green()).arg(color.blue()).arg(alpha);

		if (!bgColor.compare(alternateBgColor))
			alternateBgColor = QString("transparent");
	}

	if (m_configuration->deprecatedApi()->readBoolEntry("Look", "UseUserboxBackground", true))
	{
		QString typeName = m_configuration->deprecatedApi()->readEntry("Look", "UserboxBackgroundDisplayStyle");

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

		TalkableTree->setBackground(bgColor, alternateBgColor, m_configuration->deprecatedApi()->readEntry("Look", "UserboxBackground"), type);
	}
	else
	{
		TalkableTree->setBackground(bgColor, alternateBgColor);
	}

	triggerCompositingStateChanged();
}

void RosterWidget::storeConfiguration()
{
	if (TabBarConfigurator)
		TabBarConfigurator->storeConfiguration();
}

void RosterWidget::compositingEnabled()
{
	if (!m_configuration->deprecatedApi()->readBoolEntry("Look", "UserboxTransparency"))
	{
		compositingDisabled();
		return;
	}

	if (CompositingEnabled)
		return;

	CompositingEnabled = true;
	GroupBar->setAutoFillBackground(true);
	TalkableWidget->filterWidget()->setAutoFillBackground(true);
	TalkableTree->verticalScrollBar()->setAutoFillBackground(true);

	// TODO: find a way to paint this QFrame outside its viewport still allowing the viewport to be transparent
	TalkableTree->setFrameShape(QFrame::NoFrame);

	configurationUpdated();
}

void RosterWidget::compositingDisabled()
{
	if (!CompositingEnabled)
		return;

	CompositingEnabled = false;
	GroupBar->setAutoFillBackground(false);
	TalkableWidget->filterWidget()->setAutoFillBackground(false);
	TalkableTree->verticalScrollBar()->setAutoFillBackground(false);

	TalkableTree->setFrameShape(QFrame::StyledPanel);

	configurationUpdated();
}

ModelChain * RosterWidget::createModelChain()
{
	ModelChain *chain = new ModelChain(TalkableTree);
	chain->setBaseModel(TalkableModelFactory::createInstance(chain));

	ProxyModel = new TalkableProxyModel(chain);
	ProxyModel->addFilter(new HideContactChatsTalkableFilter(ProxyModel));
	ProxyModel->addFilter(new HideTemporaryTalkableFilter(ProxyModel));
	ProxyModel->addFilter(new UnreadMessagesTalkableFilter(ProxyModel));
	ProxyModel->addFilter(new HideAnonymousTalkableFilter(ProxyModel));

	NameTalkableFilter *nameTalkableFilter = new NameTalkableFilter(NameTalkableFilter::AcceptMatching, ProxyModel);
	connect(TalkableWidget, SIGNAL(filterChanged(QString)), nameTalkableFilter, SLOT(setName(QString)));
	ProxyModel->addFilter(nameTalkableFilter);

	MyGroupFilter = new GroupTalkableFilter(ProxyModel);
	connect(GroupBar, SIGNAL(currentGroupFilterChanged(GroupFilter)), MyGroupFilter, SLOT(setGroupFilter(GroupFilter)));
	ProxyModel->addFilter(MyGroupFilter);

	chain->addProxyModel(ProxyModel);

	return chain;
}

void RosterWidget::createTalkableWidget(QWidget *parent)
{
	TalkableWidget = new FilteredTreeView(FilteredTreeView::FilterAtTop, parent);

	TalkableTree = m_injectedFactory->makeInjected<TalkableTreeView>(TalkableWidget);
	TalkableTree->setUseConfigurationColors(true);
	TalkableTree->setContextMenuEnabled(true);
	TalkableTree->setChain(createModelChain());

	connect(TalkableTree, SIGNAL(talkableActivated(Talkable)), this, SIGNAL(talkableActivated(Talkable)));
	connect(TalkableTree, SIGNAL(currentChanged(Talkable)), this, SIGNAL(currentChanged(Talkable)));

	TalkableWidget->setView(TalkableTree);
}

TalkableTreeView * RosterWidget::talkableTreeView()
{
	return TalkableTree;
}

TalkableProxyModel * RosterWidget::talkableProxyModel()
{
	return ProxyModel;
}

ActionContext * RosterWidget::actionContext()
{
	return Context;
}

void RosterWidget::clearFilter()
{
	TalkableWidget->filterWidget()->setFilter(QString());
}

#include "moc_roster-widget.cpp"
