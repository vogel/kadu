#ifndef GADU_IMAGES_MANAGER_H
#define GADU_IMAGES_MANAGER_H

#include <QtCore/QDateTime>
#include <QtCore/QMap>
#include <QtCore/QPair>

#include "html_document.h"
#include "protocol.h"

#include "exports.h"

class KADUAPI GaduImagesManager
{
	struct ImageToSend
	{
		quint32 size;
		quint32 crc32;
		QString file_name;
		QDateTime lastSent;
		char *data;
		ImageToSend();
	};
	QMap<QPair<quint32, quint32>, ImageToSend> ImagesToSend;//size,crc32 -> 

	struct SavedImage
	{
		quint32 size;
		quint32 crc32;
		QString file_name;
		SavedImage();
	};
	QList<SavedImage> SavedImages;

public:
	GaduImagesManager();
	static void setBackgroundsForAnimatedImages(HtmlDocument &doc, const QColor &col);
		
	static QString loadingImageHtml(UinType uin, quint32 size, quint32 crc32);
	static QString imageHtml(const QString &file_name);
	void addImageToSend(const QString &file_name, quint32 &size, quint32 &crc32);
	void sendImage(UinType uin, quint32 size, quint32 crc32);

	/**
		Szuka zakolejkowanego obrazka i zwraca jego nazw� pliku
		Zwraca ci�g pusty, je�li obrazek nie zosta� w tej sesji
		zakolejkowany do wys�ania.
	**/
	QString getImageToSendFileName(quint32 size, quint32 crc32);

	/**
		Zapisuje obrazek w katalogu .gg/images.
		Zwraca pe�n� �cie�k� do zapisanego obrazka.
	**/
	QString saveImage(UinType sender, quint32 size, quint32 crc32, const QString &filename, const char *data);

	/**
		Szuka zapisanego obrazka i zwraca jego nazw� pliku
		wraz ze �cie�k�. Zwraca ci�g pusty, je�li obrazek
		nie zosta� w tej sesji zapisany.
	**/
	QString getSavedImageFileName(quint32 size, quint32 crc32);

	QString replaceLoadingImages(const QString& text, UinType sender, quint32 size, quint32 crc32);

};

extern KADUAPI GaduImagesManager gadu_images_manager;

#endif
