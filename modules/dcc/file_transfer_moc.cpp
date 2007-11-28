/****************************************************************************
** FileTransfer meta object code from reading C++ file 'file_transfer.h'
**
** Created: Wed Nov 28 03:34:18 2007
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.8   edited Feb 2 14:59 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "file_transfer.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *FileTransfer::className() const
{
    return "FileTransfer";
}

QMetaObject *FileTransfer::metaObj = 0;
static QMetaObjectCleanUp cleanUp_FileTransfer( "FileTransfer", &FileTransfer::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString FileTransfer::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "FileTransfer", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString FileTransfer::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "FileTransfer", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* FileTransfer::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QObject::staticMetaObject();
    static const QUMethod slot_0 = {"connectionTimeout", 0, 0 };
    static const QUMethod slot_1 = {"updateFileInfo", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "connectionTimeout()", &slot_0, QMetaData::Private },
	{ "updateFileInfo()", &slot_1, QMetaData::Private }
    };
    static const QUParameter param_signal_0[] = {
	{ 0, &static_QUType_ptr, "FileTransfer", QUParameter::In }
    };
    static const QUMethod signal_0 = {"newFileTransfer", 1, param_signal_0 };
    static const QUParameter param_signal_1[] = {
	{ 0, &static_QUType_ptr, "FileTransfer", QUParameter::In },
	{ 0, &static_QUType_ptr, "FileTransfer::FileTransferError", QUParameter::In }
    };
    static const QUMethod signal_1 = {"fileTransferFailed", 2, param_signal_1 };
    static const QUParameter param_signal_2[] = {
	{ 0, &static_QUType_ptr, "FileTransfer", QUParameter::In }
    };
    static const QUMethod signal_2 = {"fileTransferStatusChanged", 1, param_signal_2 };
    static const QUParameter param_signal_3[] = {
	{ 0, &static_QUType_ptr, "FileTransfer", QUParameter::In }
    };
    static const QUMethod signal_3 = {"fileTransferFinished", 1, param_signal_3 };
    static const QUParameter param_signal_4[] = {
	{ 0, &static_QUType_ptr, "FileTransfer", QUParameter::In }
    };
    static const QUMethod signal_4 = {"fileTransferDestroying", 1, param_signal_4 };
    static const QMetaData signal_tbl[] = {
	{ "newFileTransfer(FileTransfer*)", &signal_0, QMetaData::Public },
	{ "fileTransferFailed(FileTransfer*,FileTransfer::FileTransferError)", &signal_1, QMetaData::Public },
	{ "fileTransferStatusChanged(FileTransfer*)", &signal_2, QMetaData::Public },
	{ "fileTransferFinished(FileTransfer*)", &signal_3, QMetaData::Public },
	{ "fileTransferDestroying(FileTransfer*)", &signal_4, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"FileTransfer", parentObject,
	slot_tbl, 2,
	signal_tbl, 5,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_FileTransfer.setMetaObject( metaObj );
    return metaObj;
}

void* FileTransfer::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "FileTransfer" ) )
	return this;
    if ( !qstrcmp( clname, "DccHandler" ) )
	return (DccHandler*)this;
    return QObject::qt_cast( clname );
}

#include <qobjectdefs.h>
#include <qsignalslotimp.h>

// SIGNAL newFileTransfer
void FileTransfer::newFileTransfer( FileTransfer* t0 )
{
    if ( signalsBlocked() )
	return;
    QConnectionList *clist = receivers( staticMetaObject()->signalOffset() + 0 );
    if ( !clist )
	return;
    QUObject o[2];
    static_QUType_ptr.set(o+1,t0);
    activate_signal( clist, o );
}

// SIGNAL fileTransferFailed
void FileTransfer::fileTransferFailed( FileTransfer* t0, FileTransfer::FileTransferError t1 )
{
    if ( signalsBlocked() )
	return;
    QConnectionList *clist = receivers( staticMetaObject()->signalOffset() + 1 );
    if ( !clist )
	return;
    QUObject o[3];
    static_QUType_ptr.set(o+1,t0);
    static_QUType_ptr.set(o+2,&t1);
    activate_signal( clist, o );
}

// SIGNAL fileTransferStatusChanged
void FileTransfer::fileTransferStatusChanged( FileTransfer* t0 )
{
    if ( signalsBlocked() )
	return;
    QConnectionList *clist = receivers( staticMetaObject()->signalOffset() + 2 );
    if ( !clist )
	return;
    QUObject o[2];
    static_QUType_ptr.set(o+1,t0);
    activate_signal( clist, o );
}

// SIGNAL fileTransferFinished
void FileTransfer::fileTransferFinished( FileTransfer* t0 )
{
    if ( signalsBlocked() )
	return;
    QConnectionList *clist = receivers( staticMetaObject()->signalOffset() + 3 );
    if ( !clist )
	return;
    QUObject o[2];
    static_QUType_ptr.set(o+1,t0);
    activate_signal( clist, o );
}

// SIGNAL fileTransferDestroying
void FileTransfer::fileTransferDestroying( FileTransfer* t0 )
{
    if ( signalsBlocked() )
	return;
    QConnectionList *clist = receivers( staticMetaObject()->signalOffset() + 4 );
    if ( !clist )
	return;
    QUObject o[2];
    static_QUType_ptr.set(o+1,t0);
    activate_signal( clist, o );
}

bool FileTransfer::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: connectionTimeout(); break;
    case 1: updateFileInfo(); break;
    default:
	return QObject::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool FileTransfer::qt_emit( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->signalOffset() ) {
    case 0: newFileTransfer((FileTransfer*)static_QUType_ptr.get(_o+1)); break;
    case 1: fileTransferFailed((FileTransfer*)static_QUType_ptr.get(_o+1),(FileTransfer::FileTransferError)(*((FileTransfer::FileTransferError*)static_QUType_ptr.get(_o+2)))); break;
    case 2: fileTransferStatusChanged((FileTransfer*)static_QUType_ptr.get(_o+1)); break;
    case 3: fileTransferFinished((FileTransfer*)static_QUType_ptr.get(_o+1)); break;
    case 4: fileTransferDestroying((FileTransfer*)static_QUType_ptr.get(_o+1)); break;
    default:
	return QObject::qt_emit(_id,_o);
    }
    return TRUE;
}
#ifndef QT_NO_PROPERTIES

bool FileTransfer::qt_property( int id, int f, QVariant* v)
{
    return QObject::qt_property( id, f, v);
}

bool FileTransfer::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
