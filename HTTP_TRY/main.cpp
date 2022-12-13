#include "httplib.h"
#include <array>
#include <type_traits>
#include <iterator>
#include <optional>
#include <algorithm>
#include <utility>

enum class array_type : uint8_t
{
	INPUT,
	OUTPUT
};

template<array_type ArrayType,typename T>
struct boundary_value
{
	T max_ = {};
	T min_ = {};

	array_type get_type()
	{
		return ArrayType;
	}
};


template<typename T>
std::string get_full_request(std::string_view _base_url,T _x,T _y)
{
	return (std::string(_base_url) + std::to_string(_x) + "&y=" + std::to_string(_y));
}

template<typename T>
auto rngGen(T _min, T _max)
{
	assert(_min < _max, "min must be lower than max!");
	[[likely]]if constexpr(std::is_floating_point_v<T>)
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
	static_assert(std::is_trivially_constructible_v<T>,"type T is not trivially constructible");
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
	boundary_value<array_type::INPUT, double> input_bounds;

	constexpr std::string_view dd = "https://epsg.io/transform#s_srs=4326&t_srs=102954&x=";
	constexpr size_t arr_size = 500;
	constexpr double min = 0.5;
	constexpr double max = 2.0;

	using value_type = std::remove_const_t<decltype(min)>;

	auto input_array = set_array<std::pair<value_type, value_type>, arr_size>();
	auto output_array = set_array<std::pair<value_type, value_type>,arr_size>();

	initialize_input_array(input_array, min, max);


	unsigned i = 0;
	std::cout << "while loop" << '\n';
	while (i < arr_size)
	{
		std::cout << rngGen(min, max) << '\n';
		i++;
	}

	return 0;
}
