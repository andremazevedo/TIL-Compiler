#ifndef __TIL_TARGETS_SYMBOL_H__
#define __TIL_TARGETS_SYMBOL_H__

#include <string>
#include <memory>
#include <cdk/types/basic_type.h>

namespace til {

  class symbol {
    std::string _name;
    std::shared_ptr<cdk::basic_type> _type;
    std::vector<std::shared_ptr<cdk::basic_type>> _argument_types;
    bool _function; // function?
    long _value; // hack!
    
    int _offset = 0; // 0 (zero) means global variable/function

  public:
    symbol(std::shared_ptr<cdk::basic_type> type, const std::string &name, bool function) :
        _name(name), _type(type), _function(function) {
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
    bool function() const {
      return _function;
    }
    long value() const {
      return _value;
    }
    long value(long v) {
      return _value = v;
    }
    int offset() const {
      return _offset;
    }
    int offset(int offset) {
      return _offset = offset;
    }
    bool global() const {
      return _offset == 0;
    }
  };

  inline auto make_symbol(std::shared_ptr<cdk::basic_type> type, const std::string &name, bool function) {
    return std::make_shared<symbol>(type, name, function);
  }

} // til

#endif
