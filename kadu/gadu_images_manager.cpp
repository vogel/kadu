/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qregexp.h>

#include "config_file.h"
#include "debug.h"
#include "gadu_images_manager.h"
#include "icons_manager.h"
#include "misc.h"

void GaduImagesManager::setBackgroundsForAnimatedImages(HtmlDocument &doc, const QColor &col)
{
	static QRegExp animRE("<img bgcolor=\"\" animated=\"1\"");
	QString animText = QString("<img bgcolor=\"%1\" animated=\"1\"").arg(col.name());
	for(int i = 0; i < doc.countElements(); ++i)
	{
		if (!doc.isTagElement(i))
			continue;

		QString text = doc.elementText(i);
		text.replace(animRE, animText);
//		kdebugm(KDEBUG_WARNING, ">>%s\n", text.local8Bit().data());
		doc.setElementValue(i, text, true);
	}
}

QString GaduImagesManager::imageHtml(const QString& file_name)
{
	if (file_name.right(4).lower()==".gif")
		return narg(QString("<img bgcolor=\"\" animated=\"1\" src=\"%1\" title=\"%2\"/>"), file_name, file_name);
	else
		return QString("<img src=\"%1\"/>").arg(file_name);
}

QString GaduImagesManager::loadingImageHtml(UinType uin, uint32_t size, uint32_t crc32)
{
	return narg(QString("<img src=\"%1\" gg_sender=\"%2\" gg_size=\"%3\" gg_crc=\"%4\"/>"),
		icons_manager->iconPath("LoadingImage"), QString::number(uin), QString::number(size), QString::number(crc32));
}

void GaduImagesManager::addImageToSend(const QString& file_name, uint32_t& size, uint32_t& crc32)
{
	kdebugf();
	ImageToSend img;
	QFile f(file_name);
	kdebugm(KDEBUG_INFO, "Opening file \"%s\"\n", file_name.local8Bit().data());
	if (!f.open(IO_ReadOnly))
	{
		kdebugm(KDEBUG_ERROR, "Error opening file\n");
		return;
	}
	img.size = f.size();
	img.file_name = file_name;
	img.data = new char[img.size];
	kdebugm(KDEBUG_INFO, "Reading file\n");
	unsigned int ret = f.readBlock(img.data, img.size);
	if (ret != img.size)
		kdebugm(KDEBUG_ERROR, "ret:%d != %d:img.size\n", ret, img.size);
	f.close();
	img.crc32 = gg_crc32(0, (const unsigned char*)img.data, img.size);
	kdebugm(KDEBUG_INFO, "Inserting into images to send: filename=%s, size=%i, crc32=%i\n\n", img.file_name.local8Bit().data(), img.size, img.crc32);
	size = img.size;
	crc32 = img.crc32;
	ImagesToSend[qMakePair(size, crc32)] = img;
	kdebugf2();
}

void GaduImagesManager::sendImage(UinType uin, uint32_t size, uint32_t crc32)
{
	kdebugf();
	kdebugm(KDEBUG_INFO, "Searching images to send: size=%u, crc32=%u, uin=%d\n", size, crc32, uin);
	if (ImagesToSend.contains(qMakePair(size, crc32)))
	{
		ImageToSend &i = ImagesToSend[qMakePair(size, crc32)];
		if (!i.data)
		{
			QFile f(i.file_name);
			if (!f.open(IO_ReadOnly))
			{
				kdebugm(KDEBUG_ERROR, "Error opening file\n");
				return;
			}
			i.data = new char[i.size];
			kdebugm(KDEBUG_INFO, "Reading file\n");
			unsigned int ret = f.readBlock(i.data, i.size);
			if (ret != i.size)
				kdebugm(KDEBUG_ERROR, "ret:%d != %d:img.size\n", ret, i.size);
			f.close();
		}

		gadu->sendImage(userlist->byID("Gadu", QString::number(uin)), i.file_name, i.size, i.data);
		delete[] i.data;
		i.data = NULL;
		i.lastSent = QDateTime::currentDateTime(); // to pole wykorzysta siê przy zapisywaniu listy obrazków do pliku, stare obrazki bêd± usuwane
	}
	else
		kdebugm(KDEBUG_WARNING, "Image data not found\n");
}

QString GaduImagesManager::saveImage(UinType sender, uint32_t size, uint32_t crc32, const QString& filename, const char* data)
{
	kdebugf();
	QString path = ggPath("images");
	kdebugm(KDEBUG_INFO, "Creating directory: %s\n",path.local8Bit().data());
	QDir().mkdir(path);
	QString file_name = QString("%1-%2-%3-%4").arg(sender).arg(size).arg(crc32).arg(filename);
	kdebugm(KDEBUG_INFO, "Saving image as file: %s\n",file_name.local8Bit().data());
	SavedImage img;
	img.size = size;
	img.crc32 = crc32;
	img.file_name = path + "/" + file_name;
	QFile f(img.file_name);
	f.open(IO_WriteOnly);
	f.writeBlock(data,size);
	f.close();
	SavedImages.append(img);
	kdebugf2();
	return img.file_name;
}

QString GaduImagesManager::getImageToSendFileName(uint32_t size, uint32_t crc32)
{
	kdebugf();
	kdebugm(KDEBUG_INFO, "Searching images to send: size=%u, crc32=%u\n",size,crc32);
	CONST_FOREACH(i, ImagesToSend)
	{
		if ((*i).size==size && (*i).crc32==crc32)
		{
			kdebugm(KDEBUG_INFO, "Image data found\n");
			return (*i).file_name;
		}
	}
	kdebugm(KDEBUG_WARNING, "Image data not found\n");
	return "";
}

QString GaduImagesManager::getSavedImageFileName(uint32_t size, uint32_t crc32)
{
	kdebugf();
	kdebugm(KDEBUG_INFO, "Searching saved images: size=%u, crc32=%u\n",size,crc32);
	CONST_FOREACH(i, SavedImages)
	{
		if ((*i).size==size && (*i).crc32==crc32)
		{
			kdebugm(KDEBUG_INFO, "Image data found\n");
			return (*i).file_name;
		}
	}
	kdebugm(KDEBUG_WARNING, "Image data not found\n");
	return "";
}

QString GaduImagesManager::replaceLoadingImages(const QString& text, UinType sender, uint32_t size, uint32_t crc32)
{
	kdebugf();
	QString loading_string = GaduImagesManager::loadingImageHtml(sender,size,crc32);
	QString image_string;
	QString new_text = text;
	int pos;

	QString file_name = getSavedImageFileName(size,crc32);
	if (file_name.right(4).lower()==".gif")
		image_string = narg(QString("<img bgcolor=\"%1\" animated=\"1\" src=\"%2\" title=\"%3\"/>"),
			config_file.readColorEntry("Look","ChatUsrBgColor").name(), file_name, file_name);
	else
		image_string = QString("<img src=\"%1\"/>").arg(file_name);

	while ((pos = new_text.find(loading_string)) != -1)
	{
		kdebugm(KDEBUG_INFO, "Found coresponding loading image at pos %i, replacing\n",pos);
		new_text = new_text.replace(pos, loading_string.length(), image_string);
	}
	kdebugf2();
	return new_text;
}

GaduImagesManager gadu_images_manager;
