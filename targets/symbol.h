#ifndef __TIL_TARGETS_SYMBOL_H__
#define __TIL_TARGETS_SYMBOL_H__

#include <string>
#include <memory>
#include <cdk/types/basic_type.h>

namespace til {

  class symbol {
    std::string _name;
    std::shared_ptr<cdk::basic_type> _type;
    int _qualifier; // qualifiers: public, forward, external, "private" (i.e., none)
    int _offset = 0; // 0 (zero) means global variable/function
    int _lbl; // label for the function

  public:
    symbol(std::shared_ptr<cdk::basic_type> type, const std::string &name, int qualifier) :
        _name(name), _type(type), _qualifier(qualifier) {
    }

    virtual ~symbol() {
      // EMPTY
    }

    std::shared_ptr<cdk::basic_type> type() const {
      return _type;
    }
    void set_type(std::shared_ptr<cdk::basic_type> type) {
      _type = type;
    }
    bool is_typed(cdk::typename_type name) const {
      return _type->name() == name;
    }
    const std::string &name() const {
      return _name;
    }
    int qualifier() const {
      return _qualifier;
    }
    int offset() const {
      return _offset;
    }
    void offset(int offset) {
      _offset = offset;
    }
    int label() const {
      return _lbl;
    }
    void label(int label) {
      _lbl = label;
    }
    bool function() const {
      return is_typed(cdk::TYPE_FUNCTIONAL);
    }
    bool global() const {
      return _offset == 0;
    }
  };

  inline auto make_symbol(std::shared_ptr<cdk::basic_type> type, const std::string &name, int qualifier) {
    return std::make_shared<symbol>(type, name, qualifier);
  }

} // til

#endif
