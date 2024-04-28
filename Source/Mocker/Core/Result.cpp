#include "Result.hpp"

//-----------------------------------------------------------------------------

template<typename T>
Result<T>::Result() : value(nullptr), error(nullptr)
{
}

//-----------------------------------------------------------------------------

template<typename T>
Result<T>::Result(Error *error) : value(nullptr), error(error)
{
}

//-----------------------------------------------------------------------------

template<typename T>
Result<T>::Result(T *value) : value(std::make_shared<T>(value)), error(nullptr)
{
}

//-----------------------------------------------------------------------------

template<typename T>
Result<T>::Result(T *value, Error *error)
	 : value(std::make_shared<T>(value)), error(error)
{
}

//-----------------------------------------------------------------------------

template<typename T>
Result<T>::Result(std::shared_ptr<T> value) : value(value), error(nullptr)
{
}

//-----------------------------------------------------------------------------

template<typename T>
Result<T>::Result(std::shared_ptr<T> value, Error *error)
	 : value(value), error(error)
{
}

//-----------------------------------------------------------------------------

template<typename T>
Result<T>::Result(const Result<T> &other)
	 : value(other.value), error(other.error)
{
}

//-----------------------------------------------------------------------------

template<typename T>
Result<T>::operator bool() const
{
	return !error;
}

//-----------------------------------------------------------------------------

template<typename T>
T *
Result<T>::operator->() const
{
	return value.get();
}

//-----------------------------------------------------------------------------

template<typename T>
Result<T> &
Result<T>::operator=(const Result<T> &other)
{
	value = other.value;
	error = other.error;
	return *this;
}

//-----------------------------------------------------------------------------
