/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "debug.h"

#include "emoticon-expander-dom-visitor-provider.h"
#include "emoticon-path-provider.h"
#include "animated-emoticon-path-provider.h"
#include "static-emoticon-path-provider.h"
#include "emoticon-expander.h"
#include <dom/ignore-links-dom-visitor.h>

EmoticonExpanderDomVisitorProvider::EmoticonExpanderDomVisitorProvider() :
		Style(EmoticonsStyleNone)
{
}

EmoticonExpanderDomVisitorProvider::~EmoticonExpanderDomVisitorProvider()
{
}

void EmoticonExpanderDomVisitorProvider::rebuildVisitor()
{
	if (EmoticonsStyleNone == Style || !Tree)
	{
		Expander.reset(0);
		LinksVisitor.reset(0);
		return;
	}

	EmoticonPathProvider *emoticonPathProvider = Style == EmoticonsStyleAnimated
			? static_cast<EmoticonPathProvider *>(new AnimatedEmoticonPathProvider())
			: static_cast<EmoticonPathProvider *>(new StaticEmoticonPathProvider());
	Expander.reset(new EmoticonExpander(Tree.data(), emoticonPathProvider));
	LinksVisitor.reset(new IgnoreLinksDomVisitor(Expander.data()));
}

DomVisitor * EmoticonExpanderDomVisitorProvider::provide() const
{
	return LinksVisitor.data();
}

void EmoticonExpanderDomVisitorProvider::setEmoticonTree(EmoticonPrefixTree *tree)
{
	Tree.reset(tree);
	rebuildVisitor();
}

void EmoticonExpanderDomVisitorProvider::setStyle(EmoticonsStyle style)
{
	Style = style;
	rebuildVisitor();
}
