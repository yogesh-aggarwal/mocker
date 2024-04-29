#pragma once

#include <memory>
#include <functional>

#include <Mocker/Core/Error.hpp>
#include <Mocker/Core/Helpers.hpp>

template<typename T>
struct Result
{
   T          value;
   Ref<Error> error;

public:
   Result(T value) : value(value), error(nullptr) {}
   Result(T value, Error *error) : value(value), error(Ref<Error>(error)) {}
   Result(T value, Ref<Error> error) : value(value), error(error) {}

   Result(const Result<T> &other) : value(other.value), error(other.error) {}
   Result(Result<T> &&other) : value(std::move(other.value)), error(other.error)
   {
   }

   ~Result() = default;

   operator bool() const
   {
      if (error == nullptr) return true;
      if (!!*error) return false;

      return true;
   }

   T *
   operator->() const
   {
      return value.get();
   }

   Result<T> &
   operator=(const Result<T> &other)
   {
      value = other.value;
      error = other.error;
      return *this;
   }

   const Result<T> &
   WithErrorHandler(std::function<void(Ref<Error>)> handler)
   {
      if (error) handler(error);

      return *this;
   }
};
