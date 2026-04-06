#pragma once

template <class... Ts>
struct overload : Ts... {  // NOLINT(misc-multiple-inheritance)
    using Ts::operator()...;
};
template <class... Ts>
overload(Ts...) -> overload<Ts...>;
