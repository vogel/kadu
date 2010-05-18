/****************************************************************************
** Meta object code from reading C++ file 'growl_notify_configuration_widget.h'
**
** Created: Tue May 18 18:56:43 2010
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "growl_notify_configuration_widget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'growl_notify_configuration_widget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GrowlNotifyConfigurationWidget[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      39,   32,   31,   31, 0x08,
      68,   62,   31,   31, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_GrowlNotifyConfigurationWidget[] = {
    "GrowlNotifyConfigurationWidget\0\0syntax\0"
    "syntaxChanged(QString)\0title\0"
    "titleChanged(QString)\0"
};

const QMetaObject GrowlNotifyConfigurationWidget::staticMetaObject = {
    { &NotifierConfigurationWidget::staticMetaObject, qt_meta_stringdata_GrowlNotifyConfigurationWidget,
      qt_meta_data_GrowlNotifyConfigurationWidget, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GrowlNotifyConfigurationWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GrowlNotifyConfigurationWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GrowlNotifyConfigurationWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GrowlNotifyConfigurationWidget))
        return static_cast<void*>(const_cast< GrowlNotifyConfigurationWidget*>(this));
    return NotifierConfigurationWidget::qt_metacast(_clname);
}

int GrowlNotifyConfigurationWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = NotifierConfigurationWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: syntaxChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: titleChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
