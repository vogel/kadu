#ifndef FILE_TRANSFER_H
#define FILE_TRANSFER_H

#include "dcc.h"
#include "userlist.h"

#include <qdialog.h>
#include <qstring.h>
#include <qvaluelist.h>
#include <qmap.h>

class QLabel;
class QProgressBar;
class QVBoxLayout;

class FileTransferDialog : public QDialog
{
	Q_OBJECT

	public:
		enum TransferType {
			TRANSFER_TYPE_GET,
			TRANSFER_TYPE_SEND
		};

	private:
		static QMap<DccSocket*, FileTransferDialog*> Dialogs;
		DccSocket* Socket;
		int Type;
		QLabel* l_offset;
		QProgressBar* p_progress;
		QVBoxLayout* vlayout1;
		long long int prevPercent;
		QTime* time;
		int prevOffset;

	public:
		FileTransferDialog(DccSocket* socket, TransferType type);
		~FileTransferDialog();
		void printFileInfo();
		void updateFileInfo();
		static FileTransferDialog* bySocket(DccSocket* socket);
		static void destroyAll();

		bool dccFinished;		
};

class Chat;

class FileTransferManager : public QObject
{
	Q_OBJECT

	private:
		QValueList<UinType> direct;
		QMap<UinType, QValueList<QString> > pendingFiles;
		QString selectFile(DccSocket* socket);

		void handleCreatedChat(Chat *);
		void handleDestroyingChat(Chat *);

	private slots:
		void userboxMenuPopup();
		void sendFile();
		void kaduKeyPressed(QKeyEvent* e);
		void connectionBroken(DccSocket* socket);
		void dccError(DccSocket* socket);
		void needFileAccept(DccSocket* socket);
		void needFileInfo(DccSocket* socket);
		void noneEvent(DccSocket* socket);
		void dccDone(DccSocket* socket);
		void setState(DccSocket* socket);
		void socketDestroying(DccSocket* socket);

		void chatCreated(const UinsList &);
		void chatDestroying(const UinsList &);

		void fileDropped(const UinsList &, const QString &);

	public:
		FileTransferManager(QObject *parent=0, const char *name=0);
		virtual ~FileTransferManager();
	public slots:
		/**
			Inicjuje wysy³anie pliku do podanego odbiorcy.
		**/
		void sendFile(UinType receiver);
		
		void sendFile(UinType receiver, const QString &filename);
};

extern FileTransferManager* file_transfer_manager;

#endif
