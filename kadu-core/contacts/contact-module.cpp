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

#include "contact-module.h"

#include "contacts/contact-manager.h"
#include "contacts/contact-parser-tags.h"
#include "contacts/contact-storage.h"
#include "contacts/model/contact-data-extractor.h"

ContactModule::ContactModule()
{
    add_type<ContactDataExtractor>();
    add_type<ContactManager>();
    add_type<ContactParserTags>();
    add_type<ContactStorage>();
}

ContactModule::~ContactModule()
{
}
