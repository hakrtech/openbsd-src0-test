//===- llvm/Support/Options.h - Debug options support -----------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
/// \file
/// This file declares helper objects for defining debug options that can be
/// configured via the command line. The new API currently builds on the cl::opt
/// API, but does not require the use of static globals.
///
/// With this API options are registered during initialization. For passes, this
/// happens during pass initialization. Passes with options will call a static
/// registerOptions method during initialization that registers options with the
/// OptionRegistry. An example implementation of registerOptions is:
///
/// static void registerOptions() {
///   OptionRegistry::registerOption<bool, Scalarizer,
///                                &Scalarizer::ScalarizeLoadStore>(
///       "scalarize-load-store",
///       "Allow the scalarizer pass to scalarize loads and store", false);
/// }
///
/// When reading data for options the interface is via the LLVMContext. Option
/// data for passes should be read from the context during doInitialization. An
/// example of reading the above option would be:
///
/// ScalarizeLoadStore =
///   M.getContext().getOption<bool,
///                            Scalarizer,
///                            &Scalarizer::ScalarizeLoadStore>();
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_SUPPORT_OPTIONS_H
#define LLVM_SUPPORT_OPTIONS_H

#include "llvm/ADT/DenseMap.h"
#include "llvm/Support/CommandLine.h"

namespace llvm {

namespace detail {

// Options are keyed of the unique address of a static character synthesized
// based on template arguments.
template <typename ValT, typename Base, ValT(Base::*Mem)> class OptionKey {
public:
  static char ID;
};

template <typename ValT, typename Base, ValT(Base::*Mem)>
char OptionKey<ValT, Base, Mem>::ID = 0;

} // namespace detail

/// \brief Singleton class used to register debug options.
///
/// The OptionRegistry is responsible for managing lifetimes of the options and
/// provides interfaces for option registration and reading values from options.
/// This object is a singleton, only one instance should ever exist so that all
/// options are registered in the same place.
class OptionRegistry {
private:
  DenseMap<void *, cl::Option *> Options;

  /// \brief Adds a cl::Option to the registry.
  ///
  /// \param Key unique key for option
  /// \param O option to map to \p Key
  ///
  /// Allocated cl::Options are owned by the OptionRegistry and are deallocated
  /// on destruction or removal
  void addOption(void *Key, cl::Option *O);

public:
  ~OptionRegistry();
  OptionRegistry() {}

  /// \brief Returns a reference to the singleton instance.
  static OptionRegistry &instance();

  /// \brief Registers an option with the OptionRegistry singleton.
  ///
  /// \tparam ValT type of the option's data
  /// \tparam Base class used to key the option
  /// \tparam Mem member of \p Base used for keying the option
  ///
  /// Options are keyed off the template parameters to generate unique static
  /// characters. The template parameters are (1) the type of the data the
  /// option stores (\p ValT), the class that will read the option (\p Base),
  /// and the member that the class will store the data into (\p Mem).
  template <typename ValT, typename Base, ValT(Base::*Mem)>
  static void registerOption(const char *ArgStr, const char *Desc,
                             const ValT &InitValue) {
    cl::opt<ValT> *Option = new cl::opt<ValT>(ArgStr, cl::desc(Desc),
                                              cl::Hidden, cl::init(InitValue));
    instance().addOption(&detail::OptionKey<ValT, Base, Mem>::ID, Option);
  }

  /// \brief Returns the value of the option.
  ///
  /// \tparam ValT type of the option's data
  /// \tparam Base class used to key the option
  /// \tparam Mem member of \p Base used for keying the option
  ///
  /// Reads option values based on the key generated by the template parameters.
  /// Keying for get() is the same as keying for registerOption.
  template <typename ValT, typename Base, ValT(Base::*Mem)> ValT get() const {
    auto It = Options.find(&detail::OptionKey<ValT, Base, Mem>::ID);
    assert(It != Options.end() && "Option not in OptionRegistry");
    return *(cl::opt<ValT> *)It->second;
  }
};

} // namespace llvm

#endif
