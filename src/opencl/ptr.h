#ifndef _PTR_
#define _PTR_

#include <cassert>
#include <memory>
#include <iostream>

template <typename T>
using ptr = std::shared_ptr<T>;

template <typename T>
using cptr = ptr<const T>;

template <typename T>
class Pointable {
  const bool verbose = false;
  std::weak_ptr<T> self;
 protected:
  Pointable() {
    if (verbose) std::cerr << "Constructing " << typeid(*this).name() << "(this): " << this << std::endl;
  }
 public:
  virtual ~Pointable() {
    if (verbose) std::cerr << "Destroying " << typeid(*this).name() << ": " << this << std::endl;
    if (ready()) std::cerr << "Deconstructing an unreleased self!" << std::endl;
  }
  ptr<T> ref() const {
    if (verbose) std::cerr << "Locking " << typeid(*this).name() << ": " << this << std::endl;
    assert(ready());
    return self.lock();
  }
  ptr<T> bake() {
    if (verbose) std::cerr << "Baking " << typeid(*this).name() << ": " << this << std::endl;
    ptr<T> baked((T*) this);
    self = std::weak_ptr<T>(baked);
    return baked;
  }
  bool ready() const {
    return ! self.expired();
  }
};

#endif // _PTR_
