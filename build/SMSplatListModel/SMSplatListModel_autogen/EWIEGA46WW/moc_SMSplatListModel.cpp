/****************************************************************************
** Meta object code from reading C++ file 'SMSplatListModel.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../SMSplatListModel/SMSplatListModel.h"
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SMSplatListModel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.6.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSSMSplatListModelENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSSMSplatListModelENDCLASS = QtMocHelpers::stringData(
    "SMSplatListModel",
    "countChanged",
    "",
    "clear",
    "append",
    "SplatInfoPtr",
    "info",
    "insert",
    "index",
    "remove",
    "removeByName",
    "name",
    "get",
    "getByName",
    "indexOf",
    "contains",
    "update",
    "updateByName",
    "names",
    "setItems",
    "QList<SplatInfoPtr>",
    "items",
    "getByNameQml",
    "QtSMSplatGenInfo*",
    "duplicate",
    "count",
    "Roles",
    "NameRole",
    "ImagePathRole",
    "ImageWidthRole",
    "ImageHeightRole",
    "NorthRole",
    "SouthRole",
    "EastRole",
    "WestRole",
    "ZoomLevelRole",
    "TransmitterLonRole",
    "TransmitterLatRole",
    "CoverageRadiusRole",
    "ImageRole",
    "SplatInfoRole",
    "IsSavedInDbRole",
    "ImageIdRole"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSSMSplatListModelENDCLASS_t {
    uint offsetsAndSizes[86];
    char stringdata0[17];
    char stringdata1[13];
    char stringdata2[1];
    char stringdata3[6];
    char stringdata4[7];
    char stringdata5[13];
    char stringdata6[5];
    char stringdata7[7];
    char stringdata8[6];
    char stringdata9[7];
    char stringdata10[13];
    char stringdata11[5];
    char stringdata12[4];
    char stringdata13[10];
    char stringdata14[8];
    char stringdata15[9];
    char stringdata16[7];
    char stringdata17[13];
    char stringdata18[6];
    char stringdata19[9];
    char stringdata20[20];
    char stringdata21[6];
    char stringdata22[13];
    char stringdata23[18];
    char stringdata24[10];
    char stringdata25[6];
    char stringdata26[6];
    char stringdata27[9];
    char stringdata28[14];
    char stringdata29[15];
    char stringdata30[16];
    char stringdata31[10];
    char stringdata32[10];
    char stringdata33[9];
    char stringdata34[9];
    char stringdata35[14];
    char stringdata36[19];
    char stringdata37[19];
    char stringdata38[19];
    char stringdata39[10];
    char stringdata40[14];
    char stringdata41[16];
    char stringdata42[12];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSSMSplatListModelENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSSMSplatListModelENDCLASS_t qt_meta_stringdata_CLASSSMSplatListModelENDCLASS = {
    {
        QT_MOC_LITERAL(0, 16),  // "SMSplatListModel"
        QT_MOC_LITERAL(17, 12),  // "countChanged"
        QT_MOC_LITERAL(30, 0),  // ""
        QT_MOC_LITERAL(31, 5),  // "clear"
        QT_MOC_LITERAL(37, 6),  // "append"
        QT_MOC_LITERAL(44, 12),  // "SplatInfoPtr"
        QT_MOC_LITERAL(57, 4),  // "info"
        QT_MOC_LITERAL(62, 6),  // "insert"
        QT_MOC_LITERAL(69, 5),  // "index"
        QT_MOC_LITERAL(75, 6),  // "remove"
        QT_MOC_LITERAL(82, 12),  // "removeByName"
        QT_MOC_LITERAL(95, 4),  // "name"
        QT_MOC_LITERAL(100, 3),  // "get"
        QT_MOC_LITERAL(104, 9),  // "getByName"
        QT_MOC_LITERAL(114, 7),  // "indexOf"
        QT_MOC_LITERAL(122, 8),  // "contains"
        QT_MOC_LITERAL(131, 6),  // "update"
        QT_MOC_LITERAL(138, 12),  // "updateByName"
        QT_MOC_LITERAL(151, 5),  // "names"
        QT_MOC_LITERAL(157, 8),  // "setItems"
        QT_MOC_LITERAL(166, 19),  // "QList<SplatInfoPtr>"
        QT_MOC_LITERAL(186, 5),  // "items"
        QT_MOC_LITERAL(192, 12),  // "getByNameQml"
        QT_MOC_LITERAL(205, 17),  // "QtSMSplatGenInfo*"
        QT_MOC_LITERAL(223, 9),  // "duplicate"
        QT_MOC_LITERAL(233, 5),  // "count"
        QT_MOC_LITERAL(239, 5),  // "Roles"
        QT_MOC_LITERAL(245, 8),  // "NameRole"
        QT_MOC_LITERAL(254, 13),  // "ImagePathRole"
        QT_MOC_LITERAL(268, 14),  // "ImageWidthRole"
        QT_MOC_LITERAL(283, 15),  // "ImageHeightRole"
        QT_MOC_LITERAL(299, 9),  // "NorthRole"
        QT_MOC_LITERAL(309, 9),  // "SouthRole"
        QT_MOC_LITERAL(319, 8),  // "EastRole"
        QT_MOC_LITERAL(328, 8),  // "WestRole"
        QT_MOC_LITERAL(337, 13),  // "ZoomLevelRole"
        QT_MOC_LITERAL(351, 18),  // "TransmitterLonRole"
        QT_MOC_LITERAL(370, 18),  // "TransmitterLatRole"
        QT_MOC_LITERAL(389, 18),  // "CoverageRadiusRole"
        QT_MOC_LITERAL(408, 9),  // "ImageRole"
        QT_MOC_LITERAL(418, 13),  // "SplatInfoRole"
        QT_MOC_LITERAL(432, 15),  // "IsSavedInDbRole"
        QT_MOC_LITERAL(448, 11)   // "ImageIdRole"
    },
    "SMSplatListModel",
    "countChanged",
    "",
    "clear",
    "append",
    "SplatInfoPtr",
    "info",
    "insert",
    "index",
    "remove",
    "removeByName",
    "name",
    "get",
    "getByName",
    "indexOf",
    "contains",
    "update",
    "updateByName",
    "names",
    "setItems",
    "QList<SplatInfoPtr>",
    "items",
    "getByNameQml",
    "QtSMSplatGenInfo*",
    "duplicate",
    "count",
    "Roles",
    "NameRole",
    "ImagePathRole",
    "ImageWidthRole",
    "ImageHeightRole",
    "NorthRole",
    "SouthRole",
    "EastRole",
    "WestRole",
    "ZoomLevelRole",
    "TransmitterLonRole",
    "TransmitterLatRole",
    "CoverageRadiusRole",
    "ImageRole",
    "SplatInfoRole",
    "IsSavedInDbRole",
    "ImageIdRole"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSSMSplatListModelENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      17,   14, // methods
       1,  165, // properties
       1,  170, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,  116,    2, 0x06,    3 /* Public */,

 // methods: name, argc, parameters, tag, flags, initial metatype offsets
       3,    0,  117,    2, 0x02,    4 /* Public */,
       4,    1,  118,    2, 0x02,    5 /* Public */,
       7,    2,  121,    2, 0x02,    7 /* Public */,
       9,    1,  126,    2, 0x02,   10 /* Public */,
      10,    1,  129,    2, 0x02,   12 /* Public */,
      12,    1,  132,    2, 0x102,   14 /* Public | MethodIsConst  */,
      13,    1,  135,    2, 0x102,   16 /* Public | MethodIsConst  */,
      14,    1,  138,    2, 0x102,   18 /* Public | MethodIsConst  */,
      15,    1,  141,    2, 0x102,   20 /* Public | MethodIsConst  */,
      16,    2,  144,    2, 0x02,   22 /* Public */,
      17,    2,  149,    2, 0x02,   25 /* Public */,
      18,    0,  154,    2, 0x102,   28 /* Public | MethodIsConst  */,
      19,    1,  155,    2, 0x02,   29 /* Public */,
      21,    0,  158,    2, 0x102,   31 /* Public | MethodIsConst  */,
      22,    1,  159,    2, 0x102,   32 /* Public | MethodIsConst  */,
      24,    1,  162,    2, 0x02,   34 /* Public */,

 // signals: parameters
    QMetaType::Void,

 // methods: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 5,    8,    6,
    QMetaType::Bool, QMetaType::Int,    8,
    QMetaType::Bool, QMetaType::QString,   11,
    0x80000000 | 5, QMetaType::Int,    8,
    0x80000000 | 5, QMetaType::QString,   11,
    QMetaType::Int, QMetaType::QString,   11,
    QMetaType::Bool, QMetaType::QString,   11,
    QMetaType::Bool, QMetaType::Int, 0x80000000 | 5,    8,    6,
    QMetaType::Bool, QMetaType::QString, 0x80000000 | 5,   11,    6,
    QMetaType::QStringList,
    QMetaType::Void, 0x80000000 | 20,   21,
    0x80000000 | 20,
    0x80000000 | 23, QMetaType::QString,   11,
    QMetaType::Void, QMetaType::QString,   11,

 // properties: name, type, flags
      25, QMetaType::Int, 0x00015001, uint(0), 0,

 // enums: name, alias, flags, count, data
      26,   26, 0x0,   16,  175,

 // enum data: key, value
      27, uint(SMSplatListModel::NameRole),
      28, uint(SMSplatListModel::ImagePathRole),
      29, uint(SMSplatListModel::ImageWidthRole),
      30, uint(SMSplatListModel::ImageHeightRole),
      31, uint(SMSplatListModel::NorthRole),
      32, uint(SMSplatListModel::SouthRole),
      33, uint(SMSplatListModel::EastRole),
      34, uint(SMSplatListModel::WestRole),
      35, uint(SMSplatListModel::ZoomLevelRole),
      36, uint(SMSplatListModel::TransmitterLonRole),
      37, uint(SMSplatListModel::TransmitterLatRole),
      38, uint(SMSplatListModel::CoverageRadiusRole),
      39, uint(SMSplatListModel::ImageRole),
      40, uint(SMSplatListModel::SplatInfoRole),
      41, uint(SMSplatListModel::IsSavedInDbRole),
      42, uint(SMSplatListModel::ImageIdRole),

       0        // eod
};

Q_CONSTINIT const QMetaObject SMSplatListModel::staticMetaObject = { {
    QMetaObject::SuperData::link<QAbstractListModel::staticMetaObject>(),
    qt_meta_stringdata_CLASSSMSplatListModelENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSSMSplatListModelENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSSMSplatListModelENDCLASS_t,
        // property 'count'
        QtPrivate::TypeAndForceComplete<int, std::true_type>,
        // enum 'Roles'
        QtPrivate::TypeAndForceComplete<SMSplatListModel::Roles, std::true_type>,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<SMSplatListModel, std::true_type>,
        // method 'countChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'clear'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'append'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const SplatInfoPtr &, std::false_type>,
        // method 'insert'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<const SplatInfoPtr &, std::false_type>,
        // method 'remove'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'removeByName'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'get'
        QtPrivate::TypeAndForceComplete<SplatInfoPtr, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'getByName'
        QtPrivate::TypeAndForceComplete<SplatInfoPtr, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'indexOf'
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'contains'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'update'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<const SplatInfoPtr &, std::false_type>,
        // method 'updateByName'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const SplatInfoPtr &, std::false_type>,
        // method 'names'
        QtPrivate::TypeAndForceComplete<QStringList, std::false_type>,
        // method 'setItems'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QList<SplatInfoPtr> &, std::false_type>,
        // method 'items'
        QtPrivate::TypeAndForceComplete<QList<SplatInfoPtr>, std::false_type>,
        // method 'getByNameQml'
        QtPrivate::TypeAndForceComplete<QtSMSplatGenInfo *, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'duplicate'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>
    >,
    nullptr
} };

void SMSplatListModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SMSplatListModel *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->countChanged(); break;
        case 1: _t->clear(); break;
        case 2: _t->append((*reinterpret_cast< std::add_pointer_t<SplatInfoPtr>>(_a[1]))); break;
        case 3: _t->insert((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<SplatInfoPtr>>(_a[2]))); break;
        case 4: { bool _r = _t->remove((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 5: { bool _r = _t->removeByName((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 6: { SplatInfoPtr _r = _t->get((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast< SplatInfoPtr*>(_a[0]) = std::move(_r); }  break;
        case 7: { SplatInfoPtr _r = _t->getByName((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast< SplatInfoPtr*>(_a[0]) = std::move(_r); }  break;
        case 8: { int _r = _t->indexOf((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 9: { bool _r = _t->contains((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 10: { bool _r = _t->update((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<SplatInfoPtr>>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 11: { bool _r = _t->updateByName((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<SplatInfoPtr>>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 12: { QStringList _r = _t->names();
            if (_a[0]) *reinterpret_cast< QStringList*>(_a[0]) = std::move(_r); }  break;
        case 13: _t->setItems((*reinterpret_cast< std::add_pointer_t<QList<SplatInfoPtr>>>(_a[1]))); break;
        case 14: { QList<SplatInfoPtr> _r = _t->items();
            if (_a[0]) *reinterpret_cast< QList<SplatInfoPtr>*>(_a[0]) = std::move(_r); }  break;
        case 15: { QtSMSplatGenInfo* _r = _t->getByNameQml((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast< QtSMSplatGenInfo**>(_a[0]) = std::move(_r); }  break;
        case 16: _t->duplicate((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (SMSplatListModel::*)();
            if (_t _q_method = &SMSplatListModel::countChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
    } else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<SMSplatListModel *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = _t->count(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
}

const QMetaObject *SMSplatListModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SMSplatListModel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSSMSplatListModelENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QAbstractListModel::qt_metacast(_clname);
}

int SMSplatListModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractListModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 17)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 17;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 17)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 17;
    }else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void SMSplatListModel::countChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
