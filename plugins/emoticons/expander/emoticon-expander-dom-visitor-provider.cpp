/*
 * %kadu copyright begin%
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "emoticon-expander-dom-visitor-provider.h"

#include "expander/animated-emoticon-path-provider.h"
#include "expander/emoticon-expander.h"
#include "expander/static-emoticon-path-provider.h"
#include "walker/emoticon-prefix-tree-builder.h"

#include "dom/ignore-links-dom-visitor.h"
#include "misc/memory.h"
#include "debug.h"

EmoticonExpanderDomVisitorProvider::EmoticonExpanderDomVisitorProvider(QObject *parent) :
		QObject{parent}
{
}

EmoticonExpanderDomVisitorProvider::~EmoticonExpanderDomVisitorProvider()
{
}

void EmoticonExpanderDomVisitorProvider::rebuildExpander()
{
	if (!m_tree)
	{
		m_ignoreLinksVisitor = nullptr;
		return;
	}

	m_ignoreLinksVisitor = m_configuration.animate() 
			? make_unique<IgnoreLinksDomVisitor>(make_unique<EmoticonExpander>(m_tree.get(), make_unique<AnimatedEmoticonPathProvider>()))
			: make_unique<IgnoreLinksDomVisitor>(make_unique<EmoticonExpander>(m_tree.get(), make_unique<StaticEmoticonPathProvider>()));
}

const DomVisitor * EmoticonExpanderDomVisitorProvider::provide() const
{
	return m_ignoreLinksVisitor.get();
}

void EmoticonExpanderDomVisitorProvider::setConfiguration(const EmoticonConfiguration &configuration)
{
	m_configuration = configuration;

	if (!m_configuration.emoticonTheme().aliases().isEmpty())
	{
		EmoticonPrefixTreeBuilder builder;
		foreach (const Emoticon &emoticon, m_configuration.emoticonTheme().aliases())
			builder.addEmoticon(emoticon);

		m_tree.reset(builder.tree());
	}
	else
		m_tree.reset();

	rebuildExpander();
}

#include "moc_emoticon-expander-dom-visitor-provider.cpp"
