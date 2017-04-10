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

#include "configuration/emoticon-configuration.h"
#include "walker/emoticon-prefix-tree.h"

#include "dom/dom-visitor-provider.h"

#include <QtCore/QObject>
#include <memory>

class IgnoreLinksDomVisitor;

/**
 * @addtogroup Emoticons
 * @{
 */

/**
 * @class EmoticonExpanderDomVisitorProvider
 * @short This DomVisitorProvider provides instance of EmoticonExpander with up-to-date configuration.
 */
class EmoticonExpanderDomVisitorProvider : public QObject, public DomVisitorProvider
{
    Q_OBJECT

public:
    Q_INVOKABLE EmoticonExpanderDomVisitorProvider(QObject *parent = nullptr);
    virtual ~EmoticonExpanderDomVisitorProvider();

    virtual const DomVisitor *provide() const;

    /**
     * @short Update configuration of provided EmoticonExpander.
     * @param configuration new configuration for provided EmoticonExpander
     */
    void setConfiguration(const EmoticonConfiguration &configuration);

private:
    EmoticonConfiguration m_configuration;
    std::unique_ptr<IgnoreLinksDomVisitor> m_ignoreLinksVisitor;
    std::unique_ptr<EmoticonPrefixTree> m_tree;

    /**
     * @short Create new EmoticonExpander with updated EmoticonConfiguration.
     */
    void rebuildExpander();
};

/**
 * @}
 */
