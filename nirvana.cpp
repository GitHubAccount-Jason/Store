#include <iostream>
#include <list>
#include <string_view>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <hashtable.h>
#include <unordered_map>
#include <functional>
#ifdef _WIN32
#include <windows.h>
#endif
#define __NIRVANA_VERSION__ (1)
#define __NIRVANA_MAGIC__ (1)
#define __LINE_STR__ (std::to_string(__LINE__) + " ")
#define __NIRVANA_DEBUG__
using ull = unsigned long long;
// Nirvana RunTime
// 据我所知，MinGW和TDM-GCC对std::wstring的支持不咋样
namespace NRT {
// 手搓wstring
// 为了避免麻烦，怎么方便怎么来
// 效率慢的可以，哪天我高兴优化一下
template <class _T>
static inline wchar_t* from(_T _v, unsigned int base = 10) { /*{{{*/
  wchar_t* ret;
  if (_v == 0) {
    ret = (wchar_t*)malloc(sizeof(wchar_t) * 2);
    ret[0] = L'0';
    ret[1] = L'\0';
    return ret;
  }
  size_t size = 0;
  if (_v > 0) {
    ret = (wchar_t*)malloc(sizeof(wchar_t) * 1);
    ret[0] = L'\0';
    while (_v != 0) {
      wchar_t* pnew = (wchar_t*)malloc(sizeof(wchar_t) * (size + 2));
      wmemcpy(pnew + 1, ret, (size + 1));
      pnew[0] = _v % 10 + L'0';
      free(ret);
      ret = pnew;
      _v /= 10;
      ++size;
    }
  } else {
    ret = (wchar_t*)malloc(sizeof(wchar_t) * 2);
    ret[0] = L'-';
    ret[1] = L'\0';
    _v = -_v;
    while (_v != 0) {
      wchar_t* pnew = (wchar_t*)malloc(sizeof(wchar_t) * (size + 3));
      wmemcpy(pnew + 2, ret + 1, (size + 1));
      pnew[0] = L'-';
      pnew[1] = _v % 10 + L'0';
      _v /= 10;
      free(ret);
      ret = pnew;
      ++size;
    }
  }
  return ret;

} /*}}}*/
template <class _T = int>
static inline _T to(const wchar_t* _c, unsigned int base = 10) { /*{{{*/
  _T ret = 0;
  if (_c[0] == '-') {
    size_t sz = wcslen(_c);
    for (int i = 1; i != sz; ++i) {
      ret *= 10;
      ret += (_c[i] - L'0');
    }
    return -ret;
  } else if (_c[0] == '+') {
    size_t sz = wcslen(_c);
    for (int i = 1; i != sz; ++i) {
      ret *= 10;
      ret += (_c[i] - L'0');
    }
    return ret;
  } else {
    size_t sz = wcslen(_c);
    for (int i = 0; i != sz; ++i) {
      ret *= 10;
      ret += (_c[i] - L'0');
    }
    return ret;
  }
} /*}}}*/
class WString { /*{{{*/
  private:
  wchar_t* base_;
  // 不包括结束符
  ull len_;

  public:
  struct Iterator { /*{{{*/
private:
    wchar_t* ptr;

public:
    wchar_t*& pointer() { return ptr; }
    wchar_t* pointer() const { return ptr; }
    Iterator() : ptr(0) {}
    Iterator(const Iterator& i) : ptr(i.ptr) {}
    Iterator(const Iterator&& i) : ptr(i.ptr) {}
    Iterator(wchar_t* p) : ptr(p) {}
    ~Iterator() {}
    wchar_t& operator*() { return *ptr; }
    wchar_t* operator->() { return ptr; }
    Iterator& operator++(int) { /*{{{*/
      ++ptr;
      return *this;
    }                       /*}}}*/
    Iterator operator++() { /*{{{*/
      Iterator ret(*this);
      ++ptr;
      return ret;
    }                           /*}}}*/
    bool operator==(Iterator i) const { return ptr == i.ptr; }
    Iterator& operator--(int) { /*{{{*/
      --ptr;
      return *this;
    }                       /*}}}*/
    Iterator operator--() { /*{{{*/
      Iterator ret(*this);
      --ptr;
      return ret;
    } /*}}}*/
    Iterator operator+(long long i) const { return Iterator(ptr + i); }
    void operator+=(long long i) { ptr += i; }
    Iterator operator-(long long i) const { return Iterator(ptr - i); }
    void operator-=(long long i) { ptr -= i; }
    bool operator!=(Iterator i) const { return ptr != i.ptr; }
    bool operator<=(Iterator i) const { return ptr <= i.ptr; }
    bool operator>=(Iterator i) const { return ptr >= i.ptr; }
    bool operator<(Iterator i) const { return ptr < i.ptr; }
    bool operator>(Iterator i) const { return ptr > i.ptr; }
    Iterator& operator=(Iterator i) {
      ptr = i.ptr;
      return *this;
    }
  };                     /*}}}*/
  struct ConstIterator { /*{{{*/
private:
    const wchar_t* ptr;

public:
    const wchar_t*& pointer() { return ptr; }
    const wchar_t* pointer() const { return ptr; }
    ConstIterator() : ptr(0) {}
    ConstIterator(const ConstIterator& i) : ptr(i.ptr) {}
    ConstIterator(const ConstIterator&& i) : ptr(i.ptr) {}
    ConstIterator(const wchar_t* p) : ptr(p) {}
    ~ConstIterator() {}
    const wchar_t& operator*() { return *ptr; }
    const wchar_t* operator->() { return ptr; }
    ConstIterator& operator++(int) { /*{{{*/
      ++ptr;
      return *this;
    }                            /*}}}*/
    ConstIterator operator++() { /*{{{*/
      ConstIterator ret(*this);
      ++ptr;
      return ret;
    }                                /*}}}*/
    ConstIterator& operator--(int) { /*{{{*/
      --ptr;
      return *this;
    }                            /*}}}*/
    ConstIterator operator--() { /*{{{*/
      ConstIterator ret(*this);
      --ptr;
      return ret;
    }
    bool operator==(ConstIterator i) const { return ptr == i.ptr; } /*}}}*/
    ConstIterator operator+(long long i) const { return ConstIterator(ptr + i); }
    void operator+=(long long i) { ptr += i; }
    ConstIterator operator-(long long i) const { return ConstIterator(ptr - i); }
    void operator-=(long long i) { ptr -= i; }
    bool operator!=(ConstIterator i) const { return ptr != i.ptr; }
    bool operator<=(ConstIterator i) const { return ptr <= i.ptr; }
    bool operator>=(ConstIterator i) const { return ptr >= i.ptr; }
    bool operator<(ConstIterator i) const { return ptr < i.ptr; }
    bool operator>(ConstIterator i) const { return ptr > i.ptr; }
    ConstIterator& operator=(ConstIterator i) {
      ptr = i.ptr;
      return *this;
    }
  }; /*}}}*/
  wchar_t* c_str() { return base_; }
  const wchar_t* c_str() const { return base_; }
  ull length() const { return len_; }
  WString() : base_((wchar_t*)malloc(sizeof(wchar_t) * 1)), len_(0) { base_[0] = L'\0'; }
  WString(const wchar_t* a) { /*{{{*/
    len_ = wcslen(a);
    base_ = (wchar_t*)malloc(sizeof(wchar_t) * (len_ + 1));
    wcscpy(base_, a);
  }                         /*}}}*/
  WString(const WString& a) /*{{{*/
      : base_((wchar_t*)malloc(sizeof(wchar_t) * (a.length() + 1))), len_(a.length()) {
    wcscpy(base_, a.c_str());
  }                          /*}}}*/
  WString(const WString&& a) /*{{{*/
      : base_((wchar_t*)malloc(sizeof(wchar_t) * (a.length() + 1))), len_(a.length()) {
    wcscpy(base_, a.c_str());
  }                                      /*}}}*/
  WString& operator=(const WString& a) { /*{{{*/
    free(base_);
    base_ = (wchar_t*)malloc(sizeof(wchar_t) * (a.length() + 1));
    len_ = a.length();
    wcscpy(base_, a.c_str());
    return *this;
  }                                  /*}}}*/
  WString substr(ull pos, ull len) { /*{{{*/
    wchar_t* p = (wchar_t*)malloc(sizeof(wchar_t) * (len + 1));
    wmemcpy(p, base_ + pos, len);
    p[len] = L'\0';
    return WString::stealFrom(p);
  }                            /*}}}*/
  ull find(const wchar_t* w) { /*{{{*/
    ull wlen = wcslen(w);
    if (len_ < wlen) {
      return -1;
    }
    for (ull i = 0; i != len_ - wlen + 1; ++i) {
      bool __will_break = false;
      for (ull j = 0; j != wlen; ++j) {
        if (base_[i + j] != w[j]) {
          __will_break = true;
          break;
        }
      }
      if (!__will_break) {
        return i;
      }
    }
    return (ull)-1;
  }                                     /*}}}*/
  ConstIterator find(wchar_t w) const { /*{{{*/
    for (auto i = begin(); i != end(); ++i) {
      if (*i == w) {
        return i;
      }
    }
    return end();
  }                                                   /*}}}*/
  void replace(ull pos, ull len, const WString& ws) { /*{{{*/
    wchar_t* __new = (wchar_t*)malloc(sizeof(wchar_t) * (len_ - len + ws.length() + 1));
    wmemcpy(__new, base_, (pos));
    wmemcpy(__new + pos, ws.c_str(), ws.length());
    wmemcpy(__new + pos + ws.length(), base_ + pos + len, (len_ - pos - len));
    __new[len_ - len + ws.length()] = L'\0';
    free(base_);
    base_ = __new;
    len_ = len_ - len + ws.length();
  } /*}}}*/

  Iterator begin() { return Iterator(base_); }
  Iterator end() { return Iterator(base_ + len_); }
  ConstIterator begin() const { return ConstIterator(base_); }
  ConstIterator end() const { return ConstIterator(base_ + len_); }
  void pushBack(wchar_t a) { /*{{{*/
    ++len_;
    base_ = (wchar_t*)realloc(base_, sizeof(wchar_t) * (len_ + 1));
    base_[len_ - 1] = a;
    base_[len_] = L'\0';
  }                /*}}}*/
  void popBack() { /*{{{*/
    --len_;
    base_ = (wchar_t*)realloc(base_, sizeof(wchar_t) * (len_ + 1));
    base_[len_] = L'\0';
  }                        /*}}}*/
  void append(wchar_t a) { /*{{{*/
    ++len_;
    base_ = (wchar_t*)realloc(base_, sizeof(wchar_t) * (len_ + 1));
    base_[len_ - 1] = a;
    base_[len_] = L'\0';
  }                               /*}}}*/
  void append(const WString& w) { /*{{{*/
    base_ = (wchar_t*)realloc(base_, sizeof(wchar_t) * (len_ + w.length() + 1));
    wmemcpy(base_ + len_, w.c_str(), (w.length()));
    len_ += w.length();
    base_[len_] = L'\0';
  }                               /*}}}*/
  void append(const wchar_t* w) { /*{{{*/
    ull wlen = wcslen(w);
    base_ = (wchar_t*)realloc(base_, sizeof(wchar_t) * (len_ + wlen + 1));
    wmemcpy(base_ + len_, w, (wlen));
    len_ += wlen;
    base_[len_] = L'\0';
  } /*}}}*/
  wchar_t& operator[](ull p) { return base_[p]; }
  const wchar_t& operator[](ull p) const { return base_[p]; }
  bool operator==(const WString& w) const { /*{{{*/
    if (len_ != w.length()) {
      return false;
    }
    for (ull i = 0; i != len_; ++i) {
      if (base_[i] != w[i]) {
        return false;
      }
    }
    return true;
  }                                         /*}}}*/
  bool operator==(const wchar_t* w) const { /*{{{*/
    if (len_ != wcslen(w)) {
      return false;
    }
    for (ull i = 0; i != len_; ++i) {
      if (base_[i] != w[i]) {
        return false;
      }
    }
    return true;
  }                                           /*}}}*/
  WString operator+(const WString& w) const { /*{{{*/
    ull slen = len_, wlen = w.len_;
    wchar_t* buf = (wchar_t*)malloc(sizeof(wchar_t) * (slen + wlen + 1));
    wmemcpy(buf, base_, (slen));
    wmemcpy(buf + slen, w.c_str(), (wlen));
    buf[slen + wlen] = L'\0';
    return WString::stealFrom(buf);
  }                                           /*}}}*/
  WString operator+(const wchar_t* w) const { /*{{{*/
    ull slen = len_, wlen = wcslen(w);
    wchar_t* buf = (wchar_t*)malloc(sizeof(wchar_t) * (slen + wlen + 1));
    wmemcpy(buf, base_, (slen));
    wmemcpy(buf + slen, w, (wlen));
    buf[slen + wlen] = L'\0';
    return WString::stealFrom(buf);
  }                                   /*}}}*/
  void operator+=(const WString& w) { /*{{{*/
    base_ = (wchar_t*)realloc(base_, sizeof(wchar_t) * (len_ + w.length() + 1));
    wmemcpy(base_ + len_, w.c_str(), (w.length()));
    len_ += w.length();
    base_[len_] = L'\0';
  }                                   /*}}}*/
  void operator+=(const wchar_t* w) { /*{{{*/
    ull wlen = wcslen(w);
    base_ = (wchar_t*)realloc(base_, sizeof(wchar_t) * sizeof(wchar_t) * (len_ + wlen + 1));
    wmemcpy(base_ + len_, w, (wlen));
    len_ += wlen;
    base_[len_] = L'\0';
  }                                      /*}}}*/
  static WString stealFrom(wchar_t* w) { /*{{{*/
    WString ret;
    ret.len_ = wcslen(w);
    ret.base_ = w;
    return ret;
  }                                    /*}}}*/
  static WString fromStdInReadLine() { /*{{{*/
    WString ret;
    wchar_t buffer;
    DWORD wasted;
    while (ReadConsoleW(GetStdHandle(STD_INPUT_HANDLE), &buffer, 1, &wasted, NULL), wasted == 1) {
      if (buffer == '\r') {
        // eat 换行符
        ReadConsoleW(GetStdHandle(STD_INPUT_HANDLE), &buffer, 1, &wasted, NULL);
        break;
      }
      ret.pushBack(buffer);
    }
    return ret;
  }                       /*}}}*/
  void toStdOut() const { /*{{{*/
    DWORD wasted;
    WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), base_, len_, &wasted, NULL);
  }                                              /*}}}*/
  ~WString() { free(base_); }
};                                               /*}}}*/
template <class T>
WString toWString(T i, unsigned int base = 10) { /*{{{*/
  return WString::stealFrom(from<T>(i, base));
} /*}}}*/
template <class T>
T WStringTo(const WString& ws, unsigned int base = 10) { /*{{{*/
  return to(ws.c_str(), base);
} /*}}}*/
WString operator+(const wchar_t* w, const WString& s) { /*{{{*/
  ull wlen = wcslen(w), slen = s.length();
  wchar_t* buf = (wchar_t*)malloc(sizeof(wchar_t) * (slen + wlen + 1));
  wmemcpy(buf, w, (wlen));
  wmemcpy(buf + wlen, s.c_str(), (slen));
  buf[wlen + slen] = L'\0';
  return WString::stealFrom(buf);
} /*}}}*/
}  // namespace NRT

using stringType = NRT::WString;
std::array<const char, 30> __identifies = {'(',  ')', '[', ']', '{', '}', '`', '~', '!', '@',
                                           '#',  '$', '%', '^', '&', '*', '-', '_', '+', '=',
                                           '\'', '|', ';', ':', ',', '<', '.', '>', '/', '?'};
std::array<const char, 2> __string_pair = {'"', '\''};
std::array<std::pair<const wchar_t*, const wchar_t*>, 4> __escape_char = {
    std::make_pair(L"\\t", L"\t"), std::make_pair(L"\\b", L"\b"), std::make_pair(L"\\e", L"\e"),
    std::make_pair(L"\\n", L"\n")};
std::array<const char, 3> __wasted = {' ', '\t', '\n'};
std::array<stringType, 2> __keywords = {L"import", L"if"};
std::array<std::array<stringType, 2>, 6> __token_type_map_table{
    std::array<stringType, 2>{L"(", L"pair-token"}, std::array<stringType, 2>{L")", L"pair-token"},
    std::array<stringType, 2>{L"[", L"pair-token"}, std::array<stringType, 2>{L"]", L"pair-token"},
    std::array<stringType, 2>{L"{", L"pair-token"}, std::array<stringType, 2>{L"}", L"pair-token"}};
stringType __token_type_map(stringType name) {
  for (auto& i : __token_type_map_table) {
    if (i[0] == name) {
      return i[1];
    }
  }
  return L"identifier";
}
class System { /*{{{*/
  public:
  class Console {
public:
    enum TextAttrbuteFg {
      FgBlack = 0,
      FgBlue = FOREGROUND_BLUE,
      FgGreen = FOREGROUND_GREEN,
      FgRed = FOREGROUND_RED,
      FgPurple = FgRed | FgBlue,
      FgCyan = FgBlue | FgGreen,
      FgYellow = FgRed | FgGreen,
      FgWhite = FgBlue | FgGreen | FgRed,
    };
    enum TextAttributeBg {
      BgBlack = 0,
      BgBlue = BACKGROUND_BLUE,
      BgGreen = BACKGROUND_GREEN,
      BgRed = BACKGROUND_RED,
      BgPurple = BgRed | BgBlue,
      BgCyan = BgBlue | BgGreen,
      BgYellow = BgRed | BgGreen,
      BgWhite = BgBlue | BgGreen | BgRed,
    };
    static void setOutputTextAttribute(TextAttributeBg b, TextAttrbuteFg f) {
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), int(b) | int(f));
    }
  };
};          /*}}}*/
struct GC { /*{{{*/
  template <class _T>
  struct deleter {
    deleter() = delete;
    deleter(const deleter&) = delete;
    static void delete_obj(_T*& p) {
      delete p;
      p = nullptr;
    }
  };
  struct GCObject {
    void* p_;
    void (*deleter_)(void*&);
    GCObject(void* p, void (*deleter)(void*&)) {
      p_ = p;
      deleter_ = deleter;
    }
    GCObject(const GCObject&& o) {
      p_ = o.p_;
      deleter_ = o.deleter_;
    }
  };
  std::list<GCObject> l_;
  size_t sz_ = 0;
  template <class _T, class... Args>
  _T* create(Args&&... args) {
    _T* p = new _T(args...);
    using f = void(void*&);
    l_.emplace_back(GCObject{(void*)p, (f*)deleter<_T>::delete_obj});
    sz_ += sizeof(_T);
    return p;
  }
  void destroy(void* p) {
    for (auto i = l_.begin(); i != l_.end(); ++i) {
      if (i->p_ == p) {
        i->deleter_(i->p_);
        i = l_.erase(i);
      }
    }
  }
  void destroyAll() {
    for (auto& o : l_) {
      o.deleter_(o.p_);
    }
  }
  ~GC() {
    std::cout << '\n';
    System::Console::setOutputTextAttribute(System::Console::BgGreen, System::Console::FgWhite);
    std::cout << "GC ";
    System::Console::setOutputTextAttribute(System::Console::BgGreen, System::Console::FgBlue);
    std::cout << l_.size();
    System::Console::setOutputTextAttribute(System::Console::BgGreen, System::Console::FgWhite);
    std::cout << " objects, ";
    System::Console::setOutputTextAttribute(System::Console::BgGreen, System::Console::FgBlue);
    std::cout << sz_;
    System::Console::setOutputTextAttribute(System::Console::BgGreen, System::Console::FgWhite);
    std::cout << " bytes";
    System::Console::setOutputTextAttribute(System::Console::BgBlack, System::Console::FgWhite);
    std::cout << '\n';
    destroyAll();
  }
};
GC gc;
/*}}}*/
class Exception { /*{{{*/
  stringType data;

  public:
  Exception(const stringType& s) : data(s) {}
  static Exception outOfRange(const stringType& detail = L"") {
    return Exception(detail == L"" ? L"Out of range" : L"Out of range: " + detail);
  }
  static Exception invalidSyntax(const stringType& detail = L"") {
    return Exception(detail == L"" ? L"Invalid syntax" : L"Invalid syntax: " + detail);
  }
  static Exception runtimeError(const stringType& detail = L"") {
    return Exception(detail == L"" ? L"Runtime error" : L"Runtime error: " + detail);
  }
  static Exception compilationError(const stringType& detail = L"") {
    return Exception(detail == L"" ? L"Compilation error" : L"Compilation error: " + detail);
  }
  void raise() {
    System::Console::setOutputTextAttribute(System::Console::BgRed, System::Console::FgWhite);
    stringType(L"Exception raised").toStdOut();
    System::Console::setOutputTextAttribute(System::Console::BgBlack, System::Console::FgWhite);
    stringType(L"\n").toStdOut();
    System::Console::setOutputTextAttribute(System::Console::BgRed, System::Console::FgWhite);
    data.toStdOut();
    System::Console::setOutputTextAttribute(System::Console::BgBlack, System::Console::FgWhite);
    stringType(L"\n").toStdOut();
    System::Console::setOutputTextAttribute(System::Console::BgBlack, System::Console::FgWhite);
    gc.destroyAll();
    exit(3);
  }
}; /*}}}*/
/*{{{*/
#define __RaiseError__(type, content)                                                \
  do {                                                                               \
    Exception::type(L"In line " + NRT::toWString(__LINE__) + ":" + content).raise(); \
  } while (0)
#define __RaiseCompilationError__(content)                                                        \
  do {                                                                                            \
    Exception::compilationError(L"In line " + NRT::toWString(__LINE__) + L":" + content).raise(); \
  } while (0)
#define __RaiseInvalidSyntax__(content)                                                        \
  do {                                                                                         \
    Exception::invalidSyntax(L"In line " + NRT::toWString(__LINE__) + L":" + content).raise(); \
  } while (0)
#define __RaiseRuntimeError__(content)                                                        \
  do {                                                                                        \
    Exception::runtimeError(L"In line " + NRT::toWString(__LINE__) + L":" + content).raise(); \
  } while (0)
#define __RaiseUnknownError(content)                                           \
  do {                                                                         \
    Exception(L"In line " + NRT::toWString(__LINE__) + ":" + content).raise(); \
  } while (0)
/*}}}*/
struct typeinfo { /*{{{*/
  private:
  stringType name_;

  public:
  typeinfo() {}
  typeinfo(const stringType& s) : name_(s) {}
  typeinfo(const typeinfo& a) : name_(a.name()) {}
  typeinfo(const typeinfo&& a) : name_(a.name()) {}
  typeinfo& operator=(const typeinfo& a) {
    name_ = a.name();
    return *this;
  }
  bool operator==(const typeinfo& a) const { return name_ == a.name(); }
  bool operator!=(const typeinfo& a) const { return name_ != a.name(); }
  ~typeinfo() {}
  stringType& name() { return name_; }
  const stringType& name() const { return name_; }
};           /*}}}*/
struct Var { /*{{{*/
  private:
  stringType name_;
  typeinfo type_;
  void* p_;

  public:
  Var() : type_(L"void"), p_(nullptr) {}
  Var(const stringType& name) : type_(L"void"), p_(nullptr), name_(name) {}
  Var(const stringType& name, const typeinfo& t) : type_(t), name_(name) {
    if (t.name() == L"str") {
      p_ = gc.create<stringType>();
    } else if (t.name() == L"int") {
      p_ = gc.create<int>();
    } else if (t.name() == L"void") {
      p_ = nullptr;
    } else if (t.name() == L"ref") {
      p_ = nullptr;
    }
  }
  Var(const stringType& name, int p) : name_(name), type_(L"int"), p_(gc.create<int>(p)) {}
  Var(const stringType& name, Var& p) : name_(name), type_(L"ref"), p_(&p) {}
  Var(const stringType& name, const stringType& s)
      : name_(name), type_(L"str"), p_(gc.create<stringType>(s)) {}
  Var(const Var& v) {
    name_ = v.name();
    type_ = v.type();
    if (v.type().name() == L"int") {
      p_ = gc.create<int>(v.toInt());
    } else if (v.type().name() == L"str") {
      p_ = gc.create<stringType>(v.toStr());
    } else if (v.type().name() == L"void") {
      p_ = nullptr;
    } else if (v.type().name() == L"ref") {
      p_ = v.pointer();
    }
  }
  Var(const Var& v, const stringType& name) {
    name_ = name;
    type_ = v.type_;
    if (v.type().name() == L"int") {
      p_ = gc.create<int>(v.toInt());
    } else if (v.type().name() == L"str") {
      p_ = gc.create<stringType>(v.toStr());
    } else if (v.type().name() == L"void") {
      p_ = nullptr;
    } else if (v.type().name() == L"ref") {
      p_ = v.pointer();
    }
  }
  Var(const Var&& v) {
    name_ = v.name();
    type_ = v.type_;
    if (v.type().name() == L"int") {
      p_ = gc.create<int>(v.toInt());
    } else if (v.type().name() == L"str") {
      p_ = gc.create<stringType>(v.toStr());
    } else if (v.type().name() == L"void") {
      p_ = nullptr;
    } else if (v.type().name() == L"ref") {
      p_ = v.pointer();
    }
  }
  Var(const Var&& v, const stringType& name) {
    name_ = name;
    type_ = v.type_;
    if (v.type().name() == L"int") {
      p_ = gc.create<int>(v.toInt());
    } else if (v.type().name() == L"str") {
      p_ = gc.create<stringType>(v.toStr());
    } else if (v.type().name() == L"void") {
      p_ = nullptr;
    } else if (v.type().name() == L"ref") {
      p_ = v.pointer();
    }
  }
  Var& operator=(const Var& v) {
    type_ = v.type_;
    if (v.type().name() == L"int") {
      p_ = gc.create<int>(v.toInt());
    } else if (v.type().name() == L"str") {
      p_ = gc.create<stringType>(v.toStr());
    } else if (v.type().name() == L"void") {
      p_ = nullptr;
    } else if (v.type().name() == L"ref") {
      p_ = v.pointer();
    }
    return *this;
  }
  typeinfo& type() { return type_; }
  const typeinfo& type() const { return type_; }
  stringType& name() { return name_; }
  const stringType& name() const { return name_; }
  void*& pointer() { return p_; }
  void* pointer() const { return p_; }
  Var& unBox() { return *((Var*)p_); }
  const Var& unBox() const { return *((Var*)p_); }
  int& toInt() { return *(int*)p_; }
  const int& toInt() const { return *(int*)p_; }
  stringType& toStr() { return *(stringType*)p_; }
  const stringType& toStr() const { return *(stringType*)p_; }
  template <class _T>
  _T& to() {
    return _T::fromVar(*this);
  }
  template <class _T>
  const _T& to() {
    return _T::fromVar(*this);
  }
  ~Var() {}
}; /*}}}*/
namespace RuntimeTest {
void isVarPointer(Var* v) {
#pragma push(optimize, "")
  (void)*v;
  (void)v->name();
  (void)v->type();
  (void)v->pointer();
#pragma pop
}
}  // namespace RuntimeTest
struct VarManager { /*{{{*/
  private:
  std::list<Var*> vlist;

  public:
  std::list<Var*>& list() { return vlist; }
  const std::list<Var*>& list() const { return vlist; }
  VarManager() {}
  VarManager(const VarManager& v) : vlist(v.list()) {}
  VarManager(const VarManager&& v) : vlist(v.list()) {}
  VarManager& operator=(const VarManager& c) {
    vlist = c.list();
    return *this;
  }
  ~VarManager() {}
  void regist(Var* v) { vlist.emplace_back(v); }
  Var* find(const stringType& vm) {
    for (auto& i : vlist) {
      if (i->name() == vm) {
        return i;
      }
    }
    return nullptr;
  }
  const Var* find(const stringType& vm) const {
    for (auto& i : vlist) {
      if (i->name() == vm) {
        return i;
      }
    }
    return nullptr;
  }
  Var* findRaiseIfNotFound(const stringType& vm) {
    for (auto& i : vlist) {
      if (i->name() == vm) {
        return i;
      }
    }
    __RaiseRuntimeError__(L"Var " + vm + L" not found");
    return nullptr;
  }
  const Var* findRaiseIfNotFound(const stringType& vm) const {
    for (auto& i : vlist) {
      if (i->name() == vm) {
        return i;
      }
    }
    __RaiseRuntimeError__(L"Var " + vm + L" not found");
    return nullptr;
  }
};                /*}}}*/
struct VarModel { /*{{{*/
  private:
  typeinfo type_;

  public:
  VarModel() {}
  VarModel(const typeinfo& type) : type_(type) {}
  VarModel(const VarModel& a) : type_(a.type()) {}
  VarModel(const VarModel&& a) : type_(a.type()) {}
  VarModel(const Var& a) : type_(a.type()) {}
  bool operator==(const VarModel& a) const { return type_ == a.type(); }
  bool operator!=(const VarModel& a) const { return type_ != a.type(); }
  VarModel& operator=(const VarModel& a) {
    type_ = a.type();
    return *this;
  }
  ~VarModel() {}
  typeinfo& type() { return type_; }
  const typeinfo& type() const { return type_; }
};                       /*}}}*/
struct VarModelManager { /*{{{*/
  private:
  std::list<VarModel*> varmodellist;

  public:
  VarModelManager() {}
  VarModelManager(const VarModelManager& a) {
    for (auto& i : a) {
      varmodellist.emplace_back(gc.create<VarModel>(*i));
    }
  }
  VarModelManager(const VarModelManager&& a) {
    for (auto& i : a) {
      varmodellist.emplace_back(gc.create<VarModel>(*i));
    }
  }
  VarModelManager(const std::initializer_list<VarModel*>& l) : varmodellist(l) {}
  VarModelManager(const std::initializer_list<stringType>& l) {
    for (auto& i : l) {
      varmodellist.emplace_back(gc.create<VarModel>(typeinfo(i)));
    }
  }
  ~VarModelManager() {}
  bool operator==(const VarModelManager& v) const {
    if (varmodellist.size() != v.size()) {
      return false;
    }
    auto a = varmodellist.begin(), b = v.begin();
    while (a != varmodellist.end()) {
      if (**a != **b) {
        return false;
      }
      ++a;
      ++b;
    }
    return true;
  }
  bool operator!=(const VarModelManager& v) const {
    if (varmodellist.size() != v.size()) {
      return true;
    }
    auto a = varmodellist.begin(), b = v.begin();
    while (a != varmodellist.end()) {
      if (**a != **b) {
        return true;
      }
      ++a;
      ++b;
    }
    return false;
  }
  VarModelManager& operator=(const VarModelManager& v) {
    for (auto& i : v) {
      varmodellist.emplace_back(gc.create<VarModel>(*i));
    }
    return *this;
  }
  bool operator!=(const VarModelManager& v) {
    if (varmodellist.size() != v.size()) {
      return true;
    }
    auto a = varmodellist.begin();
    auto b = v.begin();
    while (a != varmodellist.end()) {
      if (**a != **b) {
        return true;
      }
      ++a;
      ++b;
    }
    return false;
  }
  std::list<VarModel*>& list() { return varmodellist; }
  const std::list<VarModel*>& list() const { return varmodellist; }
  std::list<VarModel*>::iterator begin() { return varmodellist.begin(); }
  const std::list<VarModel*>::const_iterator begin() const { return varmodellist.begin(); }
  std::list<VarModel*>::iterator end() { return varmodellist.end(); }
  const std::list<VarModel*>::const_iterator end() const { return varmodellist.end(); }
  size_t size() const { return varmodellist.size(); }
  void regist(VarModel* v) { varmodellist.emplace_back(v); }
};             /*}}}*/
struct Stack { /*{{{*/
  private:
  std::list<Var*> l;

  public:
  Stack() {}
  Stack(const Stack& s) {
    for (const auto& o : s.l) {
      l.emplace_back(gc.create<Var>(*o));
    }
  }
  Stack(const Stack&& s) {
    for (const auto& o : s.l) {
      l.emplace_back(gc.create<Var>(*o));
    }
  }
  Stack(const std::initializer_list<Var*>& i) : l(i) {}
  std::list<Var*>& list() { return l; }
  const std::list<Var*>& list() const { return l; }
  void push(Var* v) { l.emplace_back(v); }
  Var* pop() {
    Var* ret = l.back();
    l.pop_back();
    return ret;
  }
  size_t size() const { return l.size(); }
  std::list<Var*>::iterator begin() { return l.begin(); }
  std::list<Var*>::const_iterator begin() const { return l.begin(); }
  std::list<Var*>::iterator end() { return l.end(); }
  std::list<Var*>::const_iterator end() const { return l.end(); }
  Stack& operator=(const Stack& s) {
    for (const auto& o : s.l) {
      l.emplace_back(gc.create<Var>(*o));
    }
    return *this;
  }
  ~Stack() {}
};            /*}}}*/
using funcType = std::function<Var*(Stack*)>;
struct Func { /*{{{*/
  private:
  stringType name_;
  VarModelManager params_;
  funcType pfunc;
  Stack* stack_;

  public:
  stringType& name() { return name_; }
  const stringType& name() const { return name_; }
  VarModelManager& params() { return params_; }
  const VarModelManager& params() const { return params_; }
  funcType& function() { return pfunc; }
  const funcType& function() const { return pfunc; }
  Stack& stack() { return *stack_; }
  const Stack& stack() const { return *stack_; }
  void push(Var* v) { stack_->push(v); }
  Var* pop() { return stack_->pop(); }
  Func() {}
  Func(const stringType& name, const VarModelManager& v, funcType func_)
      : name_(name), params_(v), pfunc(func_), stack_(gc.create<Stack>()) {}
  Func(const Func& f)
      : name_(f.name()), params_(f.params()), pfunc(f.function()), stack_(gc.create<Stack>()) {}
  Func(const Func&& f)
      : name_(f.name()), params_(f.params()), pfunc(f.function()), stack_(gc.create<Stack>()) {}
  ~Func() {}
  Func& operator=(const Func& f) {
    name_ = f.name();
    params_ = f.params();
    pfunc = f.function();
    stack_ = gc.create<Stack>();
    return *this;
  }
  Func& operator=(const Func&& f) {
    name_ = f.name();
    params_ = f.params();
    pfunc = f.function();
    stack_ = gc.create<Stack>();
    return *this;
  }
  Var* call() const {
    if (stack_->list().size() != params_.size()) {
      Exception::runtimeError(L"Function `" + name_ + L"` required a stack of size " +
                              NRT::toWString(params_.size()) + L" which given is " +
                              NRT::toWString(stack_->size()))
          .raise();
    }
    auto a1 = params_.begin();
    auto a2 = stack_->begin();
    size_t p = 0;
    while (a1 != params_.end()) {
      if ((*a1)->type() != (*a2)->type()) {
        __RaiseRuntimeError__(L"Function `" + name_ + L"` required a stack of position " +
                              NRT::toWString(p) + L" is type `" + (*a1)->type().name() +
                              L"` which given is `" + (*a2)->type().name() + L"`");
      }
      ++a1;
      ++p;
      ++a2;
    }
    Var* ret = pfunc(stack_);
    stack_->list().clear();
    return ret;
  }
};                   /*}}}*/
struct FuncManager { /*{{{*/
  private:
  std::list<Func*> funclist;

  public:
  std::list<Func*>& list() { return funclist; }
  const std::list<Func*>& list() const { return funclist; }
  FuncManager() {}
  FuncManager(const FuncManager& f) : funclist(f.list()) {}
  FuncManager(const FuncManager&& f) : funclist(f.list()) {}
  FuncManager& operator=(const FuncManager& f) {
    funclist = f.list();
    return *this;
  }
  FuncManager& operator=(const FuncManager&& f) {
    funclist = f.list();
    return *this;
  }
  ~FuncManager() {}
  void regist(Func* f) { funclist.emplace_back(f); }
  Func* find(const stringType& name) {
    for (auto& f : funclist) {
      if (f->name() == name) {
        return f;
      }
    }
    return nullptr;
  }
  Func* find(const stringType& name, const VarModelManager& v) {
    for (auto& f : funclist) {
      if (f->name() == name && v == f->params()) {
        return f;
      }
    }
    return nullptr;
  }
  const Func* find(const stringType& name) const {
    for (auto& f : funclist) {
      if (f->name() == name) {
        return f;
      }
    }
    return nullptr;
  }
  const Func* find(const stringType& name, const VarModelManager& v) const {
    for (auto& f : funclist) {
      if (f->name() == name) {
        return f;
      }
    }
    return nullptr;
  }
  Func* findRaiseIfNotFount(const stringType& name) {
    for (auto& f : funclist) {
      if (f->name() == name) {
        return f;
      }
    }
    __RaiseRuntimeError__(L"Function " + name + L" not found");
    return nullptr;
  }
  Func* findRaiseIfNotFount(const stringType& name, const VarModelManager& v) {
    for (auto& f : funclist) {
      if (f->name() == name && v == f->params()) {
        return f;
      }
    }
    NRT::WString __raise = L"Function " + name + L"(";
    for (auto& i : v.list()) {
      __raise += i->type().name();
      __raise += L", ";
    }
    __raise += L") not found";
    Exception::runtimeError(__raise).raise();
    return nullptr;
  }
  const Func* findRaiseIfNotFount(const stringType& name) const {
    for (auto& f : funclist) {
      if (f->name() == name) {
        return f;
      }
    }
    Exception::runtimeError(L"Function " + name + L" not found").raise();
    return nullptr;
  }
  const Func* findRaiseIfNotFount(const stringType& name, const VarModelManager& v) const {
    for (auto& f : funclist) {
      if (f->name() == name) {
        return f;
      }
    }
    stringType __raise = L"Function " + name + L"(";
    for (auto& i : v.list()) {
      __raise += i->type().name();
      __raise += L", ";
    }
    __raise += L") not found";
    __RaiseRuntimeError__(__raise);
    return nullptr;
  }
};             /*}}}*/
struct Class { /*{{{*/
  private:
  stringType name_;
  VarModelManager ms_;
  FuncManager funcs_;

  public:
  VarModelManager& members() { return ms_; }
  const VarModelManager& members() const { return ms_; }
  FuncManager& functions() { return funcs_; }
  const FuncManager& functions() const { return funcs_; }
  stringType& name() { return name_; }
  const stringType& name() const { return name_; }
  Class() {}
  Class(const stringType& name) : name_(name) {}
  Class(const stringType& name, const VarModelManager& vm) : name_(name), ms_(vm) {}
  Class(const stringType& name, const VarModelManager& vm, const FuncManager& fm)
      : name_(name), ms_(vm), funcs_(fm) {}

  Class& operator=(const Class& a) {
    name_ = a.name();
    ms_ = a.members();
    funcs_ = a.functions();
    return *this;
  }
  Class& operator=(const Class&& a) {
    name_ = a.name();
    ms_ = a.members();
    funcs_ = a.functions();
    return *this;
  }
  Class(const Class& c) : name_(c.name()), ms_(c.members()), funcs_(c.functions()) {}
  Class(const Class&& c) : name_(c.name()), ms_(c.members()), funcs_(c.functions()) {}
  ~Class() {}
};                    /*}}}*/
struct ClassManager { /*{{{*/
  private:
  std::list<Class*> classes_;

  public:
  std::list<Class*>& list() { return classes_; }
  const std::list<Class*>& list() const { return classes_; }
  ClassManager() {}
  ClassManager(const std::list<Class*>& l) : classes_(l) {}
  ClassManager(const ClassManager& c) : classes_(c.list()) {}
  ClassManager(const ClassManager&& c) : classes_(c.list()) {}
  ~ClassManager() {}
  ClassManager& operator=(const ClassManager& c) {
    classes_ = c.list();
    return *this;
  }
  ClassManager& operator=(const ClassManager&& c) {
    classes_ = c.list();
    return *this;
  }
  void regist(Class* c) { classes_.emplace_back(c); }
  Class* find(const stringType& name) {
    for (auto& i : classes_) {
      if (i->name() == name) {
        return i;
      }
    }
    return nullptr;
  }
  const Class* find(const stringType& name) const {
    for (auto& i : classes_) {
      if (i->name() == name) {
        return i;
      }
    }
    return nullptr;
  }
  Class* findRaiseIfNotFound(const stringType& name) {
    for (auto& i : classes_) {
      if (i->name() == name) {
        return i;
      }
    }
    __RaiseRuntimeError__(L"Class " + name + L" not found");
    return nullptr;
  }
  const Class* findRaiseIfNotFound(const stringType& name) const {
    for (auto& i : classes_) {
      if (i->name() == name) {
        return i;
      }
    }
    __RaiseRuntimeError__(L"Class " + name + L" not found");
    return nullptr;
  }
};              /*}}}*/
struct Module { /*{{{*/
  private:
  stringType name_;
  FuncManager funcMgr_;
  ClassManager classMgr_;

  public:
  stringType& name() { return name_; }
  const stringType& name() const { return name_; }
  FuncManager& funcManager() { return funcMgr_; }
  const FuncManager& funcManager() const { return funcMgr_; }
  std::list<Func*>& functions() { return funcMgr_.list(); }
  const std::list<Func*>& functions() const { return funcMgr_.list(); }
  ClassManager& classManager() { return classMgr_; }
  const ClassManager& classManager() const { return classMgr_; }
  std::list<Class*>& classes() { return classMgr_.list(); }
  const std::list<Class*>& classes() const { return classMgr_.list(); }
  Module() {}
  Module(const stringType& name, const FuncManager& funcs, const ClassManager& classes)
      : name_(name), funcMgr_(funcs), classMgr_(classes) {}
  Module(const Module& m)
      : name_(m.name()), funcMgr_(m.funcManager()), classMgr_(m.classManager()) {}
  Module(const Module&& m)
      : name_(m.name()), funcMgr_(m.funcManager()), classMgr_(m.classManager()) {}
  ~Module() {}
  Module& operator=(const Module& m) {
    name_ = m.name();
    funcMgr_ = m.funcManager();
    classMgr_ = m.classManager();
    return *this;
  }
  Module& operator=(const Module&& m) {
    name_ = m.name();
    funcMgr_ = m.funcManager();
    classMgr_ = m.classManager();
    return *this;
  }
  void registFunc(Func* f) { funcMgr_.regist(f); }
  void registClass(Class* c) { classMgr_.regist(c); }
  Func* findFunc(const stringType& s) { return funcMgr_.find(s); }
  Func* findFunc(const stringType& s, const VarModelManager& v) { return funcMgr_.find(s, v); }
  const Func* findFunc(const stringType& s) const { return funcMgr_.find(s); }
  const Func* findFunc(const stringType& s, const VarModelManager& v) const {
    return funcMgr_.find(s, v);
  }
  Class* findClass(const stringType& s) { return classMgr_.find(s); }
  const Class* findClass(const stringType& s) const { return classMgr_.find(s); }
};                     /*}}}*/
struct ModuleManager { /*{{{*/
  private:
  std::list<Module*> modules_;

  public:
  std::list<Module*>& modules() { return modules_; }
  const std::list<Module*>& modules() const { return modules_; }
  ModuleManager() {}
  ModuleManager(const std::list<Module*>& modules) : modules_(modules) {}
  ModuleManager(const ModuleManager& m) : modules_(m.modules()) {}
  ModuleManager(const ModuleManager&& m) : modules_(m.modules()) {}
  ~ModuleManager() {}
  ModuleManager& operator=(const ModuleManager* m) {
    modules_ = m->modules();
    return *this;
  }
  void regist(Module* m) { modules_.emplace_back(m); }
  Module* find(const stringType& s) {
    for (auto& i : modules_) {
      if (i->name() == s) {
        return i;
      }
    }
    return nullptr;
  }
  const Module* find(const stringType& s) const {
    for (auto& i : modules_) {
      if (i->name() == s) {
        return i;
      }
    }
    return nullptr;
  }
  Module* findRaiseIfNotFound(const stringType& s) {
    for (auto& i : modules_) {
      if (i->name() == s) {
        return i;
      }
    }
    __RaiseRuntimeError__(L"Module " + s + L" not found");
    return nullptr;
  }
  const Module* findRaiseIfNotFound(const stringType& s) const {
    for (auto& i : modules_) {
      if (i->name() == s) {
        return i;
      }
    }
    __RaiseRuntimeError__(L"Module " + s + L" not found");
    return nullptr;
  }
};           /*}}}*/
struct Env { /*{{{*/
  private:
  ModuleManager mmanager_;
  ModuleManager mumanager_;
  FuncManager fmanager_;
  ClassManager cmanager_;
  VarManager vmanager_;

  public:
  Env() {}
  ModuleManager& moduleManager() { return mmanager_; }
  const ModuleManager& moduleManager() const { return mmanager_; }
  ModuleManager& moduleUsedManager() { return mumanager_; }
  const ModuleManager& moduleUsedManager() const { return mumanager_; }
  FuncManager& funcManager() { return fmanager_; }
  const FuncManager& funcManager() const { return fmanager_; }
  ClassManager& classManager() { return cmanager_; }
  const ClassManager& classManager() const { return cmanager_; }
  VarManager& varManager() { return vmanager_; }
  const VarManager& varManager() const { return vmanager_; }
  void registModule(Module* m) { mmanager_.regist(m); }
  void registUsedModule(Module* m) { mumanager_.regist(m); }
  void registFunc(Func* f) { fmanager_.regist(f); }
  void registClass(Class* c) { cmanager_.regist(c); }
  void registVar(Var* c) { vmanager_.regist(c); }
  Module* findModule(const stringType& name) { return mmanager_.find(name); }
  const Module* findModule(const stringType& name) const { return mmanager_.find(name); }
  Module* findUsedModule(const stringType& name) { return mumanager_.find(name); }
  const Module* findUsedModule(const stringType& name) const { return mumanager_.find(name); }
  Module* findModuleRaiseIfNotFound(const stringType& name) {
    return mmanager_.findRaiseIfNotFound(name);
  }
  const Module* findModuleRaiseIfNotFound(const stringType& name) const {
    return mmanager_.findRaiseIfNotFound(name);
  }
  Module* findUsedModuleRaiseIfNotFound(const stringType& name) {
    return mumanager_.findRaiseIfNotFound(name);
  }
  const Module* findUsedModuleRaiseIfNotFound(const stringType& name) const {
    return mumanager_.findRaiseIfNotFound(name);
  }
  Func* findFunc(const stringType& s) { return fmanager_.find(s); }
  const Func* findFunc(const stringType& s) const { return fmanager_.find(s); }
  Func* findFunc(const stringType& s, const VarModelManager& v) { return fmanager_.find(s, v); }
  const Func* findFunc(const stringType& s, const VarModelManager& v) const {
    return fmanager_.find(s, v);
  }
  Func* findFuncRaiseIfNotFound(const stringType& s) { return fmanager_.findRaiseIfNotFount(s); }
  const Func* findFuncRaiseIfNotFound(const stringType& s) const {
    return fmanager_.findRaiseIfNotFount(s);
  }
  Func* findFuncRaiseIfNotFound(const stringType& s, const VarModelManager& v) {
    return fmanager_.findRaiseIfNotFount(s, v);
  }
  const Func* findFuncRaiseIfNotFound(const stringType& s, const VarModelManager& v) const {
    return fmanager_.findRaiseIfNotFount(s, v);
  }
  Class* findClass(const stringType& s) { return cmanager_.find(s); }
  const Class* findClass(const stringType& s) const { return cmanager_.find(s); }
  Class* findClassRaiseIfNotFound(const stringType& s) { return cmanager_.findRaiseIfNotFound(s); }
  const Class* findClassRaiseIfNotFound(const stringType& s) const {
    return cmanager_.findRaiseIfNotFound(s);
  }
  Var* findVar(const stringType& s) { return vmanager_.find(s); }
  Var* findVarRaiseIfNotFound(const stringType& s) { return vmanager_.findRaiseIfNotFound(s); }
  const Var* findVar(const stringType& s) const { return vmanager_.find(s); }
  const Var* findVarRaiseIfNotFound(const stringType& s) const {
    return vmanager_.findRaiseIfNotFound(s);
  }
  ~Env() {}
}; /*}}}*/
Env envLocal;

// Tokenizer Parser and Lexer
struct Token { /*{{{*/
  private:
  stringType str_;
  stringType type_;

  public:
  stringType& str() { return str_; }
  const stringType& str() const { return str_; }
  stringType& type() { return type_; }
  const stringType& type() const { return type_; }
  Token() {}
  Token(const stringType& type, const stringType& str__) : str_(str__), type_(type) {}
  Token(const Token& t) : str_(t.str()), type_(t.type()) {}
  ~Token() {}
  Token& operator=(const Token& t) {
    str_ = t.str();
    type_ = t.type();
    return *this;
  }
  Token& operator=(const Token&& t) {
    str_ = t.str();
    type_ = t.type();
    return *this;
  }
  bool operator==(const Token& t) const { return str_ == t.str() && type_ == t.type(); }
  bool operator!=(const Token& t) const { return str_ != t.str() || type_ != t.type(); }
}; /*}}}*/

using TokenList = std::vector<Token>;
class Tokenizer { /*{{{*/
  public:
  static TokenList tokenize(stringType data) {
    struct StrHelper {
      struct CharHelper {
        char c_;
        CharHelper(char c) : c_(c) {}
        size_t isOneOf(const char* c, size_t l) {
          for (size_t i = 0; i != l; ++i) {
            if (c_ == c[i]) {
              return i;
            }
          }
          return -1;
        }
      };
      stringType& p;
      size_t pos;
      StrHelper(stringType& s) : p(s), pos(0) {}
      std::list<stringType> devide() {
        std::list<stringType> ret;
        int p1 = 0, p2 = -1;
        // p1: this find
        // p2: last find
        while (p1 < p.length()) {
          if (CharHelper(p[p1]).isOneOf(__wasted.data(), __wasted.size()) != -1) {
            if (p2 + 1 != p1) {
              ret.emplace_back(p.substr(p2 + 1, p1 - p2 - 1));
            }
            p2 = p1;
            ++p1;
            continue;
          } else if (CharHelper(p[p1]).isOneOf(__identifies.data(), __identifies.size()) != -1) {
            if (p1 != p2 + 1) {
              ret.emplace_back(p.substr(p2 + 1, p1 - p2 - 1));
            }
            ret.emplace_back(p.substr(p1, 1));
            p2 = p1;
            ++p1;
            continue;
          } else if (pos = CharHelper(p[p1]).isOneOf(__string_pair.data(), __string_pair.size()),
                     pos != -1) {
            size_t __last = p1, __cur = p1 + 1;
            while (p[__cur] != __string_pair[pos]) {
              ++__cur;
            }
            ret.emplace_back(p.substr(__last, __cur - __last + 1));
            p1 = __cur;
            p2 = p1;
            ++p1;
            continue;
          } else {
            ++p1;
            continue;
          }
        }
        if (p1 == p.length() && p2 + 1 != p1) {
          ret.emplace_back(p.substr(p2 + 1, p1 - p2));
        }
        return ret;
      }
    };
    TokenList ret;
    StrHelper sh(data);
    stringType s;
    std::list<stringType> l = sh.devide();
    for (auto& p : l) {
      ret.emplace_back(Token{L"", p});
    }
    return ret;
  }
};                 /*}}}*/
class Syntaxizer { /*{{{*/
  public:
  Syntaxizer(const Env& e) : lenv(e) {}
  const Env& lenv;
  void syntaxize(TokenList& tl) {
    struct BasicSyntaxizer {
      const Env& e;
      std::list<stringType> vtable, ftable, ctable;
      BasicSyntaxizer(const Env& e) : e(e) {}
      void syntaxize(TokenList& tl) {
        struct IsOneOfKeyword {
          bool operator()(const stringType& s) {
            for (auto k : __keywords) {
              if (s == k) {
                return true;
              }
            }
            return false;
          }
        };
        struct IsStrInteger {
          bool operator()(const stringType& s) {
            for (const wchar_t c : s) {
              if (!isdigit(c)) {
                return false;
              }
            }
            return true;
          }
        };
        for (auto i = tl.begin(); i != tl.end(); ++i) {
          if (IsOneOfKeyword()(i->str())) {
            i->type() = L"keyword";
          } else if (IsStrInteger()(i->str())) {
            if (!(i + 1 == tl.end())) {
              if ((i + 1)->str() == L".") {
                if ((i + 2) == tl.end()) {
                  __RaiseInvalidSyntax__(L"incompleted float");
                }
                stringType in = i->str() + L"." + (i + 2)->str();
                auto i3 = tl.erase(i, i + 3);
                i = tl.insert(i3, Token{in, L"float"});
              }
            } else {
              i->type() = L"int";
            }
          } else if (i->str()[0] == L'\"') {
            i->str() = i->str().substr(1, i->str().length() - 2);
            for (auto& pair : __escape_char) {
              ull pos = -1;
              while (pos != -1) {
                i->str().replace(pos, wcslen(pair.first), pair.second);
                pos = i->str().find(pair.first);
              }
            }
            i->type() = L"str";
            std::cout << "In line" << __LINE__ << '\n';
          } else if ((i != tl.begin() && (i - 1)->type() == L"type") ||
                     std::find(vtable.begin(), vtable.end(), i->str()) != vtable.end()) {
            i->type() = L"var";
            vtable.emplace_back(i->str());
          } else if (e.findClass(i->str()) != nullptr ||
                     std::find(ctable.begin(), ctable.end(), i->str()) != ctable.end()) {
            i->type() = L"type";
          } else if (e.findFunc(i->str()) != nullptr ||
                     std::find(ftable.begin(), ftable.end(), i->str()) != ftable.end()) {
            i->type() = L"func";
          }
        }
      }
    };
    BasicSyntaxizer(lenv).syntaxize(tl);
  }
};                                                          /*}}}*/

std::list<TokenList> tokensToSentence(const TokenList& t) { /*{{{*/
  std::list<TokenList> ret;
  auto last = t.begin(), cur = last;
  for (; cur < t.end(); ++cur) {
    if (cur->str() == L";") {
      ret.emplace_back(TokenList(last, cur));
      ++cur;
      last = cur;
    }
  }
  if (last != t.end()) {
    ret.emplace_back(TokenList(last, t.end()));
  }
  return ret;
} /*}}}*/

struct ASTNode { /*{{{*/
  private:
  stringType attrib_;
  stringType data_;
  std::list<ASTNode*> children_;

  public:
  ASTNode() {}
  ASTNode(const stringType& data, const stringType& attrib, const std::list<ASTNode*> c)
      : attrib_(attrib), data_(data), children_(c) {}
  ASTNode(const stringType& data, const stringType& attrib) : attrib_(attrib), data_(data) {}
  ASTNode(const ASTNode& a) : attrib_(a.attribute()), data_(a.data()) {
    for (auto& i : a.children()) {
      children_.emplace_back(gc.create<ASTNode>(*i));
    }
  }
  ASTNode(const ASTNode&& a) : attrib_(a.attribute()), data_(a.data()) {
    for (auto& i : a.children()) {
      children_.emplace_back(gc.create<ASTNode>(*i));
    }
  }
  ~ASTNode() {}
  stringType& attribute() { return attrib_; }
  const stringType& attribute() const { return attrib_; }
  stringType& data() { return data_; }
  const stringType& data() const { return data_; }
  std::list<ASTNode*>& children() { return children_; }
  const std::list<ASTNode*>& children() const { return children_; }
  void addChild(ASTNode* p) { children_.emplace_back(p); }
  ASTNode& operator=(const ASTNode& a) {
    attrib_ = a.attribute();
    data_ = a.data();
    children_ = a.children();
    return *this;
  }
}; /*}}}*/

void printAstTree__(const ASTNode* root, const stringType& a = L"  ") {
  a.toStdOut();
  root->attribute().toStdOut();
  NRT::WString(L"|").toStdOut();
  root->data().toStdOut();
  NRT::WString(L"\n").toStdOut();
  for (const auto& i : root->children()) {
    printAstTree__(i, a + L"  ");
  }
}
void printAstTree(const ASTNode* root) { printAstTree__(root, L"  "); }
struct ASTBuilder { /*{{{*/
  static Env* vEnv;
  static ASTNode* build(TokenList& tl) {
    for (auto& i : tl) {
      i.str().toStdOut();
      NRT::WString(L"\n").toStdOut();
    }
    struct MiddleOperatorRangeChecker {
      static void check(TokenList::iterator cur, const TokenList& l) {
        if (cur == l.end() || cur == l.begin() || cur - 1 != l.begin()) {
          __RaiseInvalidSyntax__(L"MiddleOperatorRangeChecker failed");
        }
      }
    };
    ASTNode* ret = gc.create<ASTNode>();
    // We should kill all keywords here
    ASTNode* root = gc.create<ASTNode>();
    for (auto i = tl.begin(); i != tl.end(); ++i) {
      if (i->str() == L"=") { /*{{{*/
        MiddleOperatorRangeChecker::check(i, tl);
        root->attribute() = L"middle-operator";
        root->data() = L"=";
        TokenList __r(i + 1, tl.end());
        root->addChild(gc.create<ASTNode>(tl.begin()->str(), L"var"));
        root->addChild(build(__r));
        break;                                         /*}}}*/
      } else if (i->type() == L"str") {                /*{{{*/
        root = (gc.create<ASTNode>(i->str(), L"str")); /*}}}*/
      } else if (i->type() == L"type") {               /*{{{*/
        if (i + 1 == tl.end()) {
          __RaiseInvalidSyntax__(L"Uncompleted [type] [varName]");
        }
        if ((i + 1)->type() != L"var") {
          __RaiseInvalidSyntax__(L"[type] [varName] required");
        }
        root = gc.create<ASTNode>(i->str(), L"new-var");
        root->addChild(gc.create<ASTNode>((i + 1)->str(), L"var-name"));
        ++i;
        continue;
        /*}}}*/
      } else if (i->str() == L"import") { /*{{{*/
        if ((i + 1) == tl.end() || (i + 2) != tl.end()) {
          __RaiseInvalidSyntax__(L"incompleted `import`");
        }
        root = (gc.create<ASTNode>((i + 1)->str(), L"use-package"));
        if (vEnv->findModule((i + 1)->str()) == nullptr) {
          __RaiseCompilationError__(L"Module " + (i + 1)->str() + L" not found");
        }
        break;
        /*}}}*/
      } else if (i->str() == L".") { /*{{{*/
        if (i == tl.begin()) {
          __RaiseCompilationError__(L"`.` requires left expression");
        }
        if ((i + 1) == tl.end()) {
          __RaiseCompilationError__(L"`.` requires right expression");
        }
        stringType s = ((i - 1)->str()) + L"." + ((i + 1)->str());
        i = tl.erase(i - 1, i + 2);
        i = tl.insert(i, Token(s, L"function"));
        /*}}}*/
      } else if (i->type() == L"func") { /*{{{*/
        if (i + 1 != tl.end() && (i + 1)->str() == L"(") {
          auto __start = i + 1;
          auto __move = __start;
          long long __count[3]{0, 0, 0};  // ( [ {
          while (__move != tl.end()) {
            if (__count[0] == 1 && __count[1] == 0 && __count[2] == 0 && __move->str() == L",") {
              // 找到一个参数
              TokenList __param(__start + 1, __move);
              auto __ast = build(__param);
              ret->addChild(__ast);
              __start = __move;
              if (__start == tl.end()) {
                __RaiseCompilationError__(L"Uncompleted param");
              }
            } else if (__move->str() == L"(") {
              ++__count[0];
            } else if (__move->str() == L")") {
              --__count[0];
            } else if (__move->str() == L"[") {
              ++__count[1];
            } else if (__move->str() == L"]") {
              --__count[1];
            } else if (__move->str() == L"{") {
              ++__count[2];
            } else if (__move->str() == L"}") {
              --__count[2];
            }
            if (__count[0] == 0 && __count[1] == 0 && __count[2] == 0) {
              break;
            }
            ++__move;
          }
          // 获取最后一个参数
          // , ... )
          // __start, ,__move-1
          if (__move == tl.end()) {
            if (__count[0] != 0) {
              __RaiseCompilationError__(L"Uncompleted `()");
            }
            if (__count[1] != 0) {
              __RaiseCompilationError__(L"Uncompleted `[]`");
            }
            if (__count[2] != 0) {
              __RaiseCompilationError__(L"Uncompleted `{}`");
            }
          }
          root = gc.create<ASTNode>(i->str(), L"func");
          if (__start + 1 != __move) {
            TokenList __param(__start + 1, __move);
            ASTNode* __ast = build(__param);
            root->addChild(__ast);
          }
          i = __move;
        }

        /*}}}*/
      } else if (i->str() == L")") {                      /*{{{*/
        __RaiseCompilationError__(L"pair `()` required"); /*}}}*/
      } else {                                            /*{{{*/
        root = gc.create<ASTNode>(i->str(), i->type());
      }                                                   /*}}}*/
    }
    ret = root;
    return ret;
  }
}; /*}}}*/
// CMLReg only store pointer to object
struct CMLReg { /*{{{*/
  private:
  ull data_;

  public:
  ull data() { return data_; }
  const ull data() const { return data_; }
  void*& dataAsPointer() { return (void*&)data_; }
  const void*& dataAsPointer() const { return (const void*&)data_; }
  CMLReg() {}
  CMLReg(const CMLReg& c) : data_(c.data()) {}
  CMLReg(const CMLReg&& c) : data_(c.data()) {}
  ~CMLReg() {}
  CMLReg& operator=(const CMLReg& c) {
    data_ = c.data();
    return *this;
  }
  template <class C>
    requires requires { (ull)(C()); }
  CMLReg& operator=(const C& c) {
    data_ = (ull)c;
    return *this;
  }
  bool operator==(const CMLReg& c) const { return data_ == c.data(); }
  bool operator!=(const CMLReg& c) const { return data_ != c.data(); }
  bool operator>=(const CMLReg& c) const { return data_ >= c.data(); }
  bool operator>(const CMLReg& c) const { return data_ > c.data(); }
  bool operator<=(const CMLReg& c) const { return data_ <= c.data(); }
  bool operator<(const CMLReg& c) const { return data_ < c.data(); }
};              /*}}}*/
struct CMLEnv { /*{{{*/
  private:
#define CML_REG_COUNT (10)
#define CML_REG_RETURN (0)
  CMLReg regs[10];

  public:
  CMLReg& reg(ull i) { return regs[i]; }
  const CMLReg& reg(ull i) const { return regs[i]; }
  CMLEnv() {}
  CMLEnv(const CMLEnv& c) {
    for (ull i = 0; i != CML_REG_COUNT; ++i) {
      regs[i] = c.reg(i);
    }
  }
  CMLEnv(const CMLEnv&& c) {
    for (ull i = 0; i != CML_REG_COUNT; ++i) {
      regs[i] = c.reg(i);
    }
  }
  ~CMLEnv() {}
  CMLEnv& operator=(const CMLEnv& c) {
    for (ull i = 0; i != CML_REG_COUNT; ++i) {
      regs[i] = c.reg(i);
    }
    return *this;
  }
};             /*}}}*/
enum CMLTYPE { /*{{{*/
               // func[Func*], retreg[ull ]
               CML_CALL_FUNC = 1,
               // reg[ull], func[Func*]
               CML_PUSH_REG_TO_FUNC = 3,
               // mem[Var*], func[Func*]
               CML_PUSH_ADDR_TO_FUNC = 4,
               // func[Func*], retreg[ull]
               CML_POP_FUNC_TO_REG = 5,
               // mem[Var*], reg[ull]
               CML_MOVE_ADDR_TO_REG = 6,
               // reg[ull], mem[Var*]
               CML_MOVE_REG_TO_ADDR = 7,
               // reg[ull], reg[ull]
               CML_MOVE_REG_TO_REG = 8,
               // mem[Var*], mem[Var*]
               CML_MOVE_ADDR_TO_ADDR = 9,
               // var(Var*), NONE
               CML_STORE = 10,
               // v1[ ull ], v2[ ull ]
               CML_ASSIGN_VAR_REG_TO_VAR_REG = 11,
               CML_PLACEHOLDER = (ull)-1
};           /*}}}*/
struct CML { /*{{{*/
  private:
  ull id_;
  ull param1_, param2_;

  public:
  ull& id() { return id_; }
  const ull& id() const { return id_; }
  ull& param1() { return param1_; }
  const ull& param1() const { return param1_; }
  void*& param1AsPointer() { return (void*&)param1_; }
  const void*& param1AsPointer() const { return (const void*&)param1_; }
  ull& param2() { return param2_; }
  const ull& param2() const { return param2_; }
  void*& param2AsPointer() { return (void*&)param2_; }
  const void*& param2AsPointer() const { return (const void*&)param2_; }
  CML() {}
  CML(ull id, ull param1, ull param2) : id_(id), param1_(param1), param2_(param2) {}
  CML(const CML& c) : id_(c.id()), param1_(c.param1()), param2_(c.param2()) {}
  CML(const CML&& c) : id_(c.id()), param1_(c.param1()), param2_(c.param2()) {}
  CML& operator=(const CML& c) {
    id_ = c.id();
    param1_ = c.param1();
    param2_ = c.param2();
    return *this;
  }
  ~CML() {}

};                  /*}}}*/
struct CMLPackage { /*{{{*/
  private:
  std::vector<CML*> base_;
  CMLEnv env;

  public:
  std::vector<CML*>& code() { return base_; }
  const std::vector<CML*>& code() const { return base_; }
  CMLPackage() {}
  CMLPackage(const CMLPackage& c) : base_(c.code()) {}
  CMLPackage(const CMLPackage&& c) : base_(c.code()) {}
  CMLPackage(const std::vector<CML*>& c) : base_(c) {}
  ~CMLPackage() {}
  CMLPackage& operator=(const CMLPackage& c) {
    base_ = c.code();
    return *this;
  }
  void add(CML* code) { base_.emplace_back(code); }
  void add(CMLPackage* code) {
    for (auto& i : code->code()) {
      base_.emplace_back(i);
    }
  }
}; /*}}}*/
void printCMLPackage(CMLPackage* c) {
#define TEST_CML(attr) ((i->id()) == (attr) ? std::string(#attr) : std::string(""))
  for (auto& i : c->code()) {
    std::cout << TEST_CML(CML_CALL_FUNC) + TEST_CML(CML_PUSH_REG_TO_FUNC) +
                     TEST_CML(CML_PUSH_ADDR_TO_FUNC) + TEST_CML(CML_POP_FUNC_TO_REG) +
                     TEST_CML(CML_PLACEHOLDER) + TEST_CML(CML_MOVE_REG_TO_REG) +
                     TEST_CML(CML_MOVE_ADDR_TO_REG) + TEST_CML(CML_MOVE_REG_TO_ADDR) +
                     TEST_CML(CML_MOVE_ADDR_TO_ADDR) + TEST_CML(CML_STORE) +
                     TEST_CML(CML_ASSIGN_VAR_REG_TO_VAR_REG);
    std::cout << " " << std::hex << i->param1() << ", " << i->param2() << std::dec << '\n';
  }
}

Env* ASTBuilder::vEnv = &envLocal;
struct LambdaVarNameGenerator { /*{{{*/
  private:
  ull cur = 0;

  public:
  stringType generate() {
    using namespace std::string_literals;
    ++cur;
    return L"?V?" + NRT::toWString(cur);
  }
} lvnGenerator;                  /*}}}*/
struct LambdaFuncNameGenerator { /*{{{*/
  private:
  ull cur = 0;

  public:
  stringType generate() {
    using namespace std::string_literals;
    ++cur;
    return L"?F?" + NRT::toWString(cur);
  }
} lfnGenerator;                   /*}}}*/
struct LambdaClassNameGenerator { /*{{{*/
  private:
  ull cur = 0;

  public:
  stringType generate() {
    using namespace std::string_literals;
    ++cur;
    return L"?C?" + NRT::toWString(cur);
  }
} lcnGenerator;   /*}}}*/
struct Compiler { /*{{{*/
  Env* vEnv;
  Compiler(Env* e) : vEnv(e) {}
  CMLPackage* codeToCML(ASTNode* root) {
    CMLPackage* ret = gc.create<CMLPackage>();
    if (root->attribute() == L"func") {
      Func* f = vEnv->findFuncRaiseIfNotFound(root->data());
      for (auto& j : root->children()) {
        CMLPackage* b = codeToCML(j);
        ret->add(b);
        ret->add(gc.create<CML>(CML_PUSH_REG_TO_FUNC, CML_REG_RETURN, (ull)f));
      }
      ret->add(gc.create<CML>(CML_CALL_FUNC, (ull)f, CML_REG_RETURN));
    } else if (root->attribute() == L"str") {
      Var* v = gc.create<Var>(L"", root->data());
      ret->add(gc.create<CML>(CML_MOVE_ADDR_TO_REG, (ull)v, CML_REG_RETURN));
    } else if (root->attribute() == L"var")
    // 根据名字获取地址, 返回在reg[0]
    {
      Func* f1 = vEnv->findFuncRaiseIfNotFound(L"$get-var-pointer-by-name-raise-if-not-found");
      ret->add(
          gc.create<CML>(CML_PUSH_ADDR_TO_FUNC, (ull)gc.create<Var>(L"", root->data()), (ull)f1));
      ret->add(gc.create<CML>(CML_CALL_FUNC, (ull)f1, CML_REG_RETURN));
    } else if (root->attribute() == L"middle-operator") {
      // 对于每个操作符, 将其转为对应的 =(&Var, &Var)
      if (root->data() == L"=") {
        Func* f1 = vEnv->findFuncRaiseIfNotFound(L"$get-var-pointer-by-name-raise-if-not-found",
                                                 VarModelManager{L"str"});
        Func* f2 = vEnv->findFuncRaiseIfNotFound(L"$new-var", VarModelManager{L"str", L"str"});
        // let left operand to ref
        ret->add(gc.create<CML>(CML_PUSH_ADDR_TO_FUNC,
                                (ull)gc.create<Var>(L"", root->children().front()->data()),
                                (ull)f1));
        ret->add(gc.create<CML>(CML_CALL_FUNC, (ull)f1, 1));
        CMLPackage* __cr_operand = codeToCML(root->children().back());
        ret->add(__cr_operand);
        ret->add(gc.create<CML>(CML_ASSIGN_VAR_REG_TO_VAR_REG, CML_REG_RETURN, 1));
      }
    } else if (root->attribute() == L"new-var") {
      ret->add(gc.create<CML>(CML_PUSH_ADDR_TO_FUNC, (ull)gc.create<Var>(L"", root->data()),
                              (ull)vEnv->findFuncRaiseIfNotFound(L"$new-var")));
      ret->add(gc.create<CML>(CML_PUSH_ADDR_TO_FUNC,
                              (ull)gc.create<Var>(L"", root->children().front()->data()),
                              (ull)vEnv->findFuncRaiseIfNotFound(L"$new-var")));
      ret->add(gc.create<CML>(CML_CALL_FUNC, (ull)vEnv->findFuncRaiseIfNotFound(L"$new-var"),
                              CML_REG_RETURN));
    }
    return ret;
  }
  ~Compiler() {}
};                 /*}}}*/
struct Optimizer { /*{{{*/
  static void optimizeCML(CMLPackage* c) {
    for (auto i = c->code().begin(); i != c->code().end(); ++i) {
      if (i + 1 != c->code().end()) {
        if ((*i)->id() == CML_MOVE_ADDR_TO_REG && (*(i + 1))->id() == CML_PUSH_REG_TO_FUNC &&
            (*i)->param2() == (*(i + 1))->param1()) {
          CML* pcml =
              gc.create<CML>(CML_PUSH_ADDR_TO_FUNC, (ull)(*i)->param1(), (ull)(*(i + 1))->param2());
          i = c->code().erase(i);
          i = c->code().erase(i);
          i = c->code().insert(i, pcml);
        }
      }
    }
  }
};                   /*}}}*/
struct Initializer { /*{{{*/
  public:
  static void init(Env& envLocal) {
    // Module
    VarModelManager vm1;
    vm1.regist(gc.create<VarModel>(typeinfo(L"str")));
    envLocal.registFunc(gc.create<Func>(L"print", vm1, [](const Stack* s) -> Var* {
      for (auto& i : s->list()) {
        i->toStr().toStdOut();
      }
      return gc.create<Var>();
    }));
    VarModelManager vm2;
    envLocal.registFunc(gc.create<Func>(L"input", vm2, [](const Stack* s) -> Var* {
      return gc.create<Var>(L"", stringType::fromStdInReadLine());
    }));
    VarModelManager vm3;
    vm3.regist(gc.create<VarModel>(typeinfo(L"str")));
    vm3.regist(gc.create<VarModel>(typeinfo(L"str")));
    envLocal.registFunc(gc.create<Func>(L"$new-var", vm3, [&envLocal](const Stack* s) -> Var* {
      Var* ret = gc.create<Var>(s->list().back()->toStr(), typeinfo(s->list().front()->toStr()));
      envLocal.registVar(ret);
      return ret;
    }));
    VarModelManager vm4;
    vm4.regist(gc.create<VarModel>(typeinfo(L"str")));
    envLocal.registFunc(gc.create<Func>(
        L"$get-var-pointer-by-name-raise-if-not-found", vm4, [&envLocal](const Stack* s) -> Var* {
          return envLocal.findVarRaiseIfNotFound((*s->begin())->toStr());
        }));
    VarModelManager vm5;
    vm5.regist(gc.create<VarModel>(typeinfo(L"ref")));
    vm5.regist(gc.create<VarModel>(typeinfo(L"ref")));
    envLocal.registFunc(
        gc.create<Func>(L"$let-var-to-var", vm5, [&envLocal](const Stack* s) -> Var* {
          *s->list().front() = *s->list().back();
          return gc.create<Var>();
        }));

    VarModelManager vm6;
    vm6.regist(gc.create<VarModel>(typeinfo(L"ref")));
    vm6.regist(gc.create<VarModel>(typeinfo(L"ref")));
    envLocal.registFunc(gc.create<Func>(L"$=", vm6, [&envLocal](const Stack* s) -> Var* {
      *s->list().front() = *s->list().back();
      return gc.create<Var>();
    }));
    // Class
    envLocal.registClass(gc.create<Class>(L"str"));
  }
};                   /*}}}*/
struct Interpreter { /*{{{*/
  Env* env;
  CMLEnv* cenv;
  Interpreter(Env* e, CMLEnv* cenv) : env(e), cenv(cenv) {}
  void execCML(CMLPackage* c) {
    for (auto& code : c->code()) {
      switch (code->id()) {
        case CML_CALL_FUNC: {
          cenv->reg(code->param2()) = ((Func*)code->param1AsPointer())->call();
          break;
        }
        case CML_PUSH_REG_TO_FUNC: {
          ((Func*)code->param2AsPointer())->push((Var*)(cenv->reg(code->param1()).dataAsPointer()));
          break;
        }
        case CML_PUSH_ADDR_TO_FUNC: {
          ((Func*)code->param2AsPointer())->push((Var*)code->param1AsPointer());
          break;
        }
        case CML_POP_FUNC_TO_REG: {
          Func* f = env->findFuncRaiseIfNotFound(((Var*)code->param1AsPointer())->toStr());
          cenv->reg(code->param2()) = (ull)f->pop();
          break;
        }
        case CML_MOVE_REG_TO_REG: {
          cenv->reg(code->param2()) = cenv->reg(code->param1());
          break;
        }
        case CML_MOVE_ADDR_TO_REG: {
          cenv->reg(code->param2()) = code->param1();
          break;
        }
        case CML_MOVE_REG_TO_ADDR: {
          *(ull*)code->param2() = cenv->reg(code->param1()).data();
          break;
        }
        case CML_MOVE_ADDR_TO_ADDR: {
          *(ull*)code->param2AsPointer() = *(ull*)code->param1AsPointer();
        }
        case CML_ASSIGN_VAR_REG_TO_VAR_REG: {
          *(Var*)cenv->reg(code->param2()).dataAsPointer() =
              *(Var*)cenv->reg(code->param1()).dataAsPointer();
          break;
        }
      }
    }
  }
}; /*}}}*/

int main() {
  Initializer::init(envLocal);
  stringType s1(L"str a;a=input();print(a);");
  auto l1 = Tokenizer::tokenize(s1);
  Syntaxizer(envLocal).syntaxize(l1);
  auto l2 = tokensToSentence(l1);
  CMLEnv cenv;
  for (auto& ps : l2) {
    auto p1 = ASTBuilder::build(ps);
#ifdef __NIRVANA_DEBUG__
    printAstTree(p1);
#endif
    auto p2 = Compiler(&envLocal).codeToCML(p1);
    Optimizer::optimizeCML(p2);
#ifdef __NIRVANA_DEBUG__
    printCMLPackage(p2);
#endif
    Interpreter(&envLocal, &cenv).execCML(p2);
  }
  return 0;
}
