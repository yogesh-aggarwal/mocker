#pragma once

#include <memory>

#include <Mocker/Core/Error.hpp>
#include <Mocker/Core/Helpers.hpp>

template<typename T>
struct Result
{
	T      value;
	Error *error;

	Result(T value) : value(value), error(nullptr) {}
	Result(T value, Error *error) : value(value), error(error) {}

	Result(const Result<T> &other) : value(other.value), error(other.error) {}
	Result(Result<T> &&other) = delete;

	~Result() = default;

	operator bool() const { return !error; }

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
};
