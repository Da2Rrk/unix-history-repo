//===-- Baton.h -------------------------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef lldb_Baton_h_
#define lldb_Baton_h_

#include "lldb/lldb-enumerations.h"
#include "lldb/lldb-public.h"

#include <memory>

namespace lldb_private {
class Stream;
}

namespace lldb_private {

/// \class Baton Baton.h "lldb/Core/Baton.h"
/// A class designed to wrap callback batons so they can cleanup
///        any acquired resources
///
/// This class is designed to be used by any objects that have a callback
/// function that takes a baton where the baton might need to
/// free/delete/close itself.
///
/// The default behavior is to not free anything. Subclasses can free any
/// needed resources in their destructors.
class Baton {
public:
  Baton() {}
  virtual ~Baton() {}

  virtual void *data() = 0;

  virtual void GetDescription(Stream *s,
                              lldb::DescriptionLevel level) const = 0;
};

class UntypedBaton : public Baton {
public:
  UntypedBaton(void *Data) : m_data(Data) {}
  ~UntypedBaton() override {
    // The default destructor for an untyped baton does NOT attempt to clean up
    // anything in m_data.
  }

  void *data() override { return m_data; }
  void GetDescription(Stream *s, lldb::DescriptionLevel level) const override;

  void *m_data; // Leave baton public for easy access
};

template <typename T> class TypedBaton : public Baton {
public:
  explicit TypedBaton(std::unique_ptr<T> Item) : Item(std::move(Item)) {}

  T *getItem() { return Item.get(); }
  const T *getItem() const { return Item.get(); }

  void *data() override { return Item.get(); }
  void GetDescription(Stream *s, lldb::DescriptionLevel level) const override {}

protected:
  std::unique_ptr<T> Item;
};

} // namespace lldb_private

#endif // lldb_Baton_h_
