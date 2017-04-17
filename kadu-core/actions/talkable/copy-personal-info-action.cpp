/*
 * %kadu copyright begin%
 * Copyright 2016 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "copy-personal-info-action.h"
#include "copy-personal-info-action.moc"

#include "actions/action-context.h"
#include "actions/action.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "contacts/contact-set.h"
#include "parser/parser.h"

#include <QtGui/QClipboard>
#include <QtWidgets/QApplication>

CopyPersonalInfoAction::CopyPersonalInfoAction(QObject *parent)
        :   // using C++ initializers breaks Qt's lupdate
          ActionDescription(parent)
{
    setIcon(KaduIcon{"kadu_icons/copy-personal-info"});
    setName(QStringLiteral("copyPersonalInfoAction"));
    setText(tr("Copy Personal Info"));
    setType(ActionDescription::TypeUser);
}

CopyPersonalInfoAction::~CopyPersonalInfoAction()
{
}

void CopyPersonalInfoAction::setConfiguration(Configuration *configuration)
{
    m_configuration = configuration;
}

void CopyPersonalInfoAction::setParser(Parser *parser)
{
    m_parser = parser;
}

void CopyPersonalInfoAction::actionTriggered(QAction *sender, bool)
{
    auto *action = qobject_cast<Action *>(sender);
    if (!action)
        return;

    auto contacts = action->context()->contacts();

    auto infoList = QStringList{};
    auto defaultSyntax = m_parser->escape(tr("Contact:")) + " %a[ (%u)]\n[" + m_parser->escape(tr("First name:")) +
                         " %f\n][" + m_parser->escape(tr("Last name:")) + " %r\n][" + m_parser->escape(tr("Mobile:")) +
                         " %m\n]";
    auto copyPersonalDataSyntax =
        m_configuration->deprecatedApi()->readEntry("General", "CopyPersonalDataSyntax", defaultSyntax);
    for (auto const &contact : contacts)
        infoList.append(m_parser->parse(copyPersonalDataSyntax, Talkable{contact}, ParserEscape::NoEscape));

    auto info = infoList.join("\n");
    if (info.isEmpty())
        return;

    QApplication::clipboard()->setText(info, QClipboard::Selection);
    QApplication::clipboard()->setText(info, QClipboard::Clipboard);
}
