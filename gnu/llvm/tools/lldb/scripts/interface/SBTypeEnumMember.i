//===-- SWIG Interface for SBTypeEnumMember ---------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

namespace lldb {

%feature(
    "docstring",
    "Represents a member of an enum in lldb."
) SBTypeEnumMember;

class SBTypeEnumMember
{
public:
    SBTypeEnumMember ();

    SBTypeEnumMember (const SBTypeEnumMember& rhs);

    ~SBTypeEnumMember();

    bool
    IsValid() const;

    int64_t
    GetValueAsSigned();

    uint64_t
    GetValueAsUnsigned();

    const char *
    GetName ();

    lldb::SBType
    GetType ();

    bool
    GetDescription (lldb::SBStream &description,
                    lldb::DescriptionLevel description_level);

    %pythoncode %{
        __swig_getmethods__["name"] = GetName
        if _newclass: name = property(GetName, None, doc='''A read only property that returns the name for this enum member as a string.''')

        __swig_getmethods__["type"] = GetType
        if _newclass: type = property(GetType, None, doc='''A read only property that returns an lldb object that represents the type (lldb.SBType) for this enum member.''')

        __swig_getmethods__["signed"] = GetValueAsSigned
        if _newclass: signed = property(GetValueAsSigned, None, doc='''A read only property that returns the value of this enum member as a signed integer.''')

        __swig_getmethods__["unsigned"] = GetValueAsUnsigned
        if _newclass: unsigned = property(GetValueAsUnsigned, None, doc='''A read only property that returns the value of this enum member as a unsigned integer.''')
    %}

protected:
    friend class SBType;
    friend class SBTypeEnumMemberList;

    void
    reset (lldb_private::TypeEnumMemberImpl *);

    lldb_private::TypeEnumMemberImpl &
    ref ();

    const lldb_private::TypeEnumMemberImpl &
    ref () const;

    lldb::TypeEnumMemberImplSP m_opaque_sp;

    SBTypeEnumMember (const lldb::TypeEnumMemberImplSP &);
};

%feature(
    "docstring",
    "Represents a list of SBTypeEnumMembers."
) SBTypeEnumMemberList;

class SBTypeEnumMemberList
{
public:
    SBTypeEnumMemberList();

    SBTypeEnumMemberList(const SBTypeEnumMemberList& rhs);

    ~SBTypeEnumMemberList();

    bool
    IsValid();

    void
    Append (SBTypeEnumMember entry);

    SBTypeEnumMember
    GetTypeEnumMemberAtIndex (uint32_t index);

    uint32_t
    GetSize();


private:
    std::unique_ptr<lldb_private::TypeEnumMemberListImpl> m_opaque_ap;
};

} // namespace lldb
