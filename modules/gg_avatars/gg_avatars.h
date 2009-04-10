#ifndef GG_AVATARS_H
#define GG_AVATARS_H

#include <QtCore/QObject>
#include <QtCore/QEventLoop>

#include "action.h"
#include "configuration_aware_object.h"
#include "main_configuration_window.h"

class QFile;
class QHttp;
class QBuffer;

class GaduAvatars : public ConfigurationUiHandler, ConfigurationAwareObject
{
	Q_OBJECT
	private:
		QMap<int, QBuffer *> buffers;   /* buffers per link request */
		QMap<int, int> uins;		/* uins per link/file request */
		QMap<int, QFile *> files;	/* files per file request */
		QHttp *linkDownloader;		/* downloader for avatar links */
		QHttp *fileDownloader;		/* downloader for avatar images */
		ActionDescription *avatarActionDescription;

	public:
		GaduAvatars();
		~GaduAvatars();
		QString getAvatar(int uin);
		virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

	protected:
		virtual void configurationUpdated();

	private slots:
		void gotResponse(int id, bool error);
		void fileDownloaded(int id, bool error);
		void refreshAvatarActionActivated(QAction *sender, bool toggled);
};

extern GaduAvatars *gaduAvatars;
#endif
