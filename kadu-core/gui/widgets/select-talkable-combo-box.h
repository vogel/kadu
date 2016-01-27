/*
 * %kadu copyright begin%
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

#pragma once

#include "talkable/talkable.h"
#include "exports.h"

#include "gui/widgets/actions-combo-box.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class HideAnonymousTalkableFilter;
class InjectedFactory;
class ModelChain;
class SelectTalkablePopup;
class TalkableFilter;
class TalkableProxyModel;

class KADUAPI SelectTalkableComboBox : public ActionsComboBox
{
	Q_OBJECT

	QPointer<InjectedFactory> m_injectedFactory;

	HideAnonymousTalkableFilter *HideAnonymousFilter;
	ModelChain *Chain;
	SelectTalkablePopup *Popup;
	TalkableProxyModel *ProxyModel;

private slots:
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_SET void init();

protected:
	InjectedFactory * injectedFactory() const;

	virtual void showPopup();
	virtual void hidePopup();

public:
	explicit SelectTalkableComboBox(QWidget *parent = nullptr);
	virtual ~SelectTalkableComboBox();

	void setBaseModel(QAbstractItemModel *model);
	void setShowAnonymous(bool showAnonymous);

	Talkable currentTalkable() const;

	void addFilter(TalkableFilter *filter);
	void removeFilter(TalkableFilter *filter);

public slots:
	void setCurrentTalkable(const Talkable &talkable);

};
