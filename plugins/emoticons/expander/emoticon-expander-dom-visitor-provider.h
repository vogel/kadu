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

#ifndef EMOTICON_EXPANDER_DOM_VISITOR_PROVIDER
#define EMOTICON_EXPANDER_DOM_VISITOR_PROVIDER

#include <QtCore/QObject>
#include <QtCore/QScopedPointer>

#include "dom/ignore-links-dom-visitor.h"

#include "configuration/emoticon-configuration.h"
#include "walker/emoticon-prefix-tree.h"

#include "dom/dom-visitor-provider.h"

/**
 * @addtogroup Emoticons
 * @{
 */

/**
 * @class EmoticonExpanderDomVisitorProvider
 * @short This DomVisitorProvider provides instance of EmoticonExpander with up-to-date configuration.
 * @author Rafał 'Vogel' Malinowski
 */
class EmoticonExpanderDomVisitorProvider : public QObject, public DomVisitorProvider
{
	Q_OBJECT

	EmoticonConfiguration Configuration;
	QScopedPointer<IgnoreLinksDomVisitor> LinksVisitor;
	QScopedPointer<EmoticonPrefixTree> Tree;

	/**
	 * @short Create new EmoticonExpander with updated EmoticonConfiguration.
	 * @author Rafał 'Vogel' Malinowski
	 */
	void rebuildExpander();

public:
	EmoticonExpanderDomVisitorProvider();
	virtual ~EmoticonExpanderDomVisitorProvider();

	virtual DomVisitor * provide() const;

	/**
	 * @short Update configuration of provided EmoticonExpander.
	 * @author Rafał 'Vogel' Malinowski
	 * @param configuration new configuration for provided EmoticonExpander
	 */
	void setConfiguration(const EmoticonConfiguration &configuration);

};

/**
 * @}
 */

#endif // EMOTICON_EXPANDER_DOM_VISITOR_PROVIDER
