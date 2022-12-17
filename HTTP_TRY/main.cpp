#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include <array>
#include <type_traits>
#include <iterator>
#include <optional>
#include <algorithm>
#include <utility>
#include <vector>

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
	constexpr size_t arr_size = 2000;

	template<typename T>
	struct boundary_values
	{
		static_assert(std::is_trivially_constructible_v<T>, "Type must be trivially constructible");
		T min_{};
		T max_{};
	};
	constexpr boundary_values x_bounds{ 88.01f,92.6698695f};
	constexpr boundary_values y_bounds{ 18.551604f,92.6698695f};

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
	std::vector<T> init_vector(size);
	return init_vector;
}

std::optional<std::string> get_html_body(std::string_view _base_url,std::string_view _request)
{
	std::cout << "REQUESTING:" << '\n';
	std::cout << _request.data() << '\n';
	try
	{
		httplib::Client cli(_base_url.data());
		if (cli.is_valid())
		{
			std::cout << "CLI is valid" << '\n';
			std::string req = std::string(_request) + "/transform";
			const auto res = cli.Get(req);
			if (res)
			{
				std::cout << res->get_header_value("Location");
				return res->body;
				
			}
			else
			{
				std::cout << res->status;
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
		std::cout << e.what() << '\n';
		return std::nullopt;
	}
}


template<typename T>
auto parse_html_body(std::string& _body_str) -> position<T>
{
	const std::string delimeter_str = "srs-out-map-link";

	auto begin_parse = std::search(_body_str.begin(),_body_str.end(), delimeter_str.begin(),delimeter_str.end());
	return position<T>();
}

template<typename T,size_t array_size>
auto return_val_from_request(std::array<T, array_size>& _array) -> decltype(_array[0])
{

}

int main()
{
	constexpr std::string_view base_url = "https://epsg.io";
	constexpr std::string_view get_url = "/transform#s_srs=4326&t_srs=102954&x=";
	
	constexpr float min = 0.5;
	constexpr float max = 2.0;

	auto input_array = set_array<position<config::value_type>, config::arr_size>();
	auto output_array = set_array<position<config::value_type>,config::arr_size>();

	//initialize_input_array(input_array, min, max);


	for (auto it = input_array.begin(); it != input_array.end(); it++)
	{
		auto request = get_full_request(get_url,  (*it).x_, (*it).y_);
		auto body = get_html_body(base_url,request);
		if (body.has_value())
		{
			std::cout << body.value() << '\n';
			auto parsed_lat_lon = parse_html_body<config::value_type>(body.value());
		}
		else
		{
			std::cout << "NO BODY FOUND" << '\n';
			it--;
		}
	}


	unsigned i = 0;
	std::cout << "while loop" << '\n';
	while (i < config::arr_size)
	{
		std::cout << rngGen(min, max) << '\n';
		i++;
	}

	return 0;
}
