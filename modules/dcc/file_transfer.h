#ifndef FILE_TRANSFER_H
#define FILE_TRANSFER_H

#include "dcc.h"
#include "userlist.h"

#include <qdialog.h>
#include <qstring.h>
#include <qvaluelist.h>
#include <qmap.h>
#include <qsplitter.h>
#include <qpopupmenu.h>
#include <qdom.h>
#include <qvbox.h>
#include <qlistview.h>

class QLabel;
class QProgressBar;
class QVBoxLayout;

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

	private:
		static QMap<DccSocket*, FileTransfer*> Transfers;

	public:
		static QValueList<FileTransfer *> AllTransfers;

	private:
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
		FileTransfer(QObject *listener, bool listenerHasSlots, FileTransferType type, const UinType &contact,
			const QString &fileName);
		~FileTransfer();

		void addListener(QObject * const listener, bool listenerHasSlots);
		void removeListener(QObject * const listener, bool listenerHasSlots);

		void start(bool restore = false);
		void stop();

		void setSocket(DccSocket* Socket);

		static FileTransfer * bySocket(DccSocket* socket);
		static FileTransfer * byUin(UinType);
		static FileTransfer * byUinAndStatus(UinType, FileTransferStatus);
		static FileTransfer * search(FileTransferType type, const UinType &contact, const QString &fileName,
			bool fullFileName = true);
		static void destroyAll();

		QDomElement toDomElement(const QDomElement &root);
		static FileTransfer * fromDomElement(const QDomElement &dom, QObject *listener, bool listenerHasSlots);

		FileTransferType type();
		FileTransferStatus status();

		UinType contact();
		QString fileName();

		int percent();
		long int speed();
		long long int fileSize();
		long long int transferedSize();

		void finished();

		void needFileInfo();
		void connectionBroken();
		void dccError();
		void noneEvent();
		void dccDone();
		void setState();

	signals:
		void newFileTransfer(FileTransfer *);
		void fileTransferFailed(FileTransfer *, FileTransfer::FileTransferError);
		void fileTransferStatusChanged(FileTransfer *);
		void fileTransferFinished(FileTransfer *, bool);
		void fileTransferDestroying(FileTransfer *);
};

class FileTransferListView : public QListView
{
	Q_OBJECT

	protected:
		virtual void keyPressEvent(QKeyEvent *e);

	public:
		FileTransferListView(QWidget *parent, char *name = 0);
};

class FileTransferListViewItem : public QObject, public QListViewItem
{
	Q_OBJECT

	private:
		FileTransfer *ft;

	protected:
		virtual void keyPressEvent(QKeyEvent *e);

	public:
		FileTransferListViewItem(QListView *parent, FileTransfer *);
		virtual ~FileTransferListViewItem();

		FileTransfer *fileTransfer();

	public slots:
		void newFileTransfer(FileTransfer *);
		void fileTransferFailed(FileTransfer *, FileTransfer::FileTransferError);
		void fileTransferStatusChanged(FileTransfer *);
		void fileTransferFinished(FileTransfer *, bool);
		void fileTransferDestroying(FileTransfer *);
};

class FileTransferWindow : public QSplitter
{
	Q_OBJECT

	private:
		QVBox *incomingBox;
		QVBox *outgoingBox;
		QListView *incoming;
		QListView *outgoing;
		FileTransferListViewItem *currentListViewItem;

		QPopupMenu *popupMenu;
		int startMenuId;
		int stopMenuId;
		int removeMenuId;

	protected:
		virtual void keyPressEvent(QKeyEvent *e);

	public:
		FileTransferWindow(QWidget *parent = 0, const char *name = 0);
		virtual ~FileTransferWindow();

	private slots:
		void listItemClicked(QListViewItem *lvi, const QPoint &, int);

		void startTransferClicked();
		void stopTransferClicked();
		void removeTransferClicked();

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

		void handleCreatedChat(Chat *);
		void handleDestroyingChat(Chat *);

		QString selectFileToSend();

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

		void chatCreated(const UserGroup *group);
		void chatDestroying(const UserGroup *group);

		void fileDropped(const UserGroup *group, const QString &);

		void toggleFileTransferWindow();
		void sendFileActionActivated(const UserGroup* users);

	public:
		FileTransferManager(QObject *parent = 0, const char *name = 0);
		virtual ~FileTransferManager();

		void readFromConfig();
		void writeToConfig();

	private slots:
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

#endif
