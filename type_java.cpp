/*
 * Copyright (C) 2015, The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "type_java.h"

#include <sys/types.h>

#include "aidl_language.h"
#include "logging.h"

namespace android {
namespace aidl {

Type* VOID_TYPE;
Type* BOOLEAN_TYPE;
Type* BYTE_TYPE;
Type* CHAR_TYPE;
Type* INT_TYPE;
Type* LONG_TYPE;
Type* FLOAT_TYPE;
Type* DOUBLE_TYPE;
Type* STRING_TYPE;
Type* OBJECT_TYPE;
Type* CHAR_SEQUENCE_TYPE;
Type* TEXT_UTILS_TYPE;
Type* REMOTE_EXCEPTION_TYPE;
Type* RUNTIME_EXCEPTION_TYPE;
Type* IBINDER_TYPE;
Type* IINTERFACE_TYPE;
Type* BINDER_NATIVE_TYPE;
Type* BINDER_PROXY_TYPE;
Type* PARCEL_TYPE;
Type* PARCELABLE_INTERFACE_TYPE;
Type* CONTEXT_TYPE;
Type* MAP_TYPE;
Type* LIST_TYPE;
Type* CLASSLOADER_TYPE;

Expression* NULL_VALUE;
Expression* THIS_VALUE;
Expression* SUPER_VALUE;
Expression* TRUE_VALUE;
Expression* FALSE_VALUE;

static Type* make_generic_type(const JavaTypeNamespace* types,
                               const string& package, const string& name,
                               const vector<const Type*>& args) {
  if (package == "java.util" && name == "List") {
    return new GenericListType(types, "java.util", "List", args);
  }
  return NULL;
  // return new GenericType(package, name, args);
}

// ================================================================

Type::Type(const JavaTypeNamespace* types, const string& name, int kind,
           bool canWriteToParcel, bool canBeOut)
    : m_types(types),
      m_package(),
      m_name(name),
      m_declFile(""),
      m_declLine(-1),
      m_kind(kind),
      m_canWriteToParcel(canWriteToParcel),
      m_canBeOut(canBeOut) {
  m_qualifiedName = name;
}

Type::Type(const JavaTypeNamespace* types, const string& package,
           const string& name, int kind, bool canWriteToParcel, bool canBeOut,
           const string& declFile, int declLine)
    : m_types(types),
      m_package(package),
      m_name(name),
      m_declFile(declFile),
      m_declLine(declLine),
      m_kind(kind),
      m_canWriteToParcel(canWriteToParcel),
      m_canBeOut(canBeOut) {
  if (package.length() > 0) {
    m_qualifiedName = package;
    m_qualifiedName += '.';
  }
  m_qualifiedName += name;
}

Type::~Type() {}

string Type::HumanReadableKind() const {
  switch (Kind()) {
    case INTERFACE:
      return "an interface";
    case USERDATA:
      return "a user data";
    default:
      return "ERROR";
  }
}

bool Type::CanBeArray() const { return false; }

string Type::ImportType() const { return m_qualifiedName; }

string Type::CreatorName() const { return ""; }

string Type::InstantiableName() const { return QualifiedName(); }

void Type::WriteToParcel(StatementBlock* addTo, Variable* v, Variable* parcel,
                         int flags) const {
  fprintf(stderr, "aidl:internal error %s:%d qualifiedName=%sn", __FILE__,
          __LINE__, m_qualifiedName.c_str());
  addTo->Add(new LiteralExpression("/* WriteToParcel error " + m_qualifiedName +
                                   " */"));
}

void Type::CreateFromParcel(StatementBlock* addTo, Variable* v,
                            Variable* parcel, Variable**) const {
  fprintf(stderr, "aidl:internal error %s:%d qualifiedName=%s\n", __FILE__,
          __LINE__, m_qualifiedName.c_str());
  addTo->Add(new LiteralExpression("/* CreateFromParcel error " +
                                   m_qualifiedName + " */"));
}

void Type::ReadFromParcel(StatementBlock* addTo, Variable* v, Variable* parcel,
                          Variable**) const {
  fprintf(stderr, "aidl:internal error %s:%d qualifiedName=%s\n", __FILE__,
          __LINE__, m_qualifiedName.c_str());
  addTo->Add(new LiteralExpression("/* ReadFromParcel error " +
                                   m_qualifiedName + " */"));
}

void Type::WriteArrayToParcel(StatementBlock* addTo, Variable* v,
                              Variable* parcel, int flags) const {
  fprintf(stderr, "aidl:internal error %s:%d qualifiedName=%s\n", __FILE__,
          __LINE__, m_qualifiedName.c_str());
  addTo->Add(new LiteralExpression("/* WriteArrayToParcel error " +
                                   m_qualifiedName + " */"));
}

void Type::CreateArrayFromParcel(StatementBlock* addTo, Variable* v,
                                 Variable* parcel, Variable**) const {
  fprintf(stderr, "aidl:internal error %s:%d qualifiedName=%s\n", __FILE__,
          __LINE__, m_qualifiedName.c_str());
  addTo->Add(new LiteralExpression("/* CreateArrayFromParcel error " +
                                   m_qualifiedName + " */"));
}

void Type::ReadArrayFromParcel(StatementBlock* addTo, Variable* v,
                               Variable* parcel, Variable**) const {
  fprintf(stderr, "aidl:internal error %s:%d qualifiedName=%s\n", __FILE__,
          __LINE__, m_qualifiedName.c_str());
  addTo->Add(new LiteralExpression("/* ReadArrayFromParcel error " +
                                   m_qualifiedName + " */"));
}

void Type::SetQualifiedName(const string& qualified) {
  m_qualifiedName = qualified;
}

Expression* Type::BuildWriteToParcelFlags(int flags) const {
  if (flags == 0) {
    return new LiteralExpression("0");
  }
  if ((flags & PARCELABLE_WRITE_RETURN_VALUE) != 0) {
    return new FieldVariable(PARCELABLE_INTERFACE_TYPE,
                             "PARCELABLE_WRITE_RETURN_VALUE");
  }
  return new LiteralExpression("0");
}

// ================================================================

BasicType::BasicType(const JavaTypeNamespace* types, const string& name,
                     const string& marshallParcel,
                     const string& unmarshallParcel,
                     const string& writeArrayParcel,
                     const string& createArrayParcel,
                     const string& readArrayParcel)
    : Type(types, name, BUILT_IN, true, false),
      m_marshallParcel(marshallParcel),
      m_unmarshallParcel(unmarshallParcel),
      m_writeArrayParcel(writeArrayParcel),
      m_createArrayParcel(createArrayParcel),
      m_readArrayParcel(readArrayParcel) {}

void BasicType::WriteToParcel(StatementBlock* addTo, Variable* v,
                              Variable* parcel, int flags) const {
  addTo->Add(new MethodCall(parcel, m_marshallParcel, 1, v));
}

void BasicType::CreateFromParcel(StatementBlock* addTo, Variable* v,
                                 Variable* parcel, Variable**) const {
  addTo->Add(new Assignment(v, new MethodCall(parcel, m_unmarshallParcel)));
}

bool BasicType::CanBeArray() const { return true; }

void BasicType::WriteArrayToParcel(StatementBlock* addTo, Variable* v,
                                   Variable* parcel, int flags) const {
  addTo->Add(new MethodCall(parcel, m_writeArrayParcel, 1, v));
}

void BasicType::CreateArrayFromParcel(StatementBlock* addTo, Variable* v,
                                      Variable* parcel, Variable**) const {
  addTo->Add(new Assignment(v, new MethodCall(parcel, m_createArrayParcel)));
}

void BasicType::ReadArrayFromParcel(StatementBlock* addTo, Variable* v,
                                    Variable* parcel, Variable**) const {
  addTo->Add(new MethodCall(parcel, m_readArrayParcel, 1, v));
}

// ================================================================

BooleanType::BooleanType(const JavaTypeNamespace* types)
    : Type(types, "boolean", BUILT_IN, true, false) {}

void BooleanType::WriteToParcel(StatementBlock* addTo, Variable* v,
                                Variable* parcel, int flags) const {
  addTo->Add(new MethodCall(
      parcel, "writeInt", 1,
      new Ternary(v, new LiteralExpression("1"), new LiteralExpression("0"))));
}

void BooleanType::CreateFromParcel(StatementBlock* addTo, Variable* v,
                                   Variable* parcel, Variable**) const {
  addTo->Add(
      new Assignment(v, new Comparison(new LiteralExpression("0"), "!=",
                                       new MethodCall(parcel, "readInt"))));
}

bool BooleanType::CanBeArray() const { return true; }

void BooleanType::WriteArrayToParcel(StatementBlock* addTo, Variable* v,
                                     Variable* parcel, int flags) const {
  addTo->Add(new MethodCall(parcel, "writeBooleanArray", 1, v));
}

void BooleanType::CreateArrayFromParcel(StatementBlock* addTo, Variable* v,
                                        Variable* parcel, Variable**) const {
  addTo->Add(new Assignment(v, new MethodCall(parcel, "createBooleanArray")));
}

void BooleanType::ReadArrayFromParcel(StatementBlock* addTo, Variable* v,
                                      Variable* parcel, Variable**) const {
  addTo->Add(new MethodCall(parcel, "readBooleanArray", 1, v));
}

// ================================================================

CharType::CharType(const JavaTypeNamespace* types)
    : Type(types, "char", BUILT_IN, true, false) {}

void CharType::WriteToParcel(StatementBlock* addTo, Variable* v,
                             Variable* parcel, int flags) const {
  addTo->Add(
      new MethodCall(parcel, "writeInt", 1, new Cast(m_types->IntType(), v)));
}

void CharType::CreateFromParcel(StatementBlock* addTo, Variable* v,
                                Variable* parcel, Variable**) const {
  addTo->Add(new Assignment(v, new MethodCall(parcel, "readInt"), this));
}

bool CharType::CanBeArray() const { return true; }

void CharType::WriteArrayToParcel(StatementBlock* addTo, Variable* v,
                                  Variable* parcel, int flags) const {
  addTo->Add(new MethodCall(parcel, "writeCharArray", 1, v));
}

void CharType::CreateArrayFromParcel(StatementBlock* addTo, Variable* v,
                                     Variable* parcel, Variable**) const {
  addTo->Add(new Assignment(v, new MethodCall(parcel, "createCharArray")));
}

void CharType::ReadArrayFromParcel(StatementBlock* addTo, Variable* v,
                                   Variable* parcel, Variable**) const {
  addTo->Add(new MethodCall(parcel, "readCharArray", 1, v));
}

// ================================================================

StringType::StringType(const JavaTypeNamespace* types)
    : Type(types, "java.lang", "String", BUILT_IN, true, false) {}

string StringType::CreatorName() const {
  return "android.os.Parcel.STRING_CREATOR";
}

void StringType::WriteToParcel(StatementBlock* addTo, Variable* v,
                               Variable* parcel, int flags) const {
  addTo->Add(new MethodCall(parcel, "writeString", 1, v));
}

void StringType::CreateFromParcel(StatementBlock* addTo, Variable* v,
                                  Variable* parcel, Variable**) const {
  addTo->Add(new Assignment(v, new MethodCall(parcel, "readString")));
}

bool StringType::CanBeArray() const { return true; }

void StringType::WriteArrayToParcel(StatementBlock* addTo, Variable* v,
                                    Variable* parcel, int flags) const {
  addTo->Add(new MethodCall(parcel, "writeStringArray", 1, v));
}

void StringType::CreateArrayFromParcel(StatementBlock* addTo, Variable* v,
                                       Variable* parcel, Variable**) const {
  addTo->Add(new Assignment(v, new MethodCall(parcel, "createStringArray")));
}

void StringType::ReadArrayFromParcel(StatementBlock* addTo, Variable* v,
                                     Variable* parcel, Variable**) const {
  addTo->Add(new MethodCall(parcel, "readStringArray", 1, v));
}

// ================================================================

CharSequenceType::CharSequenceType(const JavaTypeNamespace* types)
    : Type(types, "java.lang", "CharSequence", BUILT_IN, true, false) {}

string CharSequenceType::CreatorName() const {
  return "android.os.Parcel.STRING_CREATOR";
}

void CharSequenceType::WriteToParcel(StatementBlock* addTo, Variable* v,
                                     Variable* parcel, int flags) const {
  // if (v != null) {
  //     parcel.writeInt(1);
  //     v.writeToParcel(parcel);
  // } else {
  //     parcel.writeInt(0);
  // }
  IfStatement* elsepart = new IfStatement();
  elsepart->statements->Add(
      new MethodCall(parcel, "writeInt", 1, new LiteralExpression("0")));
  IfStatement* ifpart = new IfStatement;
  ifpart->expression = new Comparison(v, "!=", NULL_VALUE);
  ifpart->elseif = elsepart;
  ifpart->statements->Add(
      new MethodCall(parcel, "writeInt", 1, new LiteralExpression("1")));
  ifpart->statements->Add(new MethodCall(TEXT_UTILS_TYPE, "writeToParcel", 3, v,
                                         parcel,
                                         BuildWriteToParcelFlags(flags)));

  addTo->Add(ifpart);
}

void CharSequenceType::CreateFromParcel(StatementBlock* addTo, Variable* v,
                                        Variable* parcel, Variable**) const {
  // if (0 != parcel.readInt()) {
  //     v = TextUtils.createFromParcel(parcel)
  // } else {
  //     v = null;
  // }
  IfStatement* elsepart = new IfStatement();
  elsepart->statements->Add(new Assignment(v, NULL_VALUE));

  IfStatement* ifpart = new IfStatement();
  ifpart->expression = new Comparison(new LiteralExpression("0"), "!=",
                                      new MethodCall(parcel, "readInt"));
  ifpart->elseif = elsepart;
  ifpart->statements->Add(new Assignment(
      v, new MethodCall(TEXT_UTILS_TYPE,
                        "CHAR_SEQUENCE_CREATOR.createFromParcel", 1, parcel)));

  addTo->Add(ifpart);
}

// ================================================================

RemoteExceptionType::RemoteExceptionType(const JavaTypeNamespace* types)
    : Type(types, "android.os", "RemoteException", BUILT_IN, false, false) {}

void RemoteExceptionType::WriteToParcel(StatementBlock* addTo, Variable* v,
                                        Variable* parcel, int flags) const {
  fprintf(stderr, "aidl:internal error %s:%d\n", __FILE__, __LINE__);
}

void RemoteExceptionType::CreateFromParcel(StatementBlock* addTo, Variable* v,
                                           Variable* parcel, Variable**) const {
  fprintf(stderr, "aidl:internal error %s:%d\n", __FILE__, __LINE__);
}

// ================================================================

RuntimeExceptionType::RuntimeExceptionType(const JavaTypeNamespace* types)
    : Type(types, "java.lang", "RuntimeException", BUILT_IN, false, false) {}

void RuntimeExceptionType::WriteToParcel(StatementBlock* addTo, Variable* v,
                                         Variable* parcel, int flags) const {
  fprintf(stderr, "aidl:internal error %s:%d\n", __FILE__, __LINE__);
}

void RuntimeExceptionType::CreateFromParcel(StatementBlock* addTo, Variable* v,
                                            Variable* parcel,
                                            Variable**) const {
  fprintf(stderr, "aidl:internal error %s:%d\n", __FILE__, __LINE__);
}

// ================================================================

IBinderType::IBinderType(const JavaTypeNamespace* types)
    : Type(types, "android.os", "IBinder", BUILT_IN, true, false) {}

void IBinderType::WriteToParcel(StatementBlock* addTo, Variable* v,
                                Variable* parcel, int flags) const {
  addTo->Add(new MethodCall(parcel, "writeStrongBinder", 1, v));
}

void IBinderType::CreateFromParcel(StatementBlock* addTo, Variable* v,
                                   Variable* parcel, Variable**) const {
  addTo->Add(new Assignment(v, new MethodCall(parcel, "readStrongBinder")));
}

void IBinderType::WriteArrayToParcel(StatementBlock* addTo, Variable* v,
                                     Variable* parcel, int flags) const {
  addTo->Add(new MethodCall(parcel, "writeBinderArray", 1, v));
}

void IBinderType::CreateArrayFromParcel(StatementBlock* addTo, Variable* v,
                                        Variable* parcel, Variable**) const {
  addTo->Add(new Assignment(v, new MethodCall(parcel, "createBinderArray")));
}

void IBinderType::ReadArrayFromParcel(StatementBlock* addTo, Variable* v,
                                      Variable* parcel, Variable**) const {
  addTo->Add(new MethodCall(parcel, "readBinderArray", 1, v));
}

// ================================================================

IInterfaceType::IInterfaceType(const JavaTypeNamespace* types)
    : Type(types, "android.os", "IInterface", BUILT_IN, false, false) {}

void IInterfaceType::WriteToParcel(StatementBlock* addTo, Variable* v,
                                   Variable* parcel, int flags) const {
  fprintf(stderr, "aidl:internal error %s:%d\n", __FILE__, __LINE__);
}

void IInterfaceType::CreateFromParcel(StatementBlock* addTo, Variable* v,
                                      Variable* parcel, Variable**) const {
  fprintf(stderr, "aidl:internal error %s:%d\n", __FILE__, __LINE__);
}

// ================================================================

BinderType::BinderType(const JavaTypeNamespace* types)
    : Type(types, "android.os", "Binder", BUILT_IN, false, false) {}

void BinderType::WriteToParcel(StatementBlock* addTo, Variable* v,
                               Variable* parcel, int flags) const {
  fprintf(stderr, "aidl:internal error %s:%d\n", __FILE__, __LINE__);
}

void BinderType::CreateFromParcel(StatementBlock* addTo, Variable* v,
                                  Variable* parcel, Variable**) const {
  fprintf(stderr, "aidl:internal error %s:%d\n", __FILE__, __LINE__);
}

// ================================================================

BinderProxyType::BinderProxyType(const JavaTypeNamespace* types)
    : Type(types, "android.os", "BinderProxy", BUILT_IN, false, false) {}

void BinderProxyType::WriteToParcel(StatementBlock* addTo, Variable* v,
                                    Variable* parcel, int flags) const {
  fprintf(stderr, "aidl:internal error %s:%d\n", __FILE__, __LINE__);
}

void BinderProxyType::CreateFromParcel(StatementBlock* addTo, Variable* v,
                                       Variable* parcel, Variable**) const {
  fprintf(stderr, "aidl:internal error %s:%d\n", __FILE__, __LINE__);
}

// ================================================================

ParcelType::ParcelType(const JavaTypeNamespace* types)
    : Type(types, "android.os", "Parcel", BUILT_IN, false, false) {}

void ParcelType::WriteToParcel(StatementBlock* addTo, Variable* v,
                               Variable* parcel, int flags) const {
  fprintf(stderr, "aidl:internal error %s:%d\n", __FILE__, __LINE__);
}

void ParcelType::CreateFromParcel(StatementBlock* addTo, Variable* v,
                                  Variable* parcel, Variable**) const {
  fprintf(stderr, "aidl:internal error %s:%d\n", __FILE__, __LINE__);
}

// ================================================================

ParcelableInterfaceType::ParcelableInterfaceType(const JavaTypeNamespace* types)
    : Type(types, "android.os", "Parcelable", BUILT_IN, false, false) {}

void ParcelableInterfaceType::WriteToParcel(StatementBlock* addTo, Variable* v,
                                            Variable* parcel, int flags) const {
  fprintf(stderr, "aidl:internal error %s:%d\n", __FILE__, __LINE__);
}

void ParcelableInterfaceType::CreateFromParcel(StatementBlock* addTo,
                                               Variable* v, Variable* parcel,
                                               Variable**) const {
  fprintf(stderr, "aidl:internal error %s:%d\n", __FILE__, __LINE__);
}

// ================================================================

MapType::MapType(const JavaTypeNamespace* types)
    : Type(types, "java.util", "Map", BUILT_IN, true, true) {}

void MapType::WriteToParcel(StatementBlock* addTo, Variable* v,
                            Variable* parcel, int flags) const {
  addTo->Add(new MethodCall(parcel, "writeMap", 1, v));
}

static void EnsureClassLoader(StatementBlock* addTo, Variable** cl) {
  // We don't want to look up the class loader once for every
  // collection argument, so ensure we do it at most once per method.
  if (*cl == NULL) {
    *cl = new Variable(CLASSLOADER_TYPE, "cl");
    addTo->Add(new VariableDeclaration(
        *cl, new LiteralExpression("this.getClass().getClassLoader()"),
        CLASSLOADER_TYPE));
  }
}

void MapType::CreateFromParcel(StatementBlock* addTo, Variable* v,
                               Variable* parcel, Variable** cl) const {
  EnsureClassLoader(addTo, cl);
  addTo->Add(new Assignment(v, new MethodCall(parcel, "readHashMap", 1, *cl)));
}

void MapType::ReadFromParcel(StatementBlock* addTo, Variable* v,
                             Variable* parcel, Variable** cl) const {
  EnsureClassLoader(addTo, cl);
  addTo->Add(new MethodCall(parcel, "readMap", 2, v, *cl));
}

// ================================================================

ListType::ListType(const JavaTypeNamespace* types)
    : Type(types, "java.util", "List", BUILT_IN, true, true) {}

string ListType::InstantiableName() const { return "java.util.ArrayList"; }

void ListType::WriteToParcel(StatementBlock* addTo, Variable* v,
                             Variable* parcel, int flags) const {
  addTo->Add(new MethodCall(parcel, "writeList", 1, v));
}

void ListType::CreateFromParcel(StatementBlock* addTo, Variable* v,
                                Variable* parcel, Variable** cl) const {
  EnsureClassLoader(addTo, cl);
  addTo->Add(
      new Assignment(v, new MethodCall(parcel, "readArrayList", 1, *cl)));
}

void ListType::ReadFromParcel(StatementBlock* addTo, Variable* v,
                              Variable* parcel, Variable** cl) const {
  EnsureClassLoader(addTo, cl);
  addTo->Add(new MethodCall(parcel, "readList", 2, v, *cl));
}

// ================================================================

UserDataType::UserDataType(const JavaTypeNamespace* types,
                           const string& package, const string& name,
                           bool builtIn, bool canWriteToParcel,
                           const string& declFile, int declLine)
    : Type(types, package, name, builtIn ? BUILT_IN : USERDATA,
           canWriteToParcel, true, declFile, declLine) {}

string UserDataType::CreatorName() const {
  return QualifiedName() + ".CREATOR";
}

void UserDataType::WriteToParcel(StatementBlock* addTo, Variable* v,
                                 Variable* parcel, int flags) const {
  // if (v != null) {
  //     parcel.writeInt(1);
  //     v.writeToParcel(parcel);
  // } else {
  //     parcel.writeInt(0);
  // }
  IfStatement* elsepart = new IfStatement();
  elsepart->statements->Add(
      new MethodCall(parcel, "writeInt", 1, new LiteralExpression("0")));
  IfStatement* ifpart = new IfStatement;
  ifpart->expression = new Comparison(v, "!=", NULL_VALUE);
  ifpart->elseif = elsepart;
  ifpart->statements->Add(
      new MethodCall(parcel, "writeInt", 1, new LiteralExpression("1")));
  ifpart->statements->Add(new MethodCall(v, "writeToParcel", 2, parcel,
                                         BuildWriteToParcelFlags(flags)));

  addTo->Add(ifpart);
}

void UserDataType::CreateFromParcel(StatementBlock* addTo, Variable* v,
                                    Variable* parcel, Variable**) const {
  // if (0 != parcel.readInt()) {
  //     v = CLASS.CREATOR.createFromParcel(parcel)
  // } else {
  //     v = null;
  // }
  IfStatement* elsepart = new IfStatement();
  elsepart->statements->Add(new Assignment(v, NULL_VALUE));

  IfStatement* ifpart = new IfStatement();
  ifpart->expression = new Comparison(new LiteralExpression("0"), "!=",
                                      new MethodCall(parcel, "readInt"));
  ifpart->elseif = elsepart;
  ifpart->statements->Add(new Assignment(
      v, new MethodCall(v->type, "CREATOR.createFromParcel", 1, parcel)));

  addTo->Add(ifpart);
}

void UserDataType::ReadFromParcel(StatementBlock* addTo, Variable* v,
                                  Variable* parcel, Variable**) const {
  // TODO: really, we don't need to have this extra check, but we
  // don't have two separate marshalling code paths
  // if (0 != parcel.readInt()) {
  //     v.readFromParcel(parcel)
  // }
  IfStatement* ifpart = new IfStatement();
  ifpart->expression = new Comparison(new LiteralExpression("0"), "!=",
                                      new MethodCall(parcel, "readInt"));
  ifpart->statements->Add(new MethodCall(v, "readFromParcel", 1, parcel));
  addTo->Add(ifpart);
}

bool UserDataType::CanBeArray() const { return true; }

void UserDataType::WriteArrayToParcel(StatementBlock* addTo, Variable* v,
                                      Variable* parcel, int flags) const {
  addTo->Add(new MethodCall(parcel, "writeTypedArray", 2, v,
                            BuildWriteToParcelFlags(flags)));
}

void UserDataType::CreateArrayFromParcel(StatementBlock* addTo, Variable* v,
                                         Variable* parcel, Variable**) const {
  string creator = v->type->QualifiedName() + ".CREATOR";
  addTo->Add(new Assignment(v, new MethodCall(parcel, "createTypedArray", 1,
                                              new LiteralExpression(creator))));
}

void UserDataType::ReadArrayFromParcel(StatementBlock* addTo, Variable* v,
                                       Variable* parcel, Variable**) const {
  string creator = v->type->QualifiedName() + ".CREATOR";
  addTo->Add(new MethodCall(parcel, "readTypedArray", 2, v,
                            new LiteralExpression(creator)));
}

// ================================================================

InterfaceType::InterfaceType(const JavaTypeNamespace* types,
                             const string& package, const string& name,
                             bool builtIn, bool oneway, const string& declFile,
                             int declLine)
    : Type(types, package, name, builtIn ? BUILT_IN : INTERFACE, true, false,
           declFile, declLine),
      m_oneway(oneway) {}

bool InterfaceType::OneWay() const { return m_oneway; }

void InterfaceType::WriteToParcel(StatementBlock* addTo, Variable* v,
                                  Variable* parcel, int flags) const {
  // parcel.writeStrongBinder(v != null ? v.asBinder() : null);
  addTo->Add(
      new MethodCall(parcel, "writeStrongBinder", 1,
                     new Ternary(new Comparison(v, "!=", NULL_VALUE),
                                 new MethodCall(v, "asBinder"), NULL_VALUE)));
}

void InterfaceType::CreateFromParcel(StatementBlock* addTo, Variable* v,
                                     Variable* parcel, Variable**) const {
  // v = Interface.asInterface(parcel.readStrongBinder());
  string stub_type = v->type->QualifiedName() + ".Stub";
  addTo->Add(new Assignment(
      v, new MethodCall(m_types->Find(stub_type), "asInterface", 1,
                        new MethodCall(parcel, "readStrongBinder"))));
}

// ================================================================

GenericType::GenericType(const JavaTypeNamespace* types, const string& package,
                         const string& name, const vector<const Type*>& args)
    : Type(types, package, name, BUILT_IN, true, true) {
  m_args = args;

  m_importName = package + '.' + name;

  string gen = "<";
  int N = args.size();
  for (int i = 0; i < N; i++) {
    const Type* t = args[i];
    gen += t->QualifiedName();
    if (i != N - 1) {
      gen += ',';
    }
  }
  gen += '>';
  m_genericArguments = gen;
  SetQualifiedName(m_importName + gen);
}

const vector<const Type*>& GenericType::GenericArgumentTypes() const {
  return m_args;
}

string GenericType::GenericArguments() const { return m_genericArguments; }

string GenericType::ImportType() const { return m_importName; }

void GenericType::WriteToParcel(StatementBlock* addTo, Variable* v,
                                Variable* parcel, int flags) const {
  fprintf(stderr, "implement GenericType::WriteToParcel\n");
}

void GenericType::CreateFromParcel(StatementBlock* addTo, Variable* v,
                                   Variable* parcel, Variable**) const {
  fprintf(stderr, "implement GenericType::CreateFromParcel\n");
}

void GenericType::ReadFromParcel(StatementBlock* addTo, Variable* v,
                                 Variable* parcel, Variable**) const {
  fprintf(stderr, "implement GenericType::ReadFromParcel\n");
}

// ================================================================

GenericListType::GenericListType(const JavaTypeNamespace* types,
                                 const string& package, const string& name,
                                 const vector<const Type*>& args)
    : GenericType(types, package, name, args),
      m_creator(args[0]->CreatorName()) {}

string GenericListType::CreatorName() const {
  return "android.os.Parcel.arrayListCreator";
}

string GenericListType::InstantiableName() const {
  return "java.util.ArrayList" + GenericArguments();
}

void GenericListType::WriteToParcel(StatementBlock* addTo, Variable* v,
                                    Variable* parcel, int flags) const {
  if (m_creator == STRING_TYPE->CreatorName()) {
    addTo->Add(new MethodCall(parcel, "writeStringList", 1, v));
  } else if (m_creator == IBINDER_TYPE->CreatorName()) {
    addTo->Add(new MethodCall(parcel, "writeBinderList", 1, v));
  } else {
    // parcel.writeTypedListXX(arg);
    addTo->Add(new MethodCall(parcel, "writeTypedList", 1, v));
  }
}

void GenericListType::CreateFromParcel(StatementBlock* addTo, Variable* v,
                                       Variable* parcel, Variable**) const {
  if (m_creator == STRING_TYPE->CreatorName()) {
    addTo->Add(
        new Assignment(v, new MethodCall(parcel, "createStringArrayList", 0)));
  } else if (m_creator == IBINDER_TYPE->CreatorName()) {
    addTo->Add(
        new Assignment(v, new MethodCall(parcel, "createBinderArrayList", 0)));
  } else {
    // v = _data.readTypedArrayList(XXX.creator);
    addTo->Add(
        new Assignment(v, new MethodCall(parcel, "createTypedArrayList", 1,
                                         new LiteralExpression(m_creator))));
  }
}

void GenericListType::ReadFromParcel(StatementBlock* addTo, Variable* v,
                                     Variable* parcel, Variable**) const {
  if (m_creator == STRING_TYPE->CreatorName()) {
    addTo->Add(new MethodCall(parcel, "readStringList", 1, v));
  } else if (m_creator == IBINDER_TYPE->CreatorName()) {
    addTo->Add(new MethodCall(parcel, "readBinderList", 1, v));
  } else {
    // v = _data.readTypedList(v, XXX.creator);
    addTo->Add(new MethodCall(parcel, "readTypedList", 2, v,
                              new LiteralExpression(m_creator)));
  }
}

// ================================================================

ClassLoaderType::ClassLoaderType(const JavaTypeNamespace* types)
    : Type(types, "java.lang", "ClassLoader", BUILT_IN, false, false) {}

// ================================================================

JavaTypeNamespace::JavaTypeNamespace() {
  VOID_TYPE = new BasicType(this, "void", "XXX", "XXX", "XXX", "XXX", "XXX");
  Add(VOID_TYPE);

  BOOLEAN_TYPE = new BooleanType(this);
  Add(BOOLEAN_TYPE);

  BYTE_TYPE =
      new BasicType(this, "byte", "writeByte", "readByte", "writeByteArray",
                    "createByteArray", "readByteArray");
  Add(BYTE_TYPE);

  CHAR_TYPE = new CharType(this);
  Add(CHAR_TYPE);

  INT_TYPE = new BasicType(this, "int", "writeInt", "readInt", "writeIntArray",
                           "createIntArray", "readIntArray");
  Add(INT_TYPE);
  m_int_type = INT_TYPE;

  LONG_TYPE =
      new BasicType(this, "long", "writeLong", "readLong", "writeLongArray",
                    "createLongArray", "readLongArray");
  Add(LONG_TYPE);

  FLOAT_TYPE =
      new BasicType(this, "float", "writeFloat", "readFloat", "writeFloatArray",
                    "createFloatArray", "readFloatArray");
  Add(FLOAT_TYPE);

  DOUBLE_TYPE =
      new BasicType(this, "double", "writeDouble", "readDouble",
                    "writeDoubleArray", "createDoubleArray", "readDoubleArray");
  Add(DOUBLE_TYPE);

  STRING_TYPE = new StringType(this);
  Add(STRING_TYPE);

  OBJECT_TYPE =
      new Type(this, "java.lang", "Object", Type::BUILT_IN, false, false);
  Add(OBJECT_TYPE);

  CHAR_SEQUENCE_TYPE = new CharSequenceType(this);
  Add(CHAR_SEQUENCE_TYPE);

  MAP_TYPE = new MapType(this);
  Add(MAP_TYPE);

  LIST_TYPE = new ListType(this);
  Add(LIST_TYPE);

  TEXT_UTILS_TYPE =
      new Type(this, "android.text", "TextUtils", Type::BUILT_IN, false, false);
  Add(TEXT_UTILS_TYPE);

  REMOTE_EXCEPTION_TYPE = new RemoteExceptionType(this);
  Add(REMOTE_EXCEPTION_TYPE);

  RUNTIME_EXCEPTION_TYPE = new RuntimeExceptionType(this);
  Add(RUNTIME_EXCEPTION_TYPE);

  IBINDER_TYPE = new IBinderType(this);
  Add(IBINDER_TYPE);

  IINTERFACE_TYPE = new IInterfaceType(this);
  Add(IINTERFACE_TYPE);

  BINDER_NATIVE_TYPE = new BinderType(this);
  Add(BINDER_NATIVE_TYPE);

  BINDER_PROXY_TYPE = new BinderProxyType(this);
  Add(BINDER_PROXY_TYPE);

  PARCEL_TYPE = new ParcelType(this);
  Add(PARCEL_TYPE);

  PARCELABLE_INTERFACE_TYPE = new ParcelableInterfaceType(this);
  Add(PARCELABLE_INTERFACE_TYPE);

  CONTEXT_TYPE = new Type(this, "android.content", "Context", Type::BUILT_IN,
                          false, false);
  Add(CONTEXT_TYPE);

  CLASSLOADER_TYPE = new ClassLoaderType(this);
  Add(CLASSLOADER_TYPE);

  NULL_VALUE = new LiteralExpression("null");
  THIS_VALUE = new LiteralExpression("this");
  SUPER_VALUE = new LiteralExpression("super");
  TRUE_VALUE = new LiteralExpression("true");
  FALSE_VALUE = new LiteralExpression("false");

  AddGenericType("java.util", "List", 1);
  AddGenericType("java.util", "Map", 2);
}

JavaTypeNamespace::~JavaTypeNamespace() {
  int N = m_types.size();
  for (int i = 0; i < N; i++) {
    delete m_types[i];
  }
}

bool JavaTypeNamespace::Add(const Type* type) {
  const Type* existing = Find(type->QualifiedName());
  if (!existing) {
    m_types.push_back(type);
    return true;
  }

  if (existing->Kind() == Type::BUILT_IN) {
    fprintf(stderr, "%s:%d attempt to redefine built in class %s\n",
            type->DeclFile().c_str(), type->DeclLine(),
            type->QualifiedName().c_str());
    return false;
  }

  if (type->Kind() != existing->Kind()) {
    fprintf(stderr, "%s:%d attempt to redefine %s as %s,\n",
            type->DeclFile().c_str(), type->DeclLine(),
            type->QualifiedName().c_str(), type->HumanReadableKind().c_str());
    fprintf(stderr, "%s:%d previously defined here as %s.\n",
            existing->DeclFile().c_str(), existing->DeclLine(),
            existing->HumanReadableKind().c_str());
    return false;
  }

  return true;
}

void JavaTypeNamespace::AddGenericType(const string& package,
                                       const string& name, int args) {
  Generic g;
  g.package = package;
  g.name = name;
  g.qualified = package + '.' + name;
  g.args = args;
  m_generics.push_back(g);
}

const Type* JavaTypeNamespace::Find(const string& name) const {
  int N = m_types.size();
  for (int i = 0; i < N; i++) {
    if (m_types[i]->QualifiedName() == name) {
      return m_types[i];
    }
  }
  return NULL;
}

const Type* JavaTypeNamespace::Find(const char* package,
                                    const char* name) const {
  string s;
  if (package != nullptr && *package != '\0') {
    s += package;
    s += '.';
  }
  s += name;
  return Find(s);
}

static string normalize_generic(const string& s) {
  string r;
  int N = s.size();
  for (int i = 0; i < N; i++) {
    char c = s[i];
    if (!isspace(c)) {
      r += c;
    }
  }
  return r;
}

bool JavaTypeNamespace::AddParcelableType(user_data_type* p,
                                          const std::string& filename) {
  Type* type =
      new UserDataType(this, p->package ? p->package : "", p->name.data, false,
                       p->parcelable, filename, p->name.lineno);
  return Add(type);
}

bool JavaTypeNamespace::AddBinderType(interface_type* b,
                                      const std::string& filename) {
  // for interfaces, add the stub, proxy, and interface types.
  Type* type =
      new InterfaceType(this, b->package ? b->package : "", b->name.data, false,
                        b->oneway, filename, b->name.lineno);
  Type* stub = new Type(this, b->package ? b->package : "",
                        string{b->name.data} + ".Stub", Type::GENERATED, false,
                        false, filename, b->name.lineno);
  Type* proxy = new Type(this, b->package ? b->package : "",
                         string{b->name.data} + ".Stub.Proxy", Type::GENERATED,
                         false, false, filename, b->name.lineno);

  bool success = true;
  success &= Add(type);
  success &= Add(stub);
  success &= Add(proxy);
  return success;
}

const Type* JavaTypeNamespace::Search(const string& name) {
  // an exact match wins
  const Type* result = Find(name);
  if (result != NULL) {
    return result;
  }

  // try the class names
  // our language doesn't allow you to not specify outer classes
  // when referencing an inner class.  that could be changed, and this
  // would be the place to do it, but I don't think the complexity in
  // scoping rules is worth it.
  int N = m_types.size();
  for (int i = 0; i < N; i++) {
    if (m_types[i]->Name() == name) {
      return m_types[i];
    }
  }

  // we got to here and it's not a generic, give up
  if (name.find('<') == name.npos) {
    return NULL;
  }

  // remove any whitespace
  string normalized = normalize_generic(name);

  // find the part before the '<', find a generic for it
  ssize_t baseIndex = normalized.find('<');
  string base(normalized.c_str(), baseIndex);
  const Generic* g = search_generic(base);
  if (g == NULL) {
    return NULL;
  }

  // For each of the args, do a recursive search on it.  We don't allow
  // generics within generics like Java does, because we're really limiting
  // them to just built-in container classes, at least for now.  Our syntax
  // ensures this right now as well.
  vector<const Type*> args;
  size_t start = baseIndex + 1;
  size_t end = start;
  while (normalized[start] != '\0') {
    end = normalized.find(',', start);
    if (end == normalized.npos) {
      end = normalized.find('>', start);
    }
    string s(normalized.c_str() + start, end - start);
    const Type* t = this->Search(s);
    if (t == NULL) {
      LOG(ERROR) << "internal error";
      return NULL;
    }
    args.push_back(t);
    start = end + 1;
  }

  // construct a GenericType, add it to our name set so they always get
  // the same object, and return it.
  result = make_generic_type(this, g->package, g->name, args);
  if (result == NULL) {
    LOG(ERROR) << "internal error";
    return NULL;
  }

  this->Add(result);
  return this->Find(result->QualifiedName());
}

const JavaTypeNamespace::Generic* JavaTypeNamespace::search_generic(
    const string& name) const {
  int N = m_generics.size();

  // first exact match
  for (int i = 0; i < N; i++) {
    const Generic& g = m_generics[i];
    if (g.qualified == name) {
      return &g;
    }
  }

  // then name match
  for (int i = 0; i < N; i++) {
    const Generic& g = m_generics[i];
    if (g.name == name) {
      return &g;
    }
  }

  return NULL;
}

void JavaTypeNamespace::Dump() const {
  int n = m_types.size();
  for (int i = 0; i < n; i++) {
    const Type* t = m_types[i];
    printf("type: package=%s name=%s qualifiedName=%s\n", t->Package().c_str(),
           t->Name().c_str(), t->QualifiedName().c_str());
  }
}

const Type* JavaTypeNamespace::IntType() const { return m_int_type; }

}  // namespace aidl
}  // namespace android
