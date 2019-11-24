#ifndef WG_TOOLS_HPP
#define WG_TOOLS_HPP

template<typename Optional, typename Value>
auto default_val(Optional o, Value def) -> Value { return o ? *o : def; }

#endif // WG_TOOLS_HPP
