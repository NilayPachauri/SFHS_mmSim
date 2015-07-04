/****************************************************************************
** Meta object code from reading C++ file 'micromouseserver.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../microMouseServer/micromouseserver.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'micromouseserver.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_microMouseServer_t {
    QByteArrayData data[19];
    char stringdata[226];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_microMouseServer_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_microMouseServer_t qt_meta_stringdata_microMouseServer = {
    {
QT_MOC_LITERAL(0, 0, 16), // "microMouseServer"
QT_MOC_LITERAL(1, 17, 26), // "on_tabWidget_tabBarClicked"
QT_MOC_LITERAL(2, 44, 0), // ""
QT_MOC_LITERAL(3, 45, 5), // "index"
QT_MOC_LITERAL(4, 51, 8), // "loadMaze"
QT_MOC_LITERAL(5, 60, 8), // "saveMaze"
QT_MOC_LITERAL(6, 69, 11), // "addLeftWall"
QT_MOC_LITERAL(7, 81, 4), // "cell"
QT_MOC_LITERAL(8, 86, 12), // "addRightWall"
QT_MOC_LITERAL(9, 99, 10), // "addTopWall"
QT_MOC_LITERAL(10, 110, 13), // "addBottomWall"
QT_MOC_LITERAL(11, 124, 15), // "removeRightWall"
QT_MOC_LITERAL(12, 140, 14), // "removeLeftWall"
QT_MOC_LITERAL(13, 155, 13), // "removeTopWall"
QT_MOC_LITERAL(14, 169, 16), // "removeBottomWall"
QT_MOC_LITERAL(15, 186, 7), // "netComs"
QT_MOC_LITERAL(16, 194, 13), // "connect2mouse"
QT_MOC_LITERAL(17, 208, 7), // "startAI"
QT_MOC_LITERAL(18, 216, 9) // "studentAI"

    },
    "microMouseServer\0on_tabWidget_tabBarClicked\0"
    "\0index\0loadMaze\0saveMaze\0addLeftWall\0"
    "cell\0addRightWall\0addTopWall\0addBottomWall\0"
    "removeRightWall\0removeLeftWall\0"
    "removeTopWall\0removeBottomWall\0netComs\0"
    "connect2mouse\0startAI\0studentAI"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_microMouseServer[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   89,    2, 0x08 /* Private */,
       4,    0,   92,    2, 0x08 /* Private */,
       5,    0,   93,    2, 0x08 /* Private */,
       6,    1,   94,    2, 0x08 /* Private */,
       8,    1,   97,    2, 0x08 /* Private */,
       9,    1,  100,    2, 0x08 /* Private */,
      10,    1,  103,    2, 0x08 /* Private */,
      11,    1,  106,    2, 0x08 /* Private */,
      12,    1,  109,    2, 0x08 /* Private */,
      13,    1,  112,    2, 0x08 /* Private */,
      14,    1,  115,    2, 0x08 /* Private */,
      15,    0,  118,    2, 0x08 /* Private */,
      16,    0,  119,    2, 0x08 /* Private */,
      17,    0,  120,    2, 0x08 /* Private */,
      18,    0,  121,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QPoint,    7,
    QMetaType::Void, QMetaType::QPoint,    7,
    QMetaType::Void, QMetaType::QPoint,    7,
    QMetaType::Void, QMetaType::QPoint,    7,
    QMetaType::Void, QMetaType::QPoint,    7,
    QMetaType::Void, QMetaType::QPoint,    7,
    QMetaType::Void, QMetaType::QPoint,    7,
    QMetaType::Void, QMetaType::QPoint,    7,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void microMouseServer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        microMouseServer *_t = static_cast<microMouseServer *>(_o);
        switch (_id) {
        case 0: _t->on_tabWidget_tabBarClicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->loadMaze(); break;
        case 2: _t->saveMaze(); break;
        case 3: _t->addLeftWall((*reinterpret_cast< QPoint(*)>(_a[1]))); break;
        case 4: _t->addRightWall((*reinterpret_cast< QPoint(*)>(_a[1]))); break;
        case 5: _t->addTopWall((*reinterpret_cast< QPoint(*)>(_a[1]))); break;
        case 6: _t->addBottomWall((*reinterpret_cast< QPoint(*)>(_a[1]))); break;
        case 7: _t->removeRightWall((*reinterpret_cast< QPoint(*)>(_a[1]))); break;
        case 8: _t->removeLeftWall((*reinterpret_cast< QPoint(*)>(_a[1]))); break;
        case 9: _t->removeTopWall((*reinterpret_cast< QPoint(*)>(_a[1]))); break;
        case 10: _t->removeBottomWall((*reinterpret_cast< QPoint(*)>(_a[1]))); break;
        case 11: _t->netComs(); break;
        case 12: _t->connect2mouse(); break;
        case 13: _t->startAI(); break;
        case 14: _t->studentAI(); break;
        default: ;
        }
    }
}

const QMetaObject microMouseServer::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_microMouseServer.data,
      qt_meta_data_microMouseServer,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *microMouseServer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *microMouseServer::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_microMouseServer.stringdata))
        return static_cast<void*>(const_cast< microMouseServer*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int microMouseServer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 15)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 15;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
