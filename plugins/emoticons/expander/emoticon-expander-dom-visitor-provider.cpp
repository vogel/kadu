/*
 * %kadu copyright begin%
 * Copyright 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "dom/ignore-links-dom-visitor.h"
#include "debug.h"

#include "expander/animated-emoticon-path-provider.h"
#include "expander/emoticon-expander.h"
#include "expander/static-emoticon-path-provider.h"
#include "walker/emoticon-prefix-tree-builder.h"

#include "emoticon-expander-dom-visitor-provider.h"

EmoticonExpanderDomVisitorProvider::EmoticonExpanderDomVisitorProvider()
{
}

EmoticonExpanderDomVisitorProvider::~EmoticonExpanderDomVisitorProvider()
{
}

void EmoticonExpanderDomVisitorProvider::rebuildExpander()
{
	if (!Tree)
	{
		LinksVisitor.reset();
		return;
	}

	EmoticonPathProvider *emoticonPathProvider = Configuration.animate()
			? static_cast<EmoticonPathProvider *>(new AnimatedEmoticonPathProvider())
			: static_cast<EmoticonPathProvider *>(new StaticEmoticonPathProvider());
	LinksVisitor.reset(new IgnoreLinksDomVisitor(new EmoticonExpander(Tree.data(), emoticonPathProvider)));
}

DomVisitor * EmoticonExpanderDomVisitorProvider::provide() const
{
	return LinksVisitor.data();
}

void EmoticonExpanderDomVisitorProvider::setConfiguration(const EmoticonConfiguration &configuration)
{
	Configuration = configuration;

	if (!Configuration.emoticonTheme().aliases().isEmpty())
	{
		EmoticonPrefixTreeBuilder builder;
		foreach (const Emoticon &emoticon, Configuration.emoticonTheme().aliases())
			builder.addEmoticon(emoticon);

		Tree.reset(builder.tree());
	}
	else
		Tree.reset(0);

	rebuildExpander();
}

#include "moc_emoticon-expander-dom-visitor-provider.cpp"
