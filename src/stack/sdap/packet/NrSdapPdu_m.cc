//
// Generated file, do not edit! Created by opp_msgtool 6.1 from stack/sdap/packet/NrSdapPdu.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include <memory>
#include <type_traits>
#include "NrSdapPdu_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp

namespace simu5g {

Register_Class(NrSdapPdu)

NrSdapPdu::NrSdapPdu() : ::inet::FieldsChunk()
{
    this->setChunkLength(inet::B(1));

}

NrSdapPdu::NrSdapPdu(const NrSdapPdu& other) : ::inet::FieldsChunk(other)
{
    copy(other);
}

NrSdapPdu::~NrSdapPdu()
{
}

NrSdapPdu& NrSdapPdu::operator=(const NrSdapPdu& other)
{
    if (this == &other) return *this;
    ::inet::FieldsChunk::operator=(other);
    copy(other);
    return *this;
}

void NrSdapPdu::copy(const NrSdapPdu& other)
{
    this->qfi = other.qfi;
    this->d_c = other.d_c;
    this->reflectiveQoSIndicator = other.reflectiveQoSIndicator;
}

void NrSdapPdu::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::inet::FieldsChunk::parsimPack(b);
    doParsimPacking(b,this->qfi);
    doParsimPacking(b,this->d_c);
    doParsimPacking(b,this->reflectiveQoSIndicator);
}

void NrSdapPdu::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::inet::FieldsChunk::parsimUnpack(b);
    doParsimUnpacking(b,this->qfi);
    doParsimUnpacking(b,this->d_c);
    doParsimUnpacking(b,this->reflectiveQoSIndicator);
}

uint8_t NrSdapPdu::getQfi() const
{
    return this->qfi;
}

void NrSdapPdu::setQfi(uint8_t qfi)
{
    handleChange();
    this->qfi = qfi;
}

bool NrSdapPdu::getD_c() const
{
    return this->d_c;
}

void NrSdapPdu::setD_c(bool d_c)
{
    handleChange();
    this->d_c = d_c;
}

bool NrSdapPdu::getReflectiveQoSIndicator() const
{
    return this->reflectiveQoSIndicator;
}

void NrSdapPdu::setReflectiveQoSIndicator(bool reflectiveQoSIndicator)
{
    handleChange();
    this->reflectiveQoSIndicator = reflectiveQoSIndicator;
}

class NrSdapPduDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_qfi,
        FIELD_d_c,
        FIELD_reflectiveQoSIndicator,
    };
  public:
    NrSdapPduDescriptor();
    virtual ~NrSdapPduDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(NrSdapPduDescriptor)

NrSdapPduDescriptor::NrSdapPduDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(simu5g::NrSdapPdu)), "inet::FieldsChunk")
{
    propertyNames = nullptr;
}

NrSdapPduDescriptor::~NrSdapPduDescriptor()
{
    delete[] propertyNames;
}

bool NrSdapPduDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<NrSdapPdu *>(obj)!=nullptr;
}

const char **NrSdapPduDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *NrSdapPduDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int NrSdapPduDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 3+base->getFieldCount() : 3;
}

unsigned int NrSdapPduDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_qfi
        FD_ISEDITABLE,    // FIELD_d_c
        FD_ISEDITABLE,    // FIELD_reflectiveQoSIndicator
    };
    return (field >= 0 && field < 3) ? fieldTypeFlags[field] : 0;
}

const char *NrSdapPduDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "qfi",
        "d_c",
        "reflectiveQoSIndicator",
    };
    return (field >= 0 && field < 3) ? fieldNames[field] : nullptr;
}

int NrSdapPduDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "qfi") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "d_c") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "reflectiveQoSIndicator") == 0) return baseIndex + 2;
    return base ? base->findField(fieldName) : -1;
}

const char *NrSdapPduDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "uint8_t",    // FIELD_qfi
        "bool",    // FIELD_d_c
        "bool",    // FIELD_reflectiveQoSIndicator
    };
    return (field >= 0 && field < 3) ? fieldTypeStrings[field] : nullptr;
}

const char **NrSdapPduDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *NrSdapPduDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int NrSdapPduDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    NrSdapPdu *pp = omnetpp::fromAnyPtr<NrSdapPdu>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void NrSdapPduDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    NrSdapPdu *pp = omnetpp::fromAnyPtr<NrSdapPdu>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'NrSdapPdu'", field);
    }
}

const char *NrSdapPduDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    NrSdapPdu *pp = omnetpp::fromAnyPtr<NrSdapPdu>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string NrSdapPduDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    NrSdapPdu *pp = omnetpp::fromAnyPtr<NrSdapPdu>(object); (void)pp;
    switch (field) {
        case FIELD_qfi: return ulong2string(pp->getQfi());
        case FIELD_d_c: return bool2string(pp->getD_c());
        case FIELD_reflectiveQoSIndicator: return bool2string(pp->getReflectiveQoSIndicator());
        default: return "";
    }
}

void NrSdapPduDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    NrSdapPdu *pp = omnetpp::fromAnyPtr<NrSdapPdu>(object); (void)pp;
    switch (field) {
        case FIELD_qfi: pp->setQfi(string2ulong(value)); break;
        case FIELD_d_c: pp->setD_c(string2bool(value)); break;
        case FIELD_reflectiveQoSIndicator: pp->setReflectiveQoSIndicator(string2bool(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'NrSdapPdu'", field);
    }
}

omnetpp::cValue NrSdapPduDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    NrSdapPdu *pp = omnetpp::fromAnyPtr<NrSdapPdu>(object); (void)pp;
    switch (field) {
        case FIELD_qfi: return (omnetpp::intval_t)(pp->getQfi());
        case FIELD_d_c: return pp->getD_c();
        case FIELD_reflectiveQoSIndicator: return pp->getReflectiveQoSIndicator();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'NrSdapPdu' as cValue -- field index out of range?", field);
    }
}

void NrSdapPduDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    NrSdapPdu *pp = omnetpp::fromAnyPtr<NrSdapPdu>(object); (void)pp;
    switch (field) {
        case FIELD_qfi: pp->setQfi(omnetpp::checked_int_cast<uint8_t>(value.intValue())); break;
        case FIELD_d_c: pp->setD_c(value.boolValue()); break;
        case FIELD_reflectiveQoSIndicator: pp->setReflectiveQoSIndicator(value.boolValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'NrSdapPdu'", field);
    }
}

const char *NrSdapPduDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr NrSdapPduDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    NrSdapPdu *pp = omnetpp::fromAnyPtr<NrSdapPdu>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void NrSdapPduDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    NrSdapPdu *pp = omnetpp::fromAnyPtr<NrSdapPdu>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'NrSdapPdu'", field);
    }
}

}  // namespace simu5g

namespace omnetpp {

}  // namespace omnetpp

