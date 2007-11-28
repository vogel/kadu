/****************************************************************************
** NewFileTransferNotification meta object code from reading C++ file 'file_transfer_notifications.h'
**
** Created: Wed Nov 28 00:56:03 2007
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.8   edited Feb 2 14:59 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "file_transfer_notifications.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *NewFileTransferNotification::className() const
{
    return "NewFileTransferNotification";
}

QMetaObject *NewFileTransferNotification::metaObj = 0;
static QMetaObjectCleanUp cleanUp_NewFileTransferNotification( "NewFileTransferNotification", &NewFileTransferNotification::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString NewFileTransferNotification::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "NewFileTransferNotification", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString NewFileTransferNotification::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "NewFileTransferNotification", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* NewFileTransferNotification::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = Notification::staticMetaObject();
    static const QUMethod slot_0 = {"callbackAcceptAsNew", 0, 0 };
    static const QUMethod slot_1 = {"callbackAccept", 0, 0 };
    static const QUMethod slot_2 = {"callbackReject", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "callbackAcceptAsNew()", &slot_0, QMetaData::Private },
	{ "callbackAccept()", &slot_1, QMetaData::Public },
	{ "callbackReject()", &slot_2, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"NewFileTransferNotification", parentObject,
	slot_tbl, 3,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_NewFileTransferNotification.setMetaObject( metaObj );
    return metaObj;
}

void* NewFileTransferNotification::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "NewFileTransferNotification" ) )
	return this;
    return Notification::qt_cast( clname );
}

bool NewFileTransferNotification::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: callbackAcceptAsNew(); break;
    case 1: callbackAccept(); break;
    case 2: callbackReject(); break;
    default:
	return Notification::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool NewFileTransferNotification::qt_emit( int _id, QUObject* _o )
{
    return Notification::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool NewFileTransferNotification::qt_property( int id, int f, QVariant* v)
{
    return Notification::qt_property( id, f, v);
}

bool NewFileTransferNotification::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
