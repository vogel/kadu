/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "emoticons-module.h"

#include "configuration/emoticon-configurator.h"
#include "emoticons-plugin-object.h"
#include "expander/emoticon-expander-dom-visitor-provider.h"
#include "gui/emoticon-clipboard-html-transformer.h"
#include "gui/emoticons-configuration-ui-handler.h"
#include "gui/insert-emoticon-action.h"

EmoticonsModule::EmoticonsModule()
{
    add_type<EmoticonClipboardHtmlTransformer>();
    add_type<EmoticonConfigurator>();
    add_type<EmoticonExpanderDomVisitorProvider>();
    add_type<EmoticonsConfigurationUiHandler>();
    add_type<EmoticonsPluginObject>();
    add_type<InsertEmoticonAction>();
}
