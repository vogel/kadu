/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GADU_IMAGES_MANAGER_H
#define GADU_IMAGES_MANAGER_H

#include <QtCore/QDateTime>
#include <QtCore/QMap>
#include <QtCore/QPair>

#include "html_document.h"
#include "protocols/protocol.h"

#include "gadu-exports.h"

class GADUAPI GaduImagesManager
{
	struct ImageToSend
	{
		uint32_t size;
		uint32_t crc32;
		QString file_name;
		QDateTime lastSent;
		char *data;
		ImageToSend();
	};
	QMap<QPair<uint32_t, uint32_t>, ImageToSend> ImagesToSend;//size,crc32 -> 

	struct SavedImage
	{
		uint32_t size;
		uint32_t crc32;
		QString file_name;
		SavedImage();
	};
	QList<SavedImage> SavedImages;

public:
	GaduImagesManager();
	static void setBackgroundsForAnimatedImages(HtmlDocument &doc, const QColor &col);
		
	static QString imageHtml(const QString &file_name);
	void addImageToSend(const QString &file_name, uint32_t &size, uint32_t &crc32);
	void sendImage(UinType uin, uint32_t size, uint32_t crc32);

	/**
		Szuka zakolejkowanego obrazka i zwraca jego nazw� pliku
		Zwraca ci�g pusty, je�li obrazek nie zosta� w tej sesji
		zakolejkowany do wys�ania.
	**/
	QString getImageToSendFileName(uint32_t size, uint32_t crc32);

	/**
		Zapisuje obrazek w katalogu .gg/images.
		Zwraca pe�n� �cie�k� do zapisanego obrazka.
	**/
	QString saveImage(UinType sender, uint32_t size, uint32_t crc32, const QString &filename, const char *data);

	/**
		Szuka zapisanego obrazka i zwraca jego nazw� pliku
		wraz ze �cie�k�. Zwraca ci�g pusty, je�li obrazek
		nie zosta� w tej sesji zapisany.
	**/
	QString getSavedImageFileName(uint32_t size, uint32_t crc32);

	QString replaceLoadingImages(const QString& text, UinType sender, uint32_t size, uint32_t crc32);

};

extern GADUAPI GaduImagesManager gadu_images_manager;

#endif
