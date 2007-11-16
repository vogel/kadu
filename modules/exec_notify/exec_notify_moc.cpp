/****************************************************************************
** ExecConfigurationWidget meta object code from reading C++ file 'exec_notify.h'
**
** Created: Fri Nov 16 04:43:41 2007
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.8   edited Feb 2 14:59 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "exec_notify.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *ExecConfigurationWidget::className() const
{
    return "ExecConfigurationWidget";
}

QMetaObject *ExecConfigurationWidget::metaObj = 0;
static QMetaObjectCleanUp cleanUp_ExecConfigurationWidget( "ExecConfigurationWidget", &ExecConfigurationWidget::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString ExecConfigurationWidget::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "ExecConfigurationWidget", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString ExecConfigurationWidget::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "ExecConfigurationWidget", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* ExecConfigurationWidget::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = NotifierConfigurationWidget::staticMetaObject();
    metaObj = QMetaObject::new_metaobject(
	"ExecConfigurationWidget", parentObject,
	0, 0,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_ExecConfigurationWidget.setMetaObject( metaObj );
    return metaObj;
}

void* ExecConfigurationWidget::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "ExecConfigurationWidget" ) )
	return this;
    return NotifierConfigurationWidget::qt_cast( clname );
}

bool ExecConfigurationWidget::qt_invoke( int _id, QUObject* _o )
{
    return NotifierConfigurationWidget::qt_invoke(_id,_o);
}

bool ExecConfigurationWidget::qt_emit( int _id, QUObject* _o )
{
    return NotifierConfigurationWidget::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool ExecConfigurationWidget::qt_property( int id, int f, QVariant* v)
{
    return NotifierConfigurationWidget::qt_property( id, f, v);
}

bool ExecConfigurationWidget::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES


const char *ExecNotify::className() const
{
    return "ExecNotify";
}

QMetaObject *ExecNotify::metaObj = 0;
static QMetaObjectCleanUp cleanUp_ExecNotify( "ExecNotify", &ExecNotify::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString ExecNotify::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "ExecNotify", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString ExecNotify::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "ExecNotify", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* ExecNotify::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = Notifier::staticMetaObject();
    static const QUParameter param_slot_0[] = {
	{ "args", &static_QUType_varptr, "\x04", QUParameter::In },
	{ "stdin", &static_QUType_QString, 0, QUParameter::In }
    };
    static const QUMethod slot_0 = {"run", 2, param_slot_0 };
    static const QMetaData slot_tbl[] = {
	{ "run(const QStringList&,const QString&)", &slot_0, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"ExecNotify", parentObject,
	slot_tbl, 1,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_ExecNotify.setMetaObject( metaObj );
    return metaObj;
}

void* ExecNotify::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "ExecNotify" ) )
	return this;
    return Notifier::qt_cast( clname );
}

bool ExecNotify::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: run((const QStringList&)*((const QStringList*)static_QUType_ptr.get(_o+1)),(const QString&)static_QUType_QString.get(_o+2)); break;
    default:
	return Notifier::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool ExecNotify::qt_emit( int _id, QUObject* _o )
{
    return Notifier::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool ExecNotify::qt_property( int id, int f, QVariant* v)
{
    return Notifier::qt_property( id, f, v);
}

bool ExecNotify::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
