#ifndef GADU_IMAGES_MANAGER_H
#define GADU_IMAGES_MANAGER_H

#include <QDateTime>
#include <QMap>
#include <QPair>

#include "html_document.h"
#include "protocol.h"

class GaduImagesManager
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
		
	static QString loadingImageHtml(UinType uin, uint32_t size, uint32_t crc32);
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

extern GaduImagesManager gadu_images_manager;

#endif
