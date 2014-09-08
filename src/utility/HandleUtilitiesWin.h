#pragma once

/***************
 * Auto-release handle utilities for Windows
 ***************/

template<class T>
struct HandleDeleter;

template<>
struct HandleDeleter<HDC> { void operator()(HDC hdc) const { if(hdc) ::DeleteDC(hdc); } };
template<>
struct HandleDeleter<HANDLE> { void operator()(HANDLE hnd) const { if(hnd) ::CloseHandle(hnd); } };
template<>
struct HandleDeleter<HBITMAP> { void operator()(HBITMAP hbmp) const { if(hbmp) ::DeleteObject(hbmp); } };

template<class T>
struct unique_ptr_of;

template<class T>
struct unique_ptr_of<T*> :
  std::unique_ptr<T, HandleDeleter<T*>>
{
  unique_ptr_of(T* ptr = nullptr) :
    std::unique_ptr<T, HandleDeleter<T*>>(ptr)
  {}

  struct proxy {
    proxy(unique_ptr_of& parent):
      parent(parent),
      ptr(nullptr)
    {}

    ~proxy(void) {
      // Forward unconditionally:
      parent.reset(ptr);
    }

    T* ptr;
    unique_ptr_of& parent;

    operator T**(void) { return &ptr; }
  };

  proxy operator&(void) { return proxy(*this); }
};