#ifndef FILE_TRANSFER_H
#define FILE_TRANSFER_H

#include "dcc.h"
#include "userlist.h"

#include <qdialog.h>
#include <qstring.h>
#include <qvaluelist.h>
#include <qmap.h>
#include <qpopupmenu.h>
#include <qdom.h>
#include <qvbox.h>
#include <qlistview.h>

class QLabel;
class QProgressBar;
class QScrollView;
class QVBoxLayout;
class FileTransferManager;

/** @ingroup dcc
 * @{
 */
class FileTransfer : public QObject
{
	Q_OBJECT

	friend class FileTransferManager;

	public:
		enum FileTransferType {
			TypeSend,
			TypeReceive
		};

		enum FileTransferStatus {
			StatusFrozen,
			StatusWaitForConnection,
			StatusTransfer,
			StatusFinished
		};

		enum FileTransferError {
			ErrorDccDisabled,
			ErrorDccSocketTransfer,
			ErrorConnectionTimeout,
			ErrorDccTooManyConnections
		};

		enum StartType {
			StartNew,
			StartRestore
		};

		enum StopType {
			StopTemporary,
			StopFinally
		};

		enum FileNameType {
			FileNameFull,
			FileNameGadu
		};

	private:
		static QMap<DccSocket*, FileTransfer*> Transfers;

	public:
		static QValueList<FileTransfer *> AllTransfers;

	private:
		QObject *mainListener;
		QValueList<QPair<QObject *, bool> > listeners;

		DccSocket* Socket;
		FileTransferType Type;
		FileTransferStatus Status;

		UinType Contact;
		QString FileName;
		QString GaduFileName;

		QTimer* connectionTimeoutTimer;
		QTimer* updateFileInfoTimer;

		long long int FileSize;
		long long int TransferedSize;
		long long int PrevTransferedSize;
		long int Speed;

		bool dccFinished;
		bool direct;

		void connectSignals(QObject *, bool);
		void disconnectSignals(QObject *, bool);

		void socketDestroying();

	private slots:
		void connectionTimeout();

		void prepareFileInfo();
		void updateFileInfo();

	public:
		FileTransfer(FileTransferManager *listener, FileTransferType type, const UinType &contact,
			const QString &fileName);
		~FileTransfer();

		void addListener(QObject * const listener, bool listenerHasSlots);
		void removeListener(QObject * const listener, bool listenerHasSlots);

		void start(StartType startType = StartNew);
		void stop(StopType stopType = StopTemporary);

		void setSocket(DccSocket* Socket);

		static FileTransfer * bySocket(DccSocket* socket);
		static FileTransfer * byUin(UinType);
		static FileTransfer * byUinAndStatus(UinType, FileTransferStatus);
		static FileTransfer * search(FileTransferType type, const UinType &contact, const QString &fileName,
			FileNameType fileNameType = FileNameFull);
		static void destroyAll();

		QDomElement toDomElement(const QDomElement &root);
		static FileTransfer * fromDomElement(const QDomElement &dom, FileTransferManager *listener);

		FileTransferType type();
		FileTransferStatus status();

		UinType contact();
		QString fileName();

		int percent();
		long int speed();
		long long int fileSize();
		long long int transferedSize();

		void finished(bool successfull);

		void needFileInfo();
		void connectionBroken();
		void dccError();

	signals:
		void newFileTransfer(FileTransfer *);
		void fileTransferFailed(FileTransfer *, FileTransfer::FileTransferError);
		void fileTransferStatusChanged(FileTransfer *);
		void fileTransferFinished(FileTransfer *, bool);
		void fileTransferDestroying(FileTransfer *);
};

class FileTransferWindow;

class FileTransferWidget : public QFrame
{
	Q_OBJECT

	private:
		FileTransfer *ft;

		QLabel *description;
		QLabel *status;
		QProgressBar *progress;
		QPushButton *pauseButton;
		QPushButton *continueButton;

	private slots:
		void remove();
		void pauseTransfer();
		void continueTransfer();

	public:
		FileTransferWidget(QWidget *parent = 0, FileTransfer * = 0);
		virtual ~FileTransferWidget();

		FileTransfer *fileTransfer();

	public slots:
		void newFileTransfer(FileTransfer *);
		void fileTransferFailed(FileTransfer *, FileTransfer::FileTransferError);
		void fileTransferStatusChanged(FileTransfer *);
		void fileTransferFinished(FileTransfer *, bool);
		void fileTransferDestroying(FileTransfer *);
};

class FileTransferWindow : public QFrame
{
	Q_OBJECT

	private:
		QScrollView *scrollView;

		QFrame *frame;
		QVBoxLayout *transfersLayout;
		QMap<FileTransfer *, FileTransferWidget *> map;

	protected:
		virtual void keyPressEvent(QKeyEvent *e);
		void contentsChanged();

	public:
		FileTransferWindow(QWidget *parent = 0, const char *name = 0);
		virtual ~FileTransferWindow();

	private slots:
		void clearClicked();

	public slots:
		void newFileTransfer(FileTransfer *);
		void fileTransferFailed(FileTransfer *, FileTransfer::FileTransferError);
		void fileTransferStatusChanged(FileTransfer *);
		void fileTransferFinished(FileTransfer *, bool);
		void fileTransferDestroying(FileTransfer *);
};

class Chat;

class FileTransferManager : public QObject
{
	Q_OBJECT

	private:
		FileTransferWindow *fileTransferWindow;

		int toggleFileTransferWindowMenuId;

		QStringList selectFilesToSend();

		void needFileAccept(DccSocket* socket);
		void needFileInfo(DccSocket* socket);

	private slots:
		void userboxMenuPopup();
		void sendFile();
		void kaduKeyPressed(QKeyEvent* e);
		void connectionBroken(DccSocket* socket);

		void dccEvent(DccSocket *socket, bool &lock);
		void dccError(DccSocket* socket);

		void setState(DccSocket* socket);
		void socketDestroying(DccSocket* socket);

		void chatCreated(Chat *chat);
		void chatDestroying(Chat *chat);

		void fileDropped(const UserGroup *group, const QString &);

		void toggleFileTransferWindow();
		void sendFileActionActivated(const UserGroup* users);

	public:
		FileTransferManager(QObject *parent = 0, const char *name = 0);
		virtual ~FileTransferManager();

		void acceptFile(FileTransfer *ft, DccSocket *socket, QString fileName, bool resume = false);
		void discardFile(DccSocket *socket);

		void showFileTransferWindow();

		void readFromConfig();
		void writeToConfig();

	private slots:
		void fileTransferFinishedSlot(FileTransfer *fileTransfer, bool ok);
		void fileTransferWindowDestroyed();

	public slots:
		/**
			Inicjuje wysy³anie pliku do podanego odbiorcy.
		**/
		void sendFile(UinType receiver);

		void sendFile(UinType receiver, const QString &filename);

	signals:
		void newFileTransfer(FileTransfer *);
		void fileTransferFailed(FileTransfer *, FileTransfer::FileTransferError);
		void fileTransferStatusChanged(FileTransfer *);
		void fileTransferFinished(FileTransfer *, bool);
		void fileTransferDestroying(FileTransfer *);
};

extern FileTransferManager* file_transfer_manager;

/** @} */

#endif
