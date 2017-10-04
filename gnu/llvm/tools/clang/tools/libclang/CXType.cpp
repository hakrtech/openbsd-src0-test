//===- CXTypes.cpp - Implements 'CXTypes' aspect of libclang ------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===--------------------------------------------------------------------===//
//
// This file implements the 'CXTypes' API hooks in the Clang-C library.
//
//===--------------------------------------------------------------------===//

#include "CIndexer.h"
#include "CXCursor.h"
#include "CXString.h"
#include "CXTranslationUnit.h"
#include "CXType.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclObjC.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/Expr.h"
#include "clang/AST/Type.h"
#include "clang/Basic/AddressSpaces.h"
#include "clang/Frontend/ASTUnit.h"

using namespace clang;

static CXTypeKind GetBuiltinTypeKind(const BuiltinType *BT) {
#define BTCASE(K) case BuiltinType::K: return CXType_##K
  switch (BT->getKind()) {
    BTCASE(Void);
    BTCASE(Bool);
    BTCASE(Char_U);
    BTCASE(UChar);
    BTCASE(Char16);
    BTCASE(Char32);
    BTCASE(UShort);
    BTCASE(UInt);
    BTCASE(ULong);
    BTCASE(ULongLong);
    BTCASE(UInt128);
    BTCASE(Char_S);
    BTCASE(SChar);
    case BuiltinType::WChar_S: return CXType_WChar;
    case BuiltinType::WChar_U: return CXType_WChar;
    BTCASE(Short);
    BTCASE(Int);
    BTCASE(Long);
    BTCASE(LongLong);
    BTCASE(Int128);
    BTCASE(Half);
    BTCASE(Float);
    BTCASE(Double);
    BTCASE(LongDouble);
    BTCASE(Float128);
    BTCASE(NullPtr);
    BTCASE(Overload);
    BTCASE(Dependent);
    BTCASE(ObjCId);
    BTCASE(ObjCClass);
    BTCASE(ObjCSel);
#define IMAGE_TYPE(ImgType, Id, SingletonId, Access, Suffix) BTCASE(Id);
#include "clang/Basic/OpenCLImageTypes.def"
#undef IMAGE_TYPE
    BTCASE(OCLSampler);
    BTCASE(OCLEvent);
    BTCASE(OCLQueue);
    BTCASE(OCLReserveID);
  default:
    return CXType_Unexposed;
  }
#undef BTCASE
}

static CXTypeKind GetTypeKind(QualType T) {
  const Type *TP = T.getTypePtrOrNull();
  if (!TP)
    return CXType_Invalid;

#define TKCASE(K) case Type::K: return CXType_##K
  switch (TP->getTypeClass()) {
    case Type::Builtin:
      return GetBuiltinTypeKind(cast<BuiltinType>(TP));
    TKCASE(Complex);
    TKCASE(Pointer);
    TKCASE(BlockPointer);
    TKCASE(LValueReference);
    TKCASE(RValueReference);
    TKCASE(Record);
    TKCASE(Enum);
    TKCASE(Typedef);
    TKCASE(ObjCInterface);
    TKCASE(ObjCObjectPointer);
    TKCASE(FunctionNoProto);
    TKCASE(FunctionProto);
    TKCASE(ConstantArray);
    TKCASE(IncompleteArray);
    TKCASE(VariableArray);
    TKCASE(DependentSizedArray);
    TKCASE(Vector);
    TKCASE(MemberPointer);
    TKCASE(Auto);
    TKCASE(Elaborated);
    TKCASE(Pipe);
    default:
      return CXType_Unexposed;
  }
#undef TKCASE
}


CXType cxtype::MakeCXType(QualType T, CXTranslationUnit TU) {
  CXTypeKind TK = CXType_Invalid;

  if (TU && !T.isNull()) {
    // Handle attributed types as the original type
    if (auto *ATT = T->getAs<AttributedType>()) {
      return MakeCXType(ATT->getModifiedType(), TU);
    }

    ASTContext &Ctx = cxtu::getASTUnit(TU)->getASTContext();
    if (Ctx.getLangOpts().ObjC1) {
      QualType UnqualT = T.getUnqualifiedType();
      if (Ctx.isObjCIdType(UnqualT))
        TK = CXType_ObjCId;
      else if (Ctx.isObjCClassType(UnqualT))
        TK = CXType_ObjCClass;
      else if (Ctx.isObjCSelType(UnqualT))
        TK = CXType_ObjCSel;
    }

    /* Handle decayed types as the original type */
    if (const DecayedType *DT = T->getAs<DecayedType>()) {
      return MakeCXType(DT->getOriginalType(), TU);
    }
  }
  if (TK == CXType_Invalid)
    TK = GetTypeKind(T);

  CXType CT = { TK, { TK == CXType_Invalid ? nullptr
                                           : T.getAsOpaquePtr(), TU } };
  return CT;
}

using cxtype::MakeCXType;

static inline QualType GetQualType(CXType CT) {
  return QualType::getFromOpaquePtr(CT.data[0]);
}

static inline CXTranslationUnit GetTU(CXType CT) {
  return static_cast<CXTranslationUnit>(CT.data[1]);
}

static Optional<ArrayRef<TemplateArgument>>
GetTemplateArguments(QualType Type) {
  assert(!Type.isNull());
  if (const auto *Specialization = Type->getAs<TemplateSpecializationType>())
    return Specialization->template_arguments();

  if (const auto *RecordDecl = Type->getAsCXXRecordDecl()) {
    const auto *TemplateDecl =
      dyn_cast<ClassTemplateSpecializationDecl>(RecordDecl);
    if (TemplateDecl)
      return TemplateDecl->getTemplateArgs().asArray();
  }

  return None;
}

static Optional<QualType> TemplateArgumentToQualType(const TemplateArgument &A) {
  if (A.getKind() == TemplateArgument::Type)
    return A.getAsType();
  return None;
}

static Optional<QualType>
FindTemplateArgumentTypeAt(ArrayRef<TemplateArgument> TA, unsigned index) {
  unsigned current = 0;
  for (const auto &A : TA) {
    if (A.getKind() == TemplateArgument::Pack) {
      if (index < current + A.pack_size())
        return TemplateArgumentToQualType(A.getPackAsArray()[index - current]);
      current += A.pack_size();
      continue;
    }
    if (current == index)
      return TemplateArgumentToQualType(A);
    current++;
  }
  return None;
}

CXType clang_getCursorType(CXCursor C) {
  using namespace cxcursor;

  CXTranslationUnit TU = cxcursor::getCursorTU(C);
  if (!TU)
    return MakeCXType(QualType(), TU);

  ASTContext &Context = cxtu::getASTUnit(TU)->getASTContext();
  if (clang_isExpression(C.kind)) {
    QualType T = cxcursor::getCursorExpr(C)->getType();
    return MakeCXType(T, TU);
  }

  if (clang_isDeclaration(C.kind)) {
    const Decl *D = cxcursor::getCursorDecl(C);
    if (!D)
      return MakeCXType(QualType(), TU);

    if (const TypeDecl *TD = dyn_cast<TypeDecl>(D))
      return MakeCXType(Context.getTypeDeclType(TD), TU);
    if (const ObjCInterfaceDecl *ID = dyn_cast<ObjCInterfaceDecl>(D))
      return MakeCXType(Context.getObjCInterfaceType(ID), TU);
    if (const DeclaratorDecl *DD = dyn_cast<DeclaratorDecl>(D))
      return MakeCXType(DD->getType(), TU);
    if (const ValueDecl *VD = dyn_cast<ValueDecl>(D))
      return MakeCXType(VD->getType(), TU);
    if (const ObjCPropertyDecl *PD = dyn_cast<ObjCPropertyDecl>(D))
      return MakeCXType(PD->getType(), TU);
    if (const FunctionTemplateDecl *FTD = dyn_cast<FunctionTemplateDecl>(D))
      return MakeCXType(FTD->getTemplatedDecl()->getType(), TU);
    return MakeCXType(QualType(), TU);
  }

  if (clang_isReference(C.kind)) {
    switch (C.kind) {
    case CXCursor_ObjCSuperClassRef: {
      QualType T
        = Context.getObjCInterfaceType(getCursorObjCSuperClassRef(C).first);
      return MakeCXType(T, TU);
    }

    case CXCursor_ObjCClassRef: {
      QualType T = Context.getObjCInterfaceType(getCursorObjCClassRef(C).first);
      return MakeCXType(T, TU);
    }

    case CXCursor_TypeRef: {
      QualType T = Context.getTypeDeclType(getCursorTypeRef(C).first);
      return MakeCXType(T, TU);

    }

    case CXCursor_CXXBaseSpecifier:
      return cxtype::MakeCXType(getCursorCXXBaseSpecifier(C)->getType(), TU);

    case CXCursor_MemberRef:
      return cxtype::MakeCXType(getCursorMemberRef(C).first->getType(), TU);

    case CXCursor_VariableRef:
      return cxtype::MakeCXType(getCursorVariableRef(C).first->getType(), TU);

    case CXCursor_ObjCProtocolRef:
    case CXCursor_TemplateRef:
    case CXCursor_NamespaceRef:
    case CXCursor_OverloadedDeclRef:
    default:
      break;
    }

    return MakeCXType(QualType(), TU);
  }

  return MakeCXType(QualType(), TU);
}

CXString clang_getTypeSpelling(CXType CT) {
  QualType T = GetQualType(CT);
  if (T.isNull())
    return cxstring::createEmpty();

  CXTranslationUnit TU = GetTU(CT);
  SmallString<64> Str;
  llvm::raw_svector_ostream OS(Str);
  PrintingPolicy PP(cxtu::getASTUnit(TU)->getASTContext().getLangOpts());

  T.print(OS, PP);

  return cxstring::createDup(OS.str());
}

CXType clang_getTypedefDeclUnderlyingType(CXCursor C) {
  using namespace cxcursor;
  CXTranslationUnit TU = cxcursor::getCursorTU(C);

  if (clang_isDeclaration(C.kind)) {
    const Decl *D = cxcursor::getCursorDecl(C);

    if (const TypedefNameDecl *TD = dyn_cast_or_null<TypedefNameDecl>(D)) {
      QualType T = TD->getUnderlyingType();
      return MakeCXType(T, TU);
    }

    return MakeCXType(QualType(), TU);
  }

  return MakeCXType(QualType(), TU);
}

CXType clang_getEnumDeclIntegerType(CXCursor C) {
  using namespace cxcursor;
  CXTranslationUnit TU = cxcursor::getCursorTU(C);

  if (clang_isDeclaration(C.kind)) {
    const Decl *D = cxcursor::getCursorDecl(C);

    if (const EnumDecl *TD = dyn_cast_or_null<EnumDecl>(D)) {
      QualType T = TD->getIntegerType();
      return MakeCXType(T, TU);
    }

    return MakeCXType(QualType(), TU);
  }

  return MakeCXType(QualType(), TU);
}

long long clang_getEnumConstantDeclValue(CXCursor C) {
  using namespace cxcursor;

  if (clang_isDeclaration(C.kind)) {
    const Decl *D = cxcursor::getCursorDecl(C);

    if (const EnumConstantDecl *TD = dyn_cast_or_null<EnumConstantDecl>(D)) {
      return TD->getInitVal().getSExtValue();
    }

    return LLONG_MIN;
  }

  return LLONG_MIN;
}

unsigned long long clang_getEnumConstantDeclUnsignedValue(CXCursor C) {
  using namespace cxcursor;

  if (clang_isDeclaration(C.kind)) {
    const Decl *D = cxcursor::getCursorDecl(C);

    if (const EnumConstantDecl *TD = dyn_cast_or_null<EnumConstantDecl>(D)) {
      return TD->getInitVal().getZExtValue();
    }

    return ULLONG_MAX;
  }

  return ULLONG_MAX;
}

int clang_getFieldDeclBitWidth(CXCursor C) {
  using namespace cxcursor;

  if (clang_isDeclaration(C.kind)) {
    const Decl *D = getCursorDecl(C);

    if (const FieldDecl *FD = dyn_cast_or_null<FieldDecl>(D)) {
      if (FD->isBitField())
        return FD->getBitWidthValue(getCursorContext(C));
    }
  }

  return -1;
}

CXType clang_getCanonicalType(CXType CT) {
  if (CT.kind == CXType_Invalid)
    return CT;

  QualType T = GetQualType(CT);
  CXTranslationUnit TU = GetTU(CT);

  if (T.isNull())
    return MakeCXType(QualType(), GetTU(CT));

  return MakeCXType(cxtu::getASTUnit(TU)->getASTContext()
                        .getCanonicalType(T),
                    TU);
}

unsigned clang_isConstQualifiedType(CXType CT) {
  QualType T = GetQualType(CT);
  return T.isLocalConstQualified();
}

unsigned clang_isVolatileQualifiedType(CXType CT) {
  QualType T = GetQualType(CT);
  return T.isLocalVolatileQualified();
}

unsigned clang_isRestrictQualifiedType(CXType CT) {
  QualType T = GetQualType(CT);
  return T.isLocalRestrictQualified();
}

unsigned clang_getAddressSpace(CXType CT) {
  QualType T = GetQualType(CT);

  // For non language-specific address space, use separate helper function.
  if (T.getAddressSpace() >= LangAS::FirstTargetAddressSpace) {
    return T.getQualifiers().getAddressSpaceAttributePrintValue();
  }
  return T.getAddressSpace();
}

CXString clang_getTypedefName(CXType CT) {
  QualType T = GetQualType(CT);
  const TypedefType *TT = T->getAs<TypedefType>();
  if (TT) {
    TypedefNameDecl *TD = TT->getDecl();
    if (TD)
      return cxstring::createDup(TD->getNameAsString().c_str());
  }
  return cxstring::createEmpty();
}

CXType clang_getPointeeType(CXType CT) {
  QualType T = GetQualType(CT);
  const Type *TP = T.getTypePtrOrNull();

  if (!TP)
    return MakeCXType(QualType(), GetTU(CT));

  switch (TP->getTypeClass()) {
    case Type::Pointer:
      T = cast<PointerType>(TP)->getPointeeType();
      break;
    case Type::BlockPointer:
      T = cast<BlockPointerType>(TP)->getPointeeType();
      break;
    case Type::LValueReference:
    case Type::RValueReference:
      T = cast<ReferenceType>(TP)->getPointeeType();
      break;
    case Type::ObjCObjectPointer:
      T = cast<ObjCObjectPointerType>(TP)->getPointeeType();
      break;
    case Type::MemberPointer:
      T = cast<MemberPointerType>(TP)->getPointeeType();
      break;
    default:
      T = QualType();
      break;
  }
  return MakeCXType(T, GetTU(CT));
}

CXCursor clang_getTypeDeclaration(CXType CT) {
  if (CT.kind == CXType_Invalid)
    return cxcursor::MakeCXCursorInvalid(CXCursor_NoDeclFound);

  QualType T = GetQualType(CT);
  const Type *TP = T.getTypePtrOrNull();

  if (!TP)
    return cxcursor::MakeCXCursorInvalid(CXCursor_NoDeclFound);

  Decl *D = nullptr;

try_again:
  switch (TP->getTypeClass()) {
  case Type::Typedef:
    D = cast<TypedefType>(TP)->getDecl();
    break;
  case Type::ObjCObject:
    D = cast<ObjCObjectType>(TP)->getInterface();
    break;
  case Type::ObjCInterface:
    D = cast<ObjCInterfaceType>(TP)->getDecl();
    break;
  case Type::Record:
  case Type::Enum:
    D = cast<TagType>(TP)->getDecl();
    break;
  case Type::TemplateSpecialization:
    if (const RecordType *Record = TP->getAs<RecordType>())
      D = Record->getDecl();
    else
      D = cast<TemplateSpecializationType>(TP)->getTemplateName()
                                                         .getAsTemplateDecl();
    break;

  case Type::Auto:
  case Type::DeducedTemplateSpecialization:
    TP = cast<DeducedType>(TP)->getDeducedType().getTypePtrOrNull();
    if (TP)
      goto try_again;
    break;

  case Type::InjectedClassName:
    D = cast<InjectedClassNameType>(TP)->getDecl();
    break;

  // FIXME: Template type parameters!      

  case Type::Elaborated:
    TP = cast<ElaboratedType>(TP)->getNamedType().getTypePtrOrNull();
    goto try_again;

  default:
    break;
  }

  if (!D)
    return cxcursor::MakeCXCursorInvalid(CXCursor_NoDeclFound);

  return cxcursor::MakeCXCursor(D, GetTU(CT));
}

CXString clang_getTypeKindSpelling(enum CXTypeKind K) {
  const char *s = nullptr;
#define TKIND(X) case CXType_##X: s = ""  #X  ""; break
  switch (K) {
    TKIND(Invalid);
    TKIND(Unexposed);
    TKIND(Void);
    TKIND(Bool);
    TKIND(Char_U);
    TKIND(UChar);
    TKIND(Char16);
    TKIND(Char32);  
    TKIND(UShort);
    TKIND(UInt);
    TKIND(ULong);
    TKIND(ULongLong);
    TKIND(UInt128);
    TKIND(Char_S);
    TKIND(SChar);
    case CXType_WChar: s = "WChar"; break;
    TKIND(Short);
    TKIND(Int);
    TKIND(Long);
    TKIND(LongLong);
    TKIND(Int128);
    TKIND(Half);
    TKIND(Float);
    TKIND(Double);
    TKIND(LongDouble);
    TKIND(Float128);
    TKIND(NullPtr);
    TKIND(Overload);
    TKIND(Dependent);
    TKIND(ObjCId);
    TKIND(ObjCClass);
    TKIND(ObjCSel);
    TKIND(Complex);
    TKIND(Pointer);
    TKIND(BlockPointer);
    TKIND(LValueReference);
    TKIND(RValueReference);
    TKIND(Record);
    TKIND(Enum);
    TKIND(Typedef);
    TKIND(ObjCInterface);
    TKIND(ObjCObjectPointer);
    TKIND(FunctionNoProto);
    TKIND(FunctionProto);
    TKIND(ConstantArray);
    TKIND(IncompleteArray);
    TKIND(VariableArray);
    TKIND(DependentSizedArray);
    TKIND(Vector);
    TKIND(MemberPointer);
    TKIND(Auto);
    TKIND(Elaborated);
    TKIND(Pipe);
#define IMAGE_TYPE(ImgType, Id, SingletonId, Access, Suffix) TKIND(Id);
#include "clang/Basic/OpenCLImageTypes.def"
#undef IMAGE_TYPE
    TKIND(OCLSampler);
    TKIND(OCLEvent);
    TKIND(OCLQueue);
    TKIND(OCLReserveID);
  }
#undef TKIND
  return cxstring::createRef(s);
}

unsigned clang_equalTypes(CXType A, CXType B) {
  return A.data[0] == B.data[0] && A.data[1] == B.data[1];
}

unsigned clang_isFunctionTypeVariadic(CXType X) {
  QualType T = GetQualType(X);
  if (T.isNull())
    return 0;

  if (const FunctionProtoType *FD = T->getAs<FunctionProtoType>())
    return (unsigned)FD->isVariadic();

  if (T->getAs<FunctionNoProtoType>())
    return 1;
  
  return 0;
}

CXCallingConv clang_getFunctionTypeCallingConv(CXType X) {
  QualType T = GetQualType(X);
  if (T.isNull())
    return CXCallingConv_Invalid;
  
  if (const FunctionType *FD = T->getAs<FunctionType>()) {
#define TCALLINGCONV(X) case CC_##X: return CXCallingConv_##X
    switch (FD->getCallConv()) {
      TCALLINGCONV(C);
      TCALLINGCONV(X86StdCall);
      TCALLINGCONV(X86FastCall);
      TCALLINGCONV(X86ThisCall);
      TCALLINGCONV(X86Pascal);
      TCALLINGCONV(X86RegCall);
      TCALLINGCONV(X86VectorCall);
      TCALLINGCONV(Win64);
      TCALLINGCONV(X86_64SysV);
      TCALLINGCONV(AAPCS);
      TCALLINGCONV(AAPCS_VFP);
      TCALLINGCONV(IntelOclBicc);
      TCALLINGCONV(Swift);
      TCALLINGCONV(PreserveMost);
      TCALLINGCONV(PreserveAll);
    case CC_SpirFunction: return CXCallingConv_Unexposed;
    case CC_OpenCLKernel: return CXCallingConv_Unexposed;
      break;
    }
#undef TCALLINGCONV
  }
  
  return CXCallingConv_Invalid;
}

int clang_getNumArgTypes(CXType X) {
  QualType T = GetQualType(X);
  if (T.isNull())
    return -1;
  
  if (const FunctionProtoType *FD = T->getAs<FunctionProtoType>()) {
    return FD->getNumParams();
  }
  
  if (T->getAs<FunctionNoProtoType>()) {
    return 0;
  }
  
  return -1;
}

CXType clang_getArgType(CXType X, unsigned i) {
  QualType T = GetQualType(X);
  if (T.isNull())
    return MakeCXType(QualType(), GetTU(X));

  if (const FunctionProtoType *FD = T->getAs<FunctionProtoType>()) {
    unsigned numParams = FD->getNumParams();
    if (i >= numParams)
      return MakeCXType(QualType(), GetTU(X));

    return MakeCXType(FD->getParamType(i), GetTU(X));
  }
  
  return MakeCXType(QualType(), GetTU(X));
}

CXType clang_getResultType(CXType X) {
  QualType T = GetQualType(X);
  if (T.isNull())
    return MakeCXType(QualType(), GetTU(X));
  
  if (const FunctionType *FD = T->getAs<FunctionType>())
    return MakeCXType(FD->getReturnType(), GetTU(X));

  return MakeCXType(QualType(), GetTU(X));
}

CXType clang_getCursorResultType(CXCursor C) {
  if (clang_isDeclaration(C.kind)) {
    const Decl *D = cxcursor::getCursorDecl(C);
    if (const ObjCMethodDecl *MD = dyn_cast_or_null<ObjCMethodDecl>(D))
      return MakeCXType(MD->getReturnType(), cxcursor::getCursorTU(C));

    return clang_getResultType(clang_getCursorType(C));
  }

  return MakeCXType(QualType(), cxcursor::getCursorTU(C));
}

int clang_getExceptionSpecificationType(CXType X) {
  QualType T = GetQualType(X);
  if (T.isNull())
    return -1;

  if (const auto *FD = T->getAs<FunctionProtoType>())
    return static_cast<int>(FD->getExceptionSpecType());

  return -1;
}

int clang_getCursorExceptionSpecificationType(CXCursor C) {
  if (clang_isDeclaration(C.kind))
    return clang_getExceptionSpecificationType(clang_getCursorType(C));

  return -1;
}

unsigned clang_isPODType(CXType X) {
  QualType T = GetQualType(X);
  if (T.isNull())
    return 0;
  
  CXTranslationUnit TU = GetTU(X);

  return T.isPODType(cxtu::getASTUnit(TU)->getASTContext()) ? 1 : 0;
}

CXType clang_getElementType(CXType CT) {
  QualType ET = QualType();
  QualType T = GetQualType(CT);
  const Type *TP = T.getTypePtrOrNull();

  if (TP) {
    switch (TP->getTypeClass()) {
    case Type::ConstantArray:
      ET = cast<ConstantArrayType> (TP)->getElementType();
      break;
    case Type::IncompleteArray:
      ET = cast<IncompleteArrayType> (TP)->getElementType();
      break;
    case Type::VariableArray:
      ET = cast<VariableArrayType> (TP)->getElementType();
      break;
    case Type::DependentSizedArray:
      ET = cast<DependentSizedArrayType> (TP)->getElementType();
      break;
    case Type::Vector:
      ET = cast<VectorType> (TP)->getElementType();
      break;
    case Type::Complex:
      ET = cast<ComplexType> (TP)->getElementType();
      break;
    default:
      break;
    }
  }
  return MakeCXType(ET, GetTU(CT));
}

long long clang_getNumElements(CXType CT) {
  long long result = -1;
  QualType T = GetQualType(CT);
  const Type *TP = T.getTypePtrOrNull();

  if (TP) {
    switch (TP->getTypeClass()) {
    case Type::ConstantArray:
      result = cast<ConstantArrayType> (TP)->getSize().getSExtValue();
      break;
    case Type::Vector:
      result = cast<VectorType> (TP)->getNumElements();
      break;
    default:
      break;
    }
  }
  return result;
}

CXType clang_getArrayElementType(CXType CT) {
  QualType ET = QualType();
  QualType T = GetQualType(CT);
  const Type *TP = T.getTypePtrOrNull();

  if (TP) {
    switch (TP->getTypeClass()) {
    case Type::ConstantArray:
      ET = cast<ConstantArrayType> (TP)->getElementType();
      break;
    case Type::IncompleteArray:
      ET = cast<IncompleteArrayType> (TP)->getElementType();
      break;
    case Type::VariableArray:
      ET = cast<VariableArrayType> (TP)->getElementType();
      break;
    case Type::DependentSizedArray:
      ET = cast<DependentSizedArrayType> (TP)->getElementType();
      break;
    default:
      break;
    }
  }
  return MakeCXType(ET, GetTU(CT));
}

long long clang_getArraySize(CXType CT) {
  long long result = -1;
  QualType T = GetQualType(CT);
  const Type *TP = T.getTypePtrOrNull();

  if (TP) {
    switch (TP->getTypeClass()) {
    case Type::ConstantArray:
      result = cast<ConstantArrayType> (TP)->getSize().getSExtValue();
      break;
    default:
      break;
    }
  }
  return result;
}

long long clang_Type_getAlignOf(CXType T) {
  if (T.kind == CXType_Invalid)
    return CXTypeLayoutError_Invalid;
  ASTContext &Ctx = cxtu::getASTUnit(GetTU(T))->getASTContext();
  QualType QT = GetQualType(T);
  // [expr.alignof] p1: return size_t value for complete object type, reference
  //                    or array.
  // [expr.alignof] p3: if reference type, return size of referenced type
  if (QT->isReferenceType())
    QT = QT.getNonReferenceType();
  if (QT->isIncompleteType())
    return CXTypeLayoutError_Incomplete;
  if (QT->isDependentType())
    return CXTypeLayoutError_Dependent;
  // Exceptions by GCC extension - see ASTContext.cpp:1313 getTypeInfoImpl
  // if (QT->isFunctionType()) return 4; // Bug #15511 - should be 1
  // if (QT->isVoidType()) return 1;
  return Ctx.getTypeAlignInChars(QT).getQuantity();
}

CXType clang_Type_getClassType(CXType CT) {
  QualType ET = QualType();
  QualType T = GetQualType(CT);
  const Type *TP = T.getTypePtrOrNull();

  if (TP && TP->getTypeClass() == Type::MemberPointer) {
    ET = QualType(cast<MemberPointerType> (TP)->getClass(), 0);
  }
  return MakeCXType(ET, GetTU(CT));
}

long long clang_Type_getSizeOf(CXType T) {
  if (T.kind == CXType_Invalid)
    return CXTypeLayoutError_Invalid;
  ASTContext &Ctx = cxtu::getASTUnit(GetTU(T))->getASTContext();
  QualType QT = GetQualType(T);
  // [expr.sizeof] p2: if reference type, return size of referenced type
  if (QT->isReferenceType())
    QT = QT.getNonReferenceType();
  // [expr.sizeof] p1: return -1 on: func, incomplete, bitfield, incomplete
  //                   enumeration
  // Note: We get the cxtype, not the cxcursor, so we can't call
  //       FieldDecl->isBitField()
  // [expr.sizeof] p3: pointer ok, function not ok.
  // [gcc extension] lib/AST/ExprConstant.cpp:1372 HandleSizeof : vla == error
  if (QT->isIncompleteType())
    return CXTypeLayoutError_Incomplete;
  if (QT->isDependentType())
    return CXTypeLayoutError_Dependent;
  if (!QT->isConstantSizeType())
    return CXTypeLayoutError_NotConstantSize;
  // [gcc extension] lib/AST/ExprConstant.cpp:1372
  //                 HandleSizeof : {voidtype,functype} == 1
  // not handled by ASTContext.cpp:1313 getTypeInfoImpl
  if (QT->isVoidType() || QT->isFunctionType())
    return 1;
  return Ctx.getTypeSizeInChars(QT).getQuantity();
}

static long long visitRecordForValidation(const RecordDecl *RD) {
  for (const auto *I : RD->fields()){
    QualType FQT = I->getType();
    if (FQT->isIncompleteType())
      return CXTypeLayoutError_Incomplete;
    if (FQT->isDependentType())
      return CXTypeLayoutError_Dependent;
    // recurse
    if (const RecordType *ChildType = I->getType()->getAs<RecordType>()) {
      if (const RecordDecl *Child = ChildType->getDecl()) {
        long long ret = visitRecordForValidation(Child);
        if (ret < 0)
          return ret;
      }
    }
    // else try next field
  }
  return 0;
}

static long long validateFieldParentType(CXCursor PC, CXType PT){
  if (clang_isInvalid(PC.kind))
    return CXTypeLayoutError_Invalid;
  const RecordDecl *RD =
        dyn_cast_or_null<RecordDecl>(cxcursor::getCursorDecl(PC));
  // validate parent declaration
  if (!RD || RD->isInvalidDecl())
    return CXTypeLayoutError_Invalid;
  RD = RD->getDefinition();
  if (!RD)
    return CXTypeLayoutError_Incomplete;
  if (RD->isInvalidDecl())
    return CXTypeLayoutError_Invalid;
  // validate parent type
  QualType RT = GetQualType(PT);
  if (RT->isIncompleteType())
    return CXTypeLayoutError_Incomplete;
  if (RT->isDependentType())
    return CXTypeLayoutError_Dependent;
  // We recurse into all record fields to detect incomplete and dependent types.
  long long Error = visitRecordForValidation(RD);
  if (Error < 0)
    return Error;
  return 0;
}

long long clang_Type_getOffsetOf(CXType PT, const char *S) {
  // check that PT is not incomplete/dependent
  CXCursor PC = clang_getTypeDeclaration(PT);
  long long Error = validateFieldParentType(PC,PT);
  if (Error < 0)
    return Error;
  if (!S)
    return CXTypeLayoutError_InvalidFieldName;
  // lookup field
  ASTContext &Ctx = cxtu::getASTUnit(GetTU(PT))->getASTContext();
  IdentifierInfo *II = &Ctx.Idents.get(S);
  DeclarationName FieldName(II);
  const RecordDecl *RD =
        dyn_cast_or_null<RecordDecl>(cxcursor::getCursorDecl(PC));
  // verified in validateFieldParentType
  RD = RD->getDefinition();
  RecordDecl::lookup_result Res = RD->lookup(FieldName);
  // If a field of the parent record is incomplete, lookup will fail.
  // and we would return InvalidFieldName instead of Incomplete.
  // But this erroneous results does protects again a hidden assertion failure
  // in the RecordLayoutBuilder
  if (Res.size() != 1)
    return CXTypeLayoutError_InvalidFieldName;
  if (const FieldDecl *FD = dyn_cast<FieldDecl>(Res.front()))
    return Ctx.getFieldOffset(FD);
  if (const IndirectFieldDecl *IFD = dyn_cast<IndirectFieldDecl>(Res.front()))
    return Ctx.getFieldOffset(IFD);
  // we don't want any other Decl Type.
  return CXTypeLayoutError_InvalidFieldName;
}

long long clang_Cursor_getOffsetOfField(CXCursor C) {
  if (clang_isDeclaration(C.kind)) {
    // we need to validate the parent type
    CXCursor PC = clang_getCursorSemanticParent(C);
    CXType PT = clang_getCursorType(PC);
    long long Error = validateFieldParentType(PC,PT);
    if (Error < 0)
      return Error;
    // proceed with the offset calculation
    const Decl *D = cxcursor::getCursorDecl(C);
    ASTContext &Ctx = cxcursor::getCursorContext(C);
    if (const FieldDecl *FD = dyn_cast_or_null<FieldDecl>(D))
      return Ctx.getFieldOffset(FD);
    if (const IndirectFieldDecl *IFD = dyn_cast_or_null<IndirectFieldDecl>(D))
      return Ctx.getFieldOffset(IFD);
  }
  return -1;
}

enum CXRefQualifierKind clang_Type_getCXXRefQualifier(CXType T) {
  QualType QT = GetQualType(T);
  if (QT.isNull())
    return CXRefQualifier_None;
  const FunctionProtoType *FD = QT->getAs<FunctionProtoType>();
  if (!FD)
    return CXRefQualifier_None;
  switch (FD->getRefQualifier()) {
    case RQ_None:
      return CXRefQualifier_None;
    case RQ_LValue:
      return CXRefQualifier_LValue;
    case RQ_RValue:
      return CXRefQualifier_RValue;
  }
  return CXRefQualifier_None;
}

unsigned clang_Cursor_isBitField(CXCursor C) {
  if (!clang_isDeclaration(C.kind))
    return 0;
  const FieldDecl *FD = dyn_cast_or_null<FieldDecl>(cxcursor::getCursorDecl(C));
  if (!FD)
    return 0;
  return FD->isBitField();
}

CXString clang_getDeclObjCTypeEncoding(CXCursor C) {
  if (!clang_isDeclaration(C.kind))
    return cxstring::createEmpty();

  const Decl *D = cxcursor::getCursorDecl(C);
  ASTContext &Ctx = cxcursor::getCursorContext(C);
  std::string encoding;

  if (const ObjCMethodDecl *OMD = dyn_cast<ObjCMethodDecl>(D))  {
    encoding = Ctx.getObjCEncodingForMethodDecl(OMD);
  } else if (const ObjCPropertyDecl *OPD = dyn_cast<ObjCPropertyDecl>(D))
    encoding = Ctx.getObjCEncodingForPropertyDecl(OPD, nullptr);
  else if (const FunctionDecl *FD = dyn_cast<FunctionDecl>(D))
    encoding = Ctx.getObjCEncodingForFunctionDecl(FD);
  else {
    QualType Ty;
    if (const TypeDecl *TD = dyn_cast<TypeDecl>(D))
      Ty = Ctx.getTypeDeclType(TD);
    if (const ValueDecl *VD = dyn_cast<ValueDecl>(D))
      Ty = VD->getType();
    else return cxstring::createRef("?");
    Ctx.getObjCEncodingForType(Ty, encoding);
  }

  return cxstring::createDup(encoding);
}

static unsigned GetTemplateArgumentArraySize(ArrayRef<TemplateArgument> TA) {
  unsigned size = TA.size();
  for (const auto &Arg : TA)
    if (Arg.getKind() == TemplateArgument::Pack)
      size += Arg.pack_size() - 1;
  return size;
}

int clang_Type_getNumTemplateArguments(CXType CT) {
  QualType T = GetQualType(CT);
  if (T.isNull())
    return -1;

  auto TA = GetTemplateArguments(T);
  if (!TA)
    return -1;

  return GetTemplateArgumentArraySize(TA.getValue());
}

CXType clang_Type_getTemplateArgumentAsType(CXType CT, unsigned index) {
  QualType T = GetQualType(CT);
  if (T.isNull())
    return MakeCXType(QualType(), GetTU(CT));

  auto TA = GetTemplateArguments(T);
  if (!TA)
    return MakeCXType(QualType(), GetTU(CT));

  Optional<QualType> QT = FindTemplateArgumentTypeAt(TA.getValue(), index);
  return MakeCXType(QT.getValueOr(QualType()), GetTU(CT));
}

unsigned clang_Type_visitFields(CXType PT,
                                CXFieldVisitor visitor,
                                CXClientData client_data){
  CXCursor PC = clang_getTypeDeclaration(PT);
  if (clang_isInvalid(PC.kind))
    return false;
  const RecordDecl *RD =
        dyn_cast_or_null<RecordDecl>(cxcursor::getCursorDecl(PC));
  if (!RD || RD->isInvalidDecl())
    return false;
  RD = RD->getDefinition();
  if (!RD || RD->isInvalidDecl())
    return false;

  for (RecordDecl::field_iterator I = RD->field_begin(), E = RD->field_end();
       I != E; ++I){
    const FieldDecl *FD = dyn_cast_or_null<FieldDecl>((*I));
    // Callback to the client.
    switch (visitor(cxcursor::MakeCXCursor(FD, GetTU(PT)), client_data)){
    case CXVisit_Break:
      return true;
    case CXVisit_Continue:
      break;
    }
  }
  return true;
}

unsigned clang_Cursor_isAnonymous(CXCursor C){
  if (!clang_isDeclaration(C.kind))
    return 0;
  const Decl *D = cxcursor::getCursorDecl(C);
  if (const RecordDecl *FD = dyn_cast_or_null<RecordDecl>(D))
    return FD->isAnonymousStructOrUnion();
  return 0;
}

CXType clang_Type_getNamedType(CXType CT){
  QualType T = GetQualType(CT);
  const Type *TP = T.getTypePtrOrNull();

  if (TP && TP->getTypeClass() == Type::Elaborated)
    return MakeCXType(cast<ElaboratedType>(TP)->getNamedType(), GetTU(CT));

  return MakeCXType(QualType(), GetTU(CT));
}

unsigned clang_Type_isTransparentTagTypedef(CXType TT){
  QualType T = GetQualType(TT);
  if (auto *TT = dyn_cast_or_null<TypedefType>(T.getTypePtrOrNull())) {
    if (auto *D = TT->getDecl())
      return D->isTransparentTag();
  }
  return false;
}
