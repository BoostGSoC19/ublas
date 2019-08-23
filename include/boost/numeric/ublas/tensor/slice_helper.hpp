//
//  Copyright (c) 2018-2019, Cem Bassoy, cem.bassoy@gmail.com
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  The authors gratefully acknowledge the support of
//  Fraunhofer IOSB, Ettlingen, Germany
//

#ifndef _BOOST_UBLAS_TENSOR_SLICE_HELPER_HPP
#define _BOOST_UBLAS_TENSOR_SLICE_HELPER_HPP

#include <iostream>
#include <vector>
#include "fwd.hpp"
#include "slice.hpp"

namespace boost::numeric::ublas::span::detail
{

/** @brief static absolute value */
template <ptrdiff_t x>
inline static constexpr auto static_abs = x < 0 ? -x : x;

/** @brief sets the slice to end of extent or index */
inline static constexpr auto end = std::numeric_limits<ptrdiff_t>::max();

/** @brief its a helper class for implementing static slice
 * 
 * @tparam T slice type
 * @tparam f_ starting index of slice
 * @tparam l_ ending index of slice
 * @tparam s_ steps for slice
 * @tparam sz_ size of slice
 * 
 */
template <typename T, ptrdiff_t f_, ptrdiff_t l_, ptrdiff_t s_, ptrdiff_t sz_>
struct slice_helper<T, f_, l_, s_, sz_>
{
    using self_type = slice_helper<T, f_, l_, s_, sz_>;
    using value_type = T;
    using size_type = size_t;

    static constexpr value_type first_ = f_;
    static constexpr value_type last_ = l_;
    static constexpr value_type step_ = s_;
    static constexpr value_type size_ = sz_;

    /** @brief returns the starting of slice */
    TENSOR_STATIC_AUTO_CONSTEXPR_RETURN first() noexcept
    {
        return self_type::first_;
    }

    /** @brief returns the ending of slice */
    TENSOR_STATIC_AUTO_CONSTEXPR_RETURN last() noexcept
    {
        return self_type::last_;
    }

    /** @brief returns the step of slice */
    TENSOR_STATIC_AUTO_CONSTEXPR_RETURN step() noexcept
    {
        return self_type::step_;
    }

    /** @brief returns the size of slice */
    TENSOR_STATIC_AUTO_CONSTEXPR_RETURN size() noexcept
    {
        return self_type::size_;
    }

    // /** @brief returns the relative address of next element
    //  *
    //  * @param idx index of element
    //  *
    //  */
    // TENSOR_CONSTEXPR_RETURN(value_type) operator[](size_type idx) const noexcept
    // {
    //     return first_ + idx * step_;
    // }

    // /** @brief caluates the next slice
    //  *
    //  * @param b of type static slice
    //  *
    //  */
    // template <ptrdiff_t f, ptrdiff_t... Args>
    // TENSOR_AUTO_CONSTEXPR_RETURN operator()(basic_slice<T, f, Args...> const & b) const noexcept
    // {
    //     using lhs_type = self_type;
    //     using rhs_type = typename basic_slice<T, Args...>::self_type;
    //     return basic_slice<T,
    //                        rhs_type::first() * lhs_type::step() + lhs_type::first(),
    //                        rhs_type::last() * lhs_type::step() + lhs_type::first(),
    //                        lhs_type::step() * rhs_type::step()>{};
    // }

    /** @brief prints the slice */
    friend std::ostream& operator<<(std::ostream& os, self_type const& rhs){
        os<<"slice( "<<rhs.first()<<", "<<rhs.last()<<", "<<rhs.step()<<" )";
        return os;
    }
};

/** @brief helper struct for storing normalized static slice */
template <typename T, ptrdiff_t f_, ptrdiff_t l_, ptrdiff_t s_, ptrdiff_t sz = (((l_ - f_) / static_abs<s_>)+1l)>
struct normalized_slice
{
    using type = slice_helper<T, f_, l_, s_, sz>;
};

/** @brief helps to normalize static slice
 * 
 * @tparam T slice type
 * @tparam f_ starting index of slice
 * @tparam l_ ending index of slice
 * @tparam s_ steps for slice
 * 
 */
template <typename T, ptrdiff_t f_, ptrdiff_t l_, ptrdiff_t s_>
struct normalized_slice_helper
{
    TENSOR_AUTO_CONSTEXPR_RETURN operator()() const
    {
        if constexpr (f_ < 0 && l_ < 0 && s_ > 0)
        {
            return normalized_slice<T, f_, l_, s_, 0l>{};
        }
        else if constexpr (f_ == l_)
        {
            return normalized_slice<T, f_, l_, s_, 1l>{};
        }
        else
        {
            static_assert(s_ != 0, "Error in basic_static_span::basic_static_span : cannot have a s_ equal to zero.");
            static_assert(s_ > 0, "Error in basic_static_span::basic_static_span : cannot have a s_ less than 0");
            if constexpr (f_ >= 0 && l_ >= 0)
            {

                if constexpr (f_ > l_ && s_ > 0)
                {
                    throw std::out_of_range("Error in basic_static_span::basic_static_span: l_ is smaller than f_");
                }

                if constexpr (l_ == detail::end)
                {
                    return normalized_slice<T, f_, l_, s_, detail::end>{};
                }
                else
                {
                    return normalized_slice<T, f_, (l_ - (l_ - f_) % static_abs<s_>), s_>{};
                }
            }
            else
            {
                return normalized_slice<T, f_, l_, s_, 0l>{};
            }
        }
    }
};

/** @brief type alias for normalized slice*/
template <typename T, ptrdiff_t f_, ptrdiff_t l_, ptrdiff_t s_>
using slice_helper_t = typename decltype(normalized_slice_helper<T, f_, l_, s_>{}())::type;

/** @brief type list for storing types such as static slices as it cannot be stored in std::vector
 * 
 * @tparam Ts parameter pack containing different types
 * 
*/
template <typename... Ts>
struct list
{
    /** @brief returns the size of parameter pack */
    TENSOR_AUTO_CONSTEXPR_RETURN size() const noexcept
    {
        return sizeof...(Ts);
    }
};

/** @brief type trait for checking of type is a type list or not
 * 
 * @tparam T any type
 * 
*/
template <typename T>
struct is_list : std::false_type
{
};

template <typename... Ts>
struct is_list<list<Ts...>> : std::true_type
{
};

/** @brief pushes the type to the front of type list */
template <typename T, typename... Ts>
TENSOR_AUTO_CONSTEXPR_RETURN push_front(list<Ts...>, T)->list<T, Ts...>{return {};}

/** @brief pops the type from the front of type list */
template <typename T, typename... Ts>
TENSOR_AUTO_CONSTEXPR_RETURN pop_front(list<T, Ts...>)->list<Ts...>{return {};}

/** @brief pushes the type to the back of type list */
template <typename T, typename... Ts>
TENSOR_AUTO_CONSTEXPR_RETURN push_back(list<Ts...>, T)->list<Ts..., T>{return {};}

/** @brief pops and pushes from the type list 
 * 
 * @return std::pair poped type and new type list
 * 
*/
template <typename T, typename... Ts>
TENSOR_AUTO_RETURN pop_and_get_front(list<T, Ts...>)
{
    return std::make_pair(T{}, list<Ts...>{});
}

/** @brief helper function for returns the type at given position from type list
 * 
 * @tparam I counter as you go deeper in recursive function
 * @param of type type list 
 * @return any type at index I
 * 
 */
template <size_t I, typename T, typename... Ts>
auto get_helper(list<T, Ts...>)
{
    if constexpr (I == 0)
    {
        return T{};
    }
    else
    {
        return get_helper<I - 1>(list<Ts...>{});
    }
}

/** @brief returns the type at given position from type list
 * 
 * @tparam I counter as you go deeper in recursive function
 * @param l of type type list 
 * @return any type at index I
 * 
 */
template <size_t I, typename... Ts>
TENSOR_AUTO_CONSTEXPR_RETURN get(list<Ts...> const &l)
{
    if constexpr (sizeof...(Ts) <= I)
    {
        throw std::out_of_range("boost::numeric::ublas::span::detail::get() : out of bound");
    }
    else
    {
        return get_helper<I>(l);
    }
}

/** @brief helper strut or proxy class for iterating over type list or std::vector
 * 
 * @tparam I counter as you go deeper in recursive function
 * @tparam CallBack 
 * @tparam T type contained in type list
 * @tparam Ts remaing types in type list
 * 
 */
template <size_t I, class CallBack, class T, class... Ts>
struct for_each_list_impl
{
    constexpr decltype(auto) operator()(list<T, Ts...>, CallBack call_back)
    {
        using new_list = list<Ts...>;
        using value_type = T;
        call_back(I, value_type{});

        if constexpr (sizeof...(Ts) != 0)
        {
            for_each_list_impl<I + 1, CallBack, Ts...> it;
            it(new_list{}, call_back);
        }
    }

    template <typename U>
    constexpr decltype(auto) operator()(std::vector<basic_slice<U>> const &l, CallBack call_back)
    {
        for (auto i = 0u; i < l.size(); i++)
        {
            call_back(i, l[i]);
        }
    }
};

/** @brief helps to iterate over type list
 * 
 * @code 
 * auto l = list<...>{};
 * for_each(l,[](auto const& I, auto const& t){...});
 * @endcode
 * 
 * @tparam CallBack 
 * @tparam Ts parameter pack of types in type list
 * @param l of type type list
 * @param call_back of type generic lambda which has function signature func(size_t,auto)
 * 
 */
template <class CallBack, class... Ts>
auto for_each_list(list<Ts...> const &l, CallBack call_back)
{
    for_each_list_impl<0, CallBack, Ts...> f;
    f(l, call_back);
}

/** @brief helps to iterate over std::vector
 * 
 * @code 
 * auto l = std::vector<...>{...};
 * for_each(l,[](auto const& I, auto const& t){...});
 * @endcode
 * 
 * @tparam CallBack 
 * @tparam T type of basic_slice
 * @param l of type std::vector<basic_slice<T>>
 * @param call_back of type generic lambda which has function signature func(size_t,auto)
 * 
 */
template <class CallBack, typename T>
auto for_each_list(std::vector<basic_slice<T>> const &l, CallBack call_back)
{
    for_each_list_impl<0, CallBack, int> f;
    f(l, call_back);
}

// template<size_t I, typename... Ts>
// TENSOR_AUTO_CONSTEXPR_RETURN get( list<Ts...> l, size_t i ){
//     static_assert( I < 3, "boost::numeric::ublas::span::detail::get : invalid index");
//     if ( sizeof...(Ts) <= i) throw std::out_of_range("boost::numeric::ublas::span::detail::get : out of bound");

//     size_t val = end;
//     for_each_list( l, [&]( auto const& j, auto const& s ){
//         if ( i == j ){
//             if constexpr ( I == 0 ) val = s.first();
//             else if constexpr ( I == 1 ) val = s.last();
//             else val = s.step();
//         }
//     });
//     return val;
// }

// template<size_t I, typename T>
// TENSOR_AUTO_CONSTEXPR_RETURN get( std::vector< basic_slice<T> > const& v, size_t i ){
//     static_assert( I < 3, "boost::numeric::ublas::span::detail::get : invalid index");
//     if ( v.size() <= i) throw std::out_of_range("boost::numeric::ublas::span::detail::get : out of bound");

//     size_t val = end;
//     auto s = v[i];
//     if constexpr ( I == 0 ) val = s.first();
//     else if constexpr ( I == 1 ) val = s.last();
//     else val = s.step();
//     return val;
// }

/** brief type trait contains the common type of basic_slice from the type list */
template <typename T, typename... Ts>
struct slice_common_type<T, Ts...>
{
    using type = ptrdiff_t;
};

template <typename U, ptrdiff_t... Args, typename... Ts>
struct slice_common_type<basic_slice<U, Args...>, Ts...>
{
    using type = std::common_type_t<U, typename slice_common_type<Ts...>::type>;
};

template <>
struct slice_common_type<>
{
    using type = typename slice_common_type<int>::type;
};

/** @brief normalize the dynamic val within bounds of extents*/
TENSOR_AUTO_CONSTEXPR_RETURN noramlize_value(size_t ext, ptrdiff_t val)
{
    if (val < 0)
    {
        auto const ret = static_cast<ptrdiff_t>( ext + val );
        if (ret < 0)
        {
            throw std::out_of_range("boost::numeric::ublas::span::detail::normalize_val : invalid slice ");
        }
        return ret;
    }
    else
    {
        return val;
    }
}

/** @brief normalize the static val within bounds of extents*/
template <size_t ext, ptrdiff_t val>
TENSOR_AUTO_CONSTEXPR_RETURN noramlize_value()
{
    if constexpr (val < 0)
    {
        constexpr auto const ret = static_cast<ptrdiff_t>( ext + val );
        if constexpr (ret < 0)
        {
            throw std::out_of_range("boost::numeric::ublas::span::detail::normalize_val : invalid slice ");
        }
        return ret;
    }
    else
    {
        return val;
    }
}

} // namespace boost::numeric::ublas::span::detail

#endif