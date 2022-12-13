#include "httplib.h"
#include <array>
#include <type_traits>
#include <iterator>
#include <optional>
#include <algorithm>
#include <utility>

template<typename T>
using pos_init_function = std::function<T(T, T)>;

enum class array_type : uint8_t
{
	INPUT,
	OUTPUT
};

template<typename T>
auto rngGen(T _min, T _max)
{
	assert(_min < _max, "min must be lower than max!");
	[[likely]] if constexpr (std::is_floating_point_v<T>)
	{
		std::random_device dev;
		std::mt19937 rng(dev());
		std::uniform_real_distribution<T> dist6(_min, _max);

		return (dist6(rng));
	}
	else
	{
		std::random_device dev;
		std::mt19937 rng(dev());
		std::uniform_int_distribution<T> dist6(_min, _max);

		return (dist6(rng));
	}

}

namespace config
{
	template<typename T>
	struct boundary_values
	{
		static_assert(std::is_trivially_constructible_v<T>, "Type must be trivially constructible");
		T min_{};
		T max_{};
	};
	constexpr boundary_values x_bounds{ 0.2,2.0 };
	constexpr boundary_values y_bounds{ 0.5,1.5 };

	using value_type = std::remove_const_t<decltype(x_bounds.min_)>;

	pos_init_function<value_type> init_func = rngGen<value_type>;
}



template<typename T>
struct position
{
	position(std::function <T(T, T)> _init_func) : initialization_func_(_init_func)
	{
		x_ = initialization_func_(config::x_bounds.min_, config::x_bounds.max_);
		y_ = initialization_func_(config::y_bounds.min_, config::y_bounds.max_);
	}
	position() : initialization_func_(config::init_func)
	{
		x_ = initialization_func_(config::x_bounds.min_, config::x_bounds.max_);
		y_ = initialization_func_(config::y_bounds.min_, config::y_bounds.max_);
	}

	std::function<T(T, T)> initialization_func_;
	T x_ = {};
	T y_ = {};
};


template<typename T>
std::string get_full_request(std::string_view _base_url,T _x,T _y)
{
	return (std::string(_base_url) + std::to_string(_x) + "&y=" + std::to_string(_y));
}



template<typename Array,typename T>
void initialize_input_array(Array& _arr,T _min, T _max)
{
	for (auto& val : _arr)
	{
		val = rngGen(_min, _max);
	}
}

template<typename T,size_t size>
constexpr auto set_array()
{
	static_assert(std::is_standard_layout_v<T>,"type T is not trivially constructible");
	std::array<T, size> init_array = {};
	return init_array;
}

std::optional<std::string> get_html_body(std::string_view _request)
{
	try
	{
		httplib::Client cli(_request.data());
		if (cli.is_valid())
		{
			const auto res = cli.Get("/");
			if (res)
			{
				return res->body;
			}
			else
			{
				return std::nullopt;
			}
		}
		else
		{
			return std::nullopt;
		}
	}
	catch (std::exception& e)
	{
		return std::nullopt;
	}
}



std::string parse_html_body(std::string& _body_str)
{
	/*std::find(_body_str.begin(), _body_str.end(), [](char _c) 
		{

		});*/
	return "";
}

template<typename T,size_t array_size>
auto return_val_from_request(std::array<T, array_size>& _array) -> decltype(_array[0])
{

}


int main()
{

	constexpr std::string_view dd = "https://epsg.io/transform#s_srs=4326&t_srs=102954&x=";
	constexpr size_t arr_size = 500;
	constexpr double min = 0.5;
	constexpr double max = 2.0;

	

	auto input_array = set_array<position<config::value_type>, arr_size>();
	auto output_array = set_array<position<config::value_type>,arr_size>();

	//initialize_input_array(input_array, min, max);


	for (auto& val : input_array)
	{
		std::cout << '<' << val.x_ << "," << val.y_ << '>' << ' ' << '\n';
	}
	return 0;

	unsigned i = 0;
	std::cout << "while loop" << '\n';
	while (i < arr_size)
	{
		std::cout << rngGen(min, max) << '\n';
		i++;
	}

	return 0;
}
