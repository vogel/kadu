#ifndef FILE_TRANSFER_H
#define FILE_TRANSFER_H

#include "dcc.h"

#include <qdialog.h>
#include <qlayout.h> 
#include <qprogressbar.h>
#include <qlabel.h>

class DccFileDialog : public QDialog
{
	Q_OBJECT

	public:
		enum TransferType {
			TRANSFER_TYPE_GET,
			TRANSFER_TYPE_SEND
		};

	protected:
		DccSocket* dccsocket;
		QLabel *l_offset;
		QProgressBar *p_progress;
		QVBoxLayout* vlayout1;
		long long int prevPercent;
		QTime *time;
		int prevOffset;
		int type;
		void closeEvent(QCloseEvent *e);

	public:
		DccFileDialog(DccSocket* dccsocket, TransferType type, QDialog* parent=NULL, const char* name=NULL);
		~DccFileDialog();
		void printFileInfo(struct gg_dcc* dccsock);
		void updateFileInfo(struct gg_dcc* dccsock);

		bool dccFinished;		
};

class FileTransferManager : public QObject
{
	Q_OBJECT

	private:
		/**
			Przechowuje informacje o wys³anych
			request'ach CTCP. Je¶li kto¶ jest
			za nat'em i chcemy wys³aæ mu plik
			to wysy³amy pro¶bê o po³±czenie a
			jego numer uin jest zapamiêtywany.
		**/
		QMap<UinType, bool> Requests;
		QMap<DccSocket*, DccFileDialog*> FileDialogs;
		QString selectFile(DccSocket* socket);

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
		void callbackReceived(DccSocket* socket);
		void setState(DccSocket* socket);
		void socketDestroying(DccSocket* socket);

	public:
		FileTransferManager();
		virtual ~FileTransferManager();
};

extern FileTransferManager* file_transfer_manager;

#endif
