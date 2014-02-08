/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "formatted-string-gadu-html-visitor.h"

#include "formatted-string/formatted-string-image-block.h"
#include "misc/memory.h"
#include "protocols/services/chat-image-service.h"
#include "services/image-storage-service.h"

#include <QtCore/QFile>

FormattedStringGaduHtmlVisitor::FormattedStringGaduHtmlVisitor(ChatImageService *chatImageService, ImageStorageService *imageStorageService) :
		m_chatImageService(chatImageService), m_imageStorageService(imageStorageService)
{
}

FormattedStringGaduHtmlVisitor::~FormattedStringGaduHtmlVisitor()
{
}

void FormattedStringGaduHtmlVisitor::visit(const FormattedStringImageBlock * const formattedStringImageBlock)
{
	if (!m_chatImageService || !m_imageStorageService)
		return FormattedStringHtmlVisitor::visit(formattedStringImageBlock);

	auto imagePath = m_imageStorageService->fullPath(formattedStringImageBlock->imagePath());
	QFile imageFile{imagePath};
	if (!imageFile.open(QFile::ReadOnly))
		return;

	auto content = imageFile.readAll();
	auto image = m_chatImageService->prepareImageToBeSent(content);
	imageFile.close();

	append(QString{"<img name=\"%1\">"}.arg(image.key()));
}

void FormattedStringGaduHtmlVisitor::visit(const FormattedStringTextBlock * const formattedStringTextBlock)
{
	FormattedStringHtmlVisitor::visit(formattedStringTextBlock);
}
