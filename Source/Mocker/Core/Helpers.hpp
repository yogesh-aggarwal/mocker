/// @brief
/// Simplifying the process of performing operations that're very common across
/// the codebase. It also includes aliases to some of the very common terms used
/// in the development process.

#pragma once

#include <cassert>
#include <memory>
#include <tuple>
#include <future>

//-----------------------------------------------------------------------------

#define ASSERT(x) _assert(condition, msg, __LINE__)

//-----------------------------------------------------------------------------
// Easy unique_ptr Ref
//-----------------------------------------------------------------------------

template<typename T>
using UniqueRef = std::unique_ptr<T>;
// Easy creation of Unique References (UniqueRef)
template<typename T, typename... Args>
constexpr UniqueRef<T>
CreateUniqueRef(Args &&...args)
{
   return std::make_unique<T>(std::forward<Args>(args)...);
}

//-----------------------------------------------------------------------------
// Easy shared_ptr Ref
//-----------------------------------------------------------------------------

template<typename T>
using Ref = std::shared_ptr<T>;
// Easy creation of References (Ref)
template<typename T, typename... Args>
constexpr Ref<T>
CreateRef(Args &&...args)
{
   return std::make_shared<T>(std::forward<Args>(args)...);
}

//-----------------------------------------------------------------------------
// Tuple Short-hand notation
//-----------------------------------------------------------------------------

template<typename T1, typename T2>
using Tuple = std::tuple<T1, T2>;
// Easy creation of Tuples
template<typename T1, typename T2>
constexpr Tuple<T1, T2>
CreateTuple(T1 v1, T2 v2)
{
   return std::make_tuple<T1, T2>(v1, v2);
}

//-----------------------------------------------------------------------------
// Pair Short-hand notation
//-----------------------------------------------------------------------------

template<typename T1, typename T2>
using Pair = std::pair<T1, T2>;
// Easy creation of Pairs
template<typename T1, typename T2>
constexpr Pair<T1, T2>
CreatePair(T1 v1, T2 v2)
{
   return std::make_pair<T1, T2>(v1, v2);
}

//-----------------------------------------------------------------------------
// Async Short-hand notation
//-----------------------------------------------------------------------------

template<typename T>
using Async = std::future<T>;

//-----------------------------------------------------------------------------
// English-like, more-understandable type names.
//-----------------------------------------------------------------------------

typedef char        Byte;
typedef Ref<Byte[]> RawString;

//-----------------------------------------------------------------------------
