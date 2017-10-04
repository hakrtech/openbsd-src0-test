//===-- SWIG Interface for SBFunction ---------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

namespace lldb {

%feature("docstring",
"Represents a generic function, which can be inlined or not.

For example (from test/lldbutil.py, but slightly modified for doc purpose),

        ...

        frame = thread.GetFrameAtIndex(i)
        addr = frame.GetPCAddress()
        load_addr = addr.GetLoadAddress(target)
        function = frame.GetFunction()
        mod_name = frame.GetModule().GetFileSpec().GetFilename()

        if not function:
            # No debug info for 'function'.
            symbol = frame.GetSymbol()
            file_addr = addr.GetFileAddress()
            start_addr = symbol.GetStartAddress().GetFileAddress()
            symbol_name = symbol.GetName()
            symbol_offset = file_addr - start_addr
            print >> output, '  frame #{num}: {addr:#016x} {mod}`{symbol} + {offset}'.format(
                num=i, addr=load_addr, mod=mod_name, symbol=symbol_name, offset=symbol_offset)
        else:
            # Debug info is available for 'function'.
            func_name = frame.GetFunctionName()
            file_name = frame.GetLineEntry().GetFileSpec().GetFilename()
            line_num = frame.GetLineEntry().GetLine()
            print >> output, '  frame #{num}: {addr:#016x} {mod}`{func} at {file}:{line} {args}'.format(
                num=i, addr=load_addr, mod=mod_name,
                func='%s [inlined]' % func_name] if frame.IsInlined() else func_name,
                file=file_name, line=line_num, args=get_args_as_string(frame, showFuncName=False))

        ...
") SBFunction;
class SBFunction
{
public:

    SBFunction ();

    SBFunction (const lldb::SBFunction &rhs);

    ~SBFunction ();

    bool
    IsValid () const;

    const char *
    GetName() const;
    
    const char *
    GetDisplayName() const;

    const char *
    GetMangledName () const;

    lldb::SBInstructionList
    GetInstructions (lldb::SBTarget target);

    lldb::SBInstructionList
    GetInstructions (lldb::SBTarget target, const char *flavor);

    lldb::SBAddress
    GetStartAddress ();

    lldb::SBAddress
    GetEndAddress ();

    const char *
    GetArgumentName (uint32_t arg_idx);

    uint32_t
    GetPrologueByteSize ();

    lldb::SBType
    GetType ();

    lldb::SBBlock
    GetBlock ();
    
    lldb::LanguageType
    GetLanguage ();

    %feature("docstring", "
    Returns true if the function was compiled with optimization.
    Optimization, in this case, is meant to indicate that the debugger
    experience may be confusing for the user -- variables optimized away,
    stepping jumping between source lines -- and the driver may want to 
    provide some guidance to the user about this.
    Returns false if unoptimized, or unknown.") GetIsOptimized;
    bool
    GetIsOptimized();

    bool
    GetDescription (lldb::SBStream &description);
    
    bool
    operator == (const lldb::SBFunction &rhs) const;
    
    bool
    operator != (const lldb::SBFunction &rhs) const;
    
    %pythoncode %{
        def get_instructions_from_current_target (self):
            return self.GetInstructions (target)

        __swig_getmethods__["addr"] = GetStartAddress
        if _newclass: addr = property(GetStartAddress, None, doc='''A read only property that returns an lldb object that represents the start address (lldb.SBAddress) for this function.''')

        __swig_getmethods__["end_addr"] = GetEndAddress
        if _newclass: end_addr = property(GetEndAddress, None, doc='''A read only property that returns an lldb object that represents the end address (lldb.SBAddress) for this function.''')
                
        __swig_getmethods__["block"] = GetBlock
        if _newclass: block = property(GetBlock, None, doc='''A read only property that returns an lldb object that represents the top level lexical block (lldb.SBBlock) for this function.''')

        __swig_getmethods__["instructions"] = get_instructions_from_current_target
        if _newclass: instructions = property(get_instructions_from_current_target, None, doc='''A read only property that returns an lldb object that represents the instructions (lldb.SBInstructionList) for this function.''')

        __swig_getmethods__["mangled"] = GetMangledName
        if _newclass: mangled = property(GetMangledName, None, doc='''A read only property that returns the mangled (linkage) name for this function as a string.''')

        __swig_getmethods__["name"] = GetName
        if _newclass: name = property(GetName, None, doc='''A read only property that returns the name for this function as a string.''')

        __swig_getmethods__["prologue_size"] = GetPrologueByteSize
        if _newclass: prologue_size = property(GetPrologueByteSize, None, doc='''A read only property that returns the size in bytes of the prologue instructions as an unsigned integer.''')

        __swig_getmethods__["type"] = GetType
        if _newclass: type = property(GetType, None, doc='''A read only property that returns an lldb object that represents the return type (lldb.SBType) for this function.''')
    %}

};

} // namespace lldb
