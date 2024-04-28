#include <memory>

#include <Mocker/Core/Error.hpp>
#include <Mocker/Core/Helpers.hpp>

template<typename T>
struct Result
{
	std::shared_ptr<T> value;
	Error				 *error;

	Result();
	Result(Error *error);

	Result(T *value);
	Result(T *value, Error *error);

	Result(std::shared_ptr<T> value);
	Result(std::shared_ptr<T> value, Error *error);

	Result(const Result<T> &other);
	Result(Result<T> &&other) = delete;

	~Result() = default;

	operator bool() const;

	T *
	operator->() const;

	Result<T> &
	operator=(const Result<T> &other);
};
