/*
 * Multi-Core Engine project
 * File /multicore_engine_core_tests/src/asset/generators_and_loaders_test.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <asset/file_asset_loader.hpp>
#include <asset/native_file_reader.hpp>
#include <boost/test/unit_test.hpp>
#include <string>
#include <fstream>
#include <boost/filesystem.hpp>
#include <vector>
#include <memory>
#include <cstring>
#include <random>
#include <algorithm>
#include <thread>
#include <future>

namespace fs = boost::filesystem;

namespace mce {
namespace asset {

struct asset_gen_and_load_test_fixture {
	struct test_file {
		std::string name;
		std::vector<char> data;
		template <typename F>
		test_file(const std::string& name, F f)
				: name(name) {
			std::fstream out(name, std::ios::binary | std::ios::trunc | std::ios::in | std::ios::out);
			BOOST_CHECK(out);
			f(out);
			BOOST_CHECK(out);
			auto len = out.tellp();
			data.resize(len);
			BOOST_CHECK(data.size());
			out.seekg(0, std::ios::beg);
			out.read(data.data(), data.size());
			BOOST_CHECK(out);
		}
		~test_file() {
			fs::remove(name);
		}
		bool check(const std::string& filename) const {
			std::ifstream in(filename, std::ios::binary | std::ios::in);
			BOOST_CHECK(in);
			in.seekg(0, std::ios::end);
			auto size = in.tellg();
			in.seekg(0, std::ios::beg);
			std::vector<char> actual(size);
			in.read(actual.data(), actual.size());
			BOOST_CHECK(in);
			return actual == data;
		}
		bool check(const char* actual_data, size_t actual_size) {
			return std::equal(data.begin(), data.end(), actual_data, actual_data + actual_size);
		}
	};
	static void print(std::ostream& str, const std::string& text) {
		str.write(text.data(), text.size());
	}

	std::unique_ptr<test_file> file_a;
	std::unique_ptr<test_file> file_b;
	std::unique_ptr<test_file> file_c;
	std::unique_ptr<test_file> file_d;

	asset_gen_and_load_test_fixture() {
		file_a = std::make_unique<test_file>("bin_sequence.test", [](std::ostream& str) {
			for(size_t i = 0; i < 0x10000; ++i) {
				char tmp[sizeof(size_t)];
				std::memcpy(tmp, &i, sizeof(size_t));
				str.write(tmp, sizeof(size_t));
			}
		});
		file_b = std::make_unique<test_file>("txt_sequence.test", [](std::ostream& str) {
			for(size_t i = 0; i < 0x10000; ++i) {
				std::string temp = std::to_string(i) + "\n";
				print(str, temp);
			}
		});
		file_c = std::make_unique<test_file>("strings.test", [](std::ostream& str) {
			print(str, "This is a test.");
			print(str, "Hello World!!");
			print(str, "Foo Bar 12345");
			print(str, "Answer: 42");
		});
		file_d = std::make_unique<test_file>("pseudorandom.test", [](std::ostream& str) {
			std::mt19937_64 eng(42);
			for(size_t i = 0; i < 0x10000; ++i) {
				auto val = eng();
				char tmp[sizeof(val)];
				std::memcpy(tmp, &val, sizeof(val));
				str.write(tmp, sizeof(val));
			}
		});
	}
	~asset_gen_and_load_test_fixture() {}
};

BOOST_AUTO_TEST_SUITE(assets)
BOOST_FIXTURE_TEST_SUITE(generators_and_loaders, asset_gen_and_load_test_fixture)

BOOST_AUTO_TEST_CASE(load_files_sync) {
	asset_manager m;
	auto loader = std::make_shared<file_asset_loader>(
			std::vector<path_prefix>({{std::make_unique<native_file_reader>(), "."}}));
	m.add_asset_loader(loader);
	auto a1 = m.load_asset_sync("bin_sequence.test");
	BOOST_CHECK(file_a->check(a1->data(), a1->size()));
	auto a2 = m.load_asset_sync("txt_sequence.test");
	BOOST_CHECK(file_b->check(a2->data(), a2->size()));
	auto a3 = m.load_asset_sync("strings.test");
	BOOST_CHECK(file_c->check(a3->data(), a3->size()));
	auto a4 = m.load_asset_sync("pseudorandom.test");
	BOOST_CHECK(file_d->check(a4->data(), a4->size()));
}

BOOST_AUTO_TEST_CASE(load_files_future) {
	asset_manager m;
	auto loader = std::make_shared<file_asset_loader>(
			std::vector<path_prefix>({{std::make_unique<native_file_reader>(), "."}}));
	m.add_asset_loader(loader);
	auto f1 = m.load_asset_future("bin_sequence.test");
	auto f2 = m.load_asset_future("txt_sequence.test");
	auto f3 = m.load_asset_future("strings.test");
	auto f4 = m.load_asset_future("pseudorandom.test");
	auto a1 = f1.get();
	auto a2 = f2.get();
	auto a3 = f3.get();
	auto a4 = f4.get();
	BOOST_CHECK(file_a->check(a1->data(), a1->size()));
	BOOST_CHECK(file_b->check(a2->data(), a2->size()));
	BOOST_CHECK(file_c->check(a3->data(), a3->size()));
	BOOST_CHECK(file_d->check(a4->data(), a4->size()));
}

BOOST_AUTO_TEST_CASE(load_files_async) {
	asset_manager m;
	auto loader = std::make_shared<file_asset_loader>(
			std::vector<path_prefix>({{std::make_unique<native_file_reader>(), "."}}));
	m.add_asset_loader(loader);
	std::promise<bool> p1;
	std::promise<bool> p2;
	std::promise<bool> p3;
	std::promise<bool> p4;
	auto f1 = p1.get_future();
	auto f2 = p2.get_future();
	auto f3 = p3.get_future();
	auto f4 = p4.get_future();
	auto a1 = m.load_asset_async("bin_sequence.test", [&p1, this](const auto& a) {
		p1.set_value(file_a->check(a->data(), a->size()));
	});
	auto a2 = m.load_asset_async("txt_sequence.test", [&p2, this](const auto& a) {
		p2.set_value(file_b->check(a->data(), a->size()));
	});
	auto a3 = m.load_asset_async("strings.test", [&p3, this](const auto& a) {
		p3.set_value(file_c->check(a->data(), a->size()));
	});
	auto a4 = m.load_asset_async("pseudorandom.test", [&p4, this](const auto& a) {
		p4.set_value(file_d->check(a->data(), a->size()));
	});
	BOOST_CHECK(f1.get());
	BOOST_CHECK(f2.get());
	BOOST_CHECK(f3.get());
	BOOST_CHECK(f4.get());
}

BOOST_AUTO_TEST_CASE(gen_and_load_load_unit) {
	BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

} /* namespace asset */
} /* namespace mce */
