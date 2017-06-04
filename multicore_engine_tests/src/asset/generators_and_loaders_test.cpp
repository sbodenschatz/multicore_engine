/*
 * Multi-Core Engine project
 * File /multicore_engine_tests/src/asset/generators_and_loaders_test.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <algorithm>
#include <mce/asset/asset_manager.hpp>
#include <mce/asset/file_asset_loader.hpp>
#include <mce/asset/load_unit_asset_loader.hpp>
#include <mce/asset/native_file_reader.hpp>
#include <mce/asset/pack_file_reader.hpp>
#include <mce/asset_gen/load_unit_gen.hpp>
#include <mce/asset_gen/pack_file_gen.hpp>
#include <boost/filesystem.hpp>
#include <cstring>
#include <fstream>
#include <future>
#include <gtest.hpp>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <thread>
#include <mce/util/finally.hpp>
#include <vector>

namespace fs = boost::filesystem;

namespace mce {
namespace asset {

struct assets_generators_and_loaders_test : public ::testing::Test {
	struct test_file {
		std::string name;
		std::vector<char> data;
		template <typename F>
		test_file(const std::string& name, F f) : name(name) {
			std::fstream out(name, std::ios::binary | std::ios::trunc | std::ios::in | std::ios::out);
			EXPECT_TRUE(out);
			f(out);
			EXPECT_TRUE(out);
			auto len = out.tellp();
			data.resize(len);
			EXPECT_TRUE(data.size());
			out.seekg(0, std::ios::beg);
			out.read(data.data(), data.size());
			EXPECT_TRUE(out);
		}
		~test_file() {
			fs::remove(name);
		}
		bool check(const std::string& filename) const {
			std::ifstream in(filename, std::ios::binary | std::ios::in);
			EXPECT_TRUE(in);
			in.seekg(0, std::ios::end);
			auto size = in.tellg();
			in.seekg(0, std::ios::beg);
			std::vector<char> actual(size);
			in.read(actual.data(), actual.size());
			EXPECT_TRUE(in);
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

	assets_generators_and_loaders_test() {
		// cppcheck-suppress useInitializationList
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
	~assets_generators_and_loaders_test() {}
};

TEST_F(assets_generators_and_loaders_test, load_files_sync) {
	asset_manager m;
	auto loader = std::make_shared<file_asset_loader>(
			std::vector<path_prefix>({{std::make_unique<native_file_reader>(), "."}}));
	m.add_asset_loader(loader);
	auto a1 = m.load_asset_sync(file_a->name);
	ASSERT_TRUE(file_a->check(a1->data(), a1->size()));
	auto a2 = m.load_asset_sync(file_b->name);
	ASSERT_TRUE(file_b->check(a2->data(), a2->size()));
	auto a3 = m.load_asset_sync(file_c->name);
	ASSERT_TRUE(file_c->check(a3->data(), a3->size()));
	auto a4 = m.load_asset_sync(file_d->name);
	ASSERT_TRUE(file_d->check(a4->data(), a4->size()));
}

TEST_F(assets_generators_and_loaders_test, load_files_future) {
	asset_manager m;
	auto loader = std::make_shared<file_asset_loader>(
			std::vector<path_prefix>({{std::make_unique<native_file_reader>(), "."}}));
	m.add_asset_loader(loader);
	auto f1 = m.load_asset_future(file_a->name);
	auto f2 = m.load_asset_future(file_b->name);
	auto f3 = m.load_asset_future(file_c->name);
	auto f4 = m.load_asset_future(file_d->name);
	auto a1 = f1.get();
	auto a2 = f2.get();
	auto a3 = f3.get();
	auto a4 = f4.get();
	ASSERT_TRUE(file_a->check(a1->data(), a1->size()));
	ASSERT_TRUE(file_b->check(a2->data(), a2->size()));
	ASSERT_TRUE(file_c->check(a3->data(), a3->size()));
	ASSERT_TRUE(file_d->check(a4->data(), a4->size()));
}

TEST_F(assets_generators_and_loaders_test, load_files_async) {
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
	auto a1 = m.load_asset_async(
			file_a->name, [&p1, this](const auto& a) { p1.set_value(file_a->check(a->data(), a->size())); });
	auto a2 = m.load_asset_async(
			file_b->name, [&p2, this](const auto& a) { p2.set_value(file_b->check(a->data(), a->size())); });
	auto a3 = m.load_asset_async(
			file_c->name, [&p3, this](const auto& a) { p3.set_value(file_c->check(a->data(), a->size())); });
	auto a4 = m.load_asset_async(
			file_d->name, [&p4, this](const auto& a) { p4.set_value(file_d->check(a->data(), a->size())); });
	ASSERT_TRUE(f1.get());
	ASSERT_TRUE(f2.get());
	ASSERT_TRUE(f3.get());
	ASSERT_TRUE(f4.get());
}

TEST_F(assets_generators_and_loaders_test, gen_and_load_load_unit_sync) {
	mce::asset_gen::load_unit_gen gen;
	gen.add_file(file_a->name, "file_a");
	gen.add_file(file_b->name, "file_b");
	gen.add_file(file_c->name, "file_c");
	gen.add_file(file_d->name, "file_d");
	auto f = util::finally([]() {
		fs::remove("test.lum");
		fs::remove("test.lup");
	});
	gen.compile_load_unit("test.lum", "test.lup");
	asset_manager m;
	auto loader = std::make_shared<load_unit_asset_loader>(
			std::vector<path_prefix>({{std::make_unique<native_file_reader>(), "."}}));
	m.add_asset_loader(loader);
	m.start_pin_load_unit("test");
	auto a1 = m.load_asset_sync("file_a");
	ASSERT_TRUE(file_a->check(a1->data(), a1->size()));
	auto a2 = m.load_asset_sync("file_b");
	ASSERT_TRUE(file_b->check(a2->data(), a2->size()));
	auto a3 = m.load_asset_sync("file_c");
	ASSERT_TRUE(file_c->check(a3->data(), a3->size()));
	auto a4 = m.load_asset_sync("file_d");
	ASSERT_TRUE(file_d->check(a4->data(), a4->size()));
}
TEST_F(assets_generators_and_loaders_test, gen_and_load_load_unit_future) {
	mce::asset_gen::load_unit_gen gen;
	gen.add_file(file_a->name, "file_a");
	gen.add_file(file_b->name, "file_b");
	gen.add_file(file_c->name, "file_c");
	gen.add_file(file_d->name, "file_d");
	auto f = util::finally([]() {
		fs::remove("test.lum");
		fs::remove("test.lup");
	});
	gen.compile_load_unit("test.lum", "test.lup");
	asset_manager m;
	auto loader = std::make_shared<load_unit_asset_loader>(
			std::vector<path_prefix>({{std::make_unique<native_file_reader>(), "."}}));
	m.add_asset_loader(loader);
	m.start_pin_load_unit("test");
	auto f1 = m.load_asset_future("file_a");
	auto f2 = m.load_asset_future("file_b");
	auto f3 = m.load_asset_future("file_c");
	auto f4 = m.load_asset_future("file_d");
	auto a1 = f1.get();
	auto a2 = f2.get();
	auto a3 = f3.get();
	auto a4 = f4.get();
	ASSERT_TRUE(file_a->check(a1->data(), a1->size()));
	ASSERT_TRUE(file_b->check(a2->data(), a2->size()));
	ASSERT_TRUE(file_c->check(a3->data(), a3->size()));
	ASSERT_TRUE(file_d->check(a4->data(), a4->size()));
}
TEST_F(assets_generators_and_loaders_test, gen_and_load_load_unit_async) {
	mce::asset_gen::load_unit_gen gen;
	gen.add_file(file_a->name, "file_a");
	gen.add_file(file_b->name, "file_b");
	gen.add_file(file_c->name, "file_c");
	gen.add_file(file_d->name, "file_d");
	auto f = util::finally([]() {
		fs::remove("test.lum");
		fs::remove("test.lup");
	});
	gen.compile_load_unit("test.lum", "test.lup");
	asset_manager m;
	auto loader = std::make_shared<load_unit_asset_loader>(
			std::vector<path_prefix>({{std::make_unique<native_file_reader>(), "."}}));
	m.add_asset_loader(loader);
	m.start_pin_load_unit("test");
	std::promise<bool> p1;
	std::promise<bool> p2;
	std::promise<bool> p3;
	std::promise<bool> p4;
	auto f1 = p1.get_future();
	auto f2 = p2.get_future();
	auto f3 = p3.get_future();
	auto f4 = p4.get_future();
	auto a1 = m.load_asset_async(
			"file_a", [&p1, this](const auto& a) { p1.set_value(file_a->check(a->data(), a->size())); });
	auto a2 = m.load_asset_async(
			"file_b", [&p2, this](const auto& a) { p2.set_value(file_b->check(a->data(), a->size())); });
	auto a3 = m.load_asset_async(
			"file_c", [&p3, this](const auto& a) { p3.set_value(file_c->check(a->data(), a->size())); });
	auto a4 = m.load_asset_async(
			"file_d", [&p4, this](const auto& a) { p4.set_value(file_d->check(a->data(), a->size())); });
	ASSERT_TRUE(f1.get());
	ASSERT_TRUE(f2.get());
	ASSERT_TRUE(f3.get());
	ASSERT_TRUE(f4.get());
}

TEST_F(assets_generators_and_loaders_test, gen_and_load_pack_file_sync) {
	mce::asset_gen::pack_file_gen gen;
	gen.add_file(file_a->name, "file_a");
	gen.add_file(file_b->name, "file_b");
	gen.add_file(file_c->name, "file_c");
	gen.add_file(file_d->name, "file_d");
	auto f = util::finally([]() { fs::remove("test.pack"); });
	gen.compile_pack_file("test.pack");
	asset_manager m;
	auto loader = std::make_shared<file_asset_loader>(
			std::vector<path_prefix>({{std::make_unique<pack_file_reader>(), "test.pack"}}));
	m.add_asset_loader(loader);
	auto a1 = m.load_asset_sync("file_a");
	ASSERT_TRUE(file_a->check(a1->data(), a1->size()));
	auto a2 = m.load_asset_sync("file_b");
	ASSERT_TRUE(file_b->check(a2->data(), a2->size()));
	auto a3 = m.load_asset_sync("file_c");
	ASSERT_TRUE(file_c->check(a3->data(), a3->size()));
	auto a4 = m.load_asset_sync("file_d");
	ASSERT_TRUE(file_d->check(a4->data(), a4->size()));
}

TEST_F(assets_generators_and_loaders_test, gen_and_load_pack_file_future) {
	mce::asset_gen::pack_file_gen gen;
	gen.add_file(file_a->name, "file_a");
	gen.add_file(file_b->name, "file_b");
	gen.add_file(file_c->name, "file_c");
	gen.add_file(file_d->name, "file_d");
	auto f = util::finally([]() { fs::remove("test.pack"); });
	gen.compile_pack_file("test.pack");
	asset_manager m;
	auto loader = std::make_shared<file_asset_loader>(
			std::vector<path_prefix>({{std::make_unique<pack_file_reader>(), "test.pack"}}));
	m.add_asset_loader(loader);
	auto f1 = m.load_asset_future("file_a");
	auto f2 = m.load_asset_future("file_b");
	auto f3 = m.load_asset_future("file_c");
	auto f4 = m.load_asset_future("file_d");
	auto a1 = f1.get();
	auto a2 = f2.get();
	auto a3 = f3.get();
	auto a4 = f4.get();
	ASSERT_TRUE(file_a->check(a1->data(), a1->size()));
	ASSERT_TRUE(file_b->check(a2->data(), a2->size()));
	ASSERT_TRUE(file_c->check(a3->data(), a3->size()));
	ASSERT_TRUE(file_d->check(a4->data(), a4->size()));
}
TEST_F(assets_generators_and_loaders_test, gen_and_load_pack_file_async) {
	mce::asset_gen::pack_file_gen gen;
	gen.add_file(file_a->name, "file_a");
	gen.add_file(file_b->name, "file_b");
	gen.add_file(file_c->name, "file_c");
	gen.add_file(file_d->name, "file_d");
	auto f = util::finally([]() { fs::remove("test.pack"); });
	gen.compile_pack_file("test.pack");
	asset_manager m;
	auto loader = std::make_shared<file_asset_loader>(
			std::vector<path_prefix>({{std::make_unique<pack_file_reader>(), "test.pack"}}));
	m.add_asset_loader(loader);
	std::promise<bool> p1;
	std::promise<bool> p2;
	std::promise<bool> p3;
	std::promise<bool> p4;
	auto f1 = p1.get_future();
	auto f2 = p2.get_future();
	auto f3 = p3.get_future();
	auto f4 = p4.get_future();
	auto a1 = m.load_asset_async(
			"file_a", [&p1, this](const auto& a) { p1.set_value(file_a->check(a->data(), a->size())); });
	auto a2 = m.load_asset_async(
			"file_b", [&p2, this](const auto& a) { p2.set_value(file_b->check(a->data(), a->size())); });
	auto a3 = m.load_asset_async(
			"file_c", [&p3, this](const auto& a) { p3.set_value(file_c->check(a->data(), a->size())); });
	auto a4 = m.load_asset_async(
			"file_d", [&p4, this](const auto& a) { p4.set_value(file_d->check(a->data(), a->size())); });
	ASSERT_TRUE(f1.get());
	ASSERT_TRUE(f2.get());
	ASSERT_TRUE(f3.get());
	ASSERT_TRUE(f4.get());
}

TEST_F(assets_generators_and_loaders_test, gen_and_load_pack_file_sync_compressed) {
	mce::asset_gen::pack_file_gen gen;
	gen.add_file_compressed(file_a->name, "file_a");
	gen.add_file_compressed(file_b->name, "file_b", 1);
	gen.add_file_compressed(file_c->name, "file_c", 5);
	gen.add_file_compressed(file_d->name, "file_d", 9);
	auto f = util::finally([]() { fs::remove("test.pack"); });
	gen.compile_pack_file("test.pack");
	asset_manager m;
	auto loader = std::make_shared<file_asset_loader>(
			std::vector<path_prefix>({{std::make_unique<pack_file_reader>(), "test.pack"}}));
	m.add_asset_loader(loader);
	auto a1 = m.load_asset_sync("file_a");
	ASSERT_TRUE(file_a->check(a1->data(), a1->size()));
	auto a2 = m.load_asset_sync("file_b");
	ASSERT_TRUE(file_b->check(a2->data(), a2->size()));
	auto a3 = m.load_asset_sync("file_c");
	ASSERT_TRUE(file_c->check(a3->data(), a3->size()));
	auto a4 = m.load_asset_sync("file_d");
	ASSERT_TRUE(file_d->check(a4->data(), a4->size()));
}

TEST_F(assets_generators_and_loaders_test, gen_and_load_pack_file_future_compressed) {
	mce::asset_gen::pack_file_gen gen;
	gen.add_file_compressed(file_a->name, "file_a");
	gen.add_file_compressed(file_b->name, "file_b", 1);
	gen.add_file_compressed(file_c->name, "file_c", 5);
	gen.add_file_compressed(file_d->name, "file_d", 9);
	auto f = util::finally([]() { fs::remove("test.pack"); });
	gen.compile_pack_file("test.pack");
	asset_manager m;
	auto loader = std::make_shared<file_asset_loader>(
			std::vector<path_prefix>({{std::make_unique<pack_file_reader>(), "test.pack"}}));
	m.add_asset_loader(loader);
	auto f1 = m.load_asset_future("file_a");
	auto f2 = m.load_asset_future("file_b");
	auto f3 = m.load_asset_future("file_c");
	auto f4 = m.load_asset_future("file_d");
	auto a1 = f1.get();
	auto a2 = f2.get();
	auto a3 = f3.get();
	auto a4 = f4.get();
	ASSERT_TRUE(file_a->check(a1->data(), a1->size()));
	ASSERT_TRUE(file_b->check(a2->data(), a2->size()));
	ASSERT_TRUE(file_c->check(a3->data(), a3->size()));
	ASSERT_TRUE(file_d->check(a4->data(), a4->size()));
}
TEST_F(assets_generators_and_loaders_test, gen_and_load_pack_file_async_compressed) {
	mce::asset_gen::pack_file_gen gen;
	gen.add_file_compressed(file_a->name, "file_a");
	gen.add_file_compressed(file_b->name, "file_b", 1);
	gen.add_file_compressed(file_c->name, "file_c", 5);
	gen.add_file_compressed(file_d->name, "file_d", 9);
	auto f = util::finally([]() { fs::remove("test.pack"); });
	gen.compile_pack_file("test.pack");
	asset_manager m;
	auto loader = std::make_shared<file_asset_loader>(
			std::vector<path_prefix>({{std::make_unique<pack_file_reader>(), "test.pack"}}));
	m.add_asset_loader(loader);
	std::promise<bool> p1;
	std::promise<bool> p2;
	std::promise<bool> p3;
	std::promise<bool> p4;
	auto f1 = p1.get_future();
	auto f2 = p2.get_future();
	auto f3 = p3.get_future();
	auto f4 = p4.get_future();
	auto a1 = m.load_asset_async(
			"file_a", [&p1, this](const auto& a) { p1.set_value(file_a->check(a->data(), a->size())); });
	auto a2 = m.load_asset_async(
			"file_b", [&p2, this](const auto& a) { p2.set_value(file_b->check(a->data(), a->size())); });
	auto a3 = m.load_asset_async(
			"file_c", [&p3, this](const auto& a) { p3.set_value(file_c->check(a->data(), a->size())); });
	auto a4 = m.load_asset_async(
			"file_d", [&p4, this](const auto& a) { p4.set_value(file_d->check(a->data(), a->size())); });
	ASSERT_TRUE(f1.get());
	ASSERT_TRUE(f2.get());
	ASSERT_TRUE(f3.get());
	ASSERT_TRUE(f4.get());
}

TEST_F(assets_generators_and_loaders_test, gen_and_load_pack_file_and_load_unit_sync) {
	mce::asset_gen::load_unit_gen gen;
	gen.add_file(file_a->name, "file_a");
	gen.add_file(file_b->name, "file_b");
	gen.add_file(file_c->name, "file_c");
	gen.add_file(file_d->name, "file_d");
	auto f = util::finally([]() {
		fs::remove("test.lum");
		fs::remove("test.lup");
		fs::remove("test.pack");
	});
	gen.compile_load_unit("test.lum", "test.lup");
	mce::asset_gen::pack_file_gen gen2;
	gen2.add_file("test.lum", "test_lu.lum");
	gen2.add_file("test.lup", "test_lu.lup");
	gen2.compile_pack_file("test.pack");
	asset_manager m;
	auto loader = std::make_shared<load_unit_asset_loader>(
			std::vector<path_prefix>({{std::make_unique<pack_file_reader>(), "test.pack"}}));
	m.add_asset_loader(loader);
	m.start_pin_load_unit("test_lu");
	auto a1 = m.load_asset_sync("file_a");
	ASSERT_TRUE(file_a->check(a1->data(), a1->size()));
	auto a2 = m.load_asset_sync("file_b");
	ASSERT_TRUE(file_b->check(a2->data(), a2->size()));
	auto a3 = m.load_asset_sync("file_c");
	ASSERT_TRUE(file_c->check(a3->data(), a3->size()));
	auto a4 = m.load_asset_sync("file_d");
	ASSERT_TRUE(file_d->check(a4->data(), a4->size()));
}
TEST_F(assets_generators_and_loaders_test, gen_and_load_pack_file_and_load_unit_future) {
	mce::asset_gen::load_unit_gen gen;
	gen.add_file(file_a->name, "file_a");
	gen.add_file(file_b->name, "file_b");
	gen.add_file(file_c->name, "file_c");
	gen.add_file(file_d->name, "file_d");
	auto f = util::finally([]() {
		fs::remove("test.lum");
		fs::remove("test.lup");
		fs::remove("test.pack");
	});
	gen.compile_load_unit("test.lum", "test.lup");
	mce::asset_gen::pack_file_gen gen2;
	gen2.add_file("test.lum", "test_lu.lum");
	gen2.add_file("test.lup", "test_lu.lup");
	gen2.compile_pack_file("test.pack");
	asset_manager m;
	auto loader = std::make_shared<load_unit_asset_loader>(
			std::vector<path_prefix>({{std::make_unique<pack_file_reader>(), "test.pack"}}));
	m.add_asset_loader(loader);
	m.start_pin_load_unit("test_lu");
	auto f1 = m.load_asset_future("file_a");
	auto f2 = m.load_asset_future("file_b");
	auto f3 = m.load_asset_future("file_c");
	auto f4 = m.load_asset_future("file_d");
	auto a1 = f1.get();
	auto a2 = f2.get();
	auto a3 = f3.get();
	auto a4 = f4.get();
	ASSERT_TRUE(file_a->check(a1->data(), a1->size()));
	ASSERT_TRUE(file_b->check(a2->data(), a2->size()));
	ASSERT_TRUE(file_c->check(a3->data(), a3->size()));
	ASSERT_TRUE(file_d->check(a4->data(), a4->size()));
}
TEST_F(assets_generators_and_loaders_test, gen_and_load_pack_file_and_load_unit_async) {
	mce::asset_gen::load_unit_gen gen;
	gen.add_file(file_a->name, "file_a");
	gen.add_file(file_b->name, "file_b");
	gen.add_file(file_c->name, "file_c");
	gen.add_file(file_d->name, "file_d");
	auto f = util::finally([]() {
		fs::remove("test.lum");
		fs::remove("test.lup");
		fs::remove("test.pack");
	});
	gen.compile_load_unit("test.lum", "test.lup");
	mce::asset_gen::pack_file_gen gen2;
	gen2.add_file("test.lum", "test_lu.lum");
	gen2.add_file("test.lup", "test_lu.lup");
	gen2.compile_pack_file("test.pack");
	asset_manager m;
	auto loader = std::make_shared<load_unit_asset_loader>(
			std::vector<path_prefix>({{std::make_unique<pack_file_reader>(), "test.pack"}}));
	m.add_asset_loader(loader);
	m.start_pin_load_unit("test_lu");
	std::promise<bool> p1;
	std::promise<bool> p2;
	std::promise<bool> p3;
	std::promise<bool> p4;
	auto f1 = p1.get_future();
	auto f2 = p2.get_future();
	auto f3 = p3.get_future();
	auto f4 = p4.get_future();
	auto a1 = m.load_asset_async(
			"file_a", [&p1, this](const auto& a) { p1.set_value(file_a->check(a->data(), a->size())); });
	auto a2 = m.load_asset_async(
			"file_b", [&p2, this](const auto& a) { p2.set_value(file_b->check(a->data(), a->size())); });
	auto a3 = m.load_asset_async(
			"file_c", [&p3, this](const auto& a) { p3.set_value(file_c->check(a->data(), a->size())); });
	auto a4 = m.load_asset_async(
			"file_d", [&p4, this](const auto& a) { p4.set_value(file_d->check(a->data(), a->size())); });
	ASSERT_TRUE(f1.get());
	ASSERT_TRUE(f2.get());
	ASSERT_TRUE(f3.get());
	ASSERT_TRUE(f4.get());
}

TEST_F(assets_generators_and_loaders_test, gen_and_load_pack_file_and_load_unit_async_mt) {
	mce::asset_gen::load_unit_gen gen;
	gen.add_file(file_a->name, "file_a");
	gen.add_file(file_b->name, "file_b");
	gen.add_file(file_c->name, "file_c");
	gen.add_file(file_d->name, "file_d");
	auto f = util::finally([]() {
		fs::remove("test.lum");
		fs::remove("test.lup");
		fs::remove("test.pack");
	});
	gen.compile_load_unit("test.lum", "test.lup");
	mce::asset_gen::pack_file_gen gen2;
	gen2.add_file("test.lum", "test_lu.lum");
	gen2.add_file("test.lup", "test_lu.lup");
	gen2.compile_pack_file("test.pack");

	asset_manager m;
	auto loader = std::make_shared<load_unit_asset_loader>(
			std::vector<path_prefix>({{std::make_unique<pack_file_reader>(), "test.pack"}}));
	m.add_asset_loader(loader);
	m.start_pin_load_unit("test_lu");
	const int thread_count = 32;
	std::vector<std::future<bool>> futures;
	test_file* files[] = {file_a.get(), file_b.get(), file_c.get(), file_d.get()};
	std::string file_names[] = {"file_a", "file_b", "file_c", "file_d"};
	for(int i = 0; i < thread_count; ++i) {
		futures.emplace_back(std::async([&m, &files, &file_names]() {
			const int loads = 8;
			std::promise<bool> promises[loads];
			std::shared_ptr<const asset> assets[loads];
			for(int j = 0; j < loads; ++j) {
				assets[j] = m.load_asset_async(file_names[j % 4], [j, &files, &promises](const auto& a) {
					promises[j].set_value(files[j % 4]->check(a->data(), a->size()));
				});
			}
			return std::all_of(std::begin(promises), std::end(promises),
							   [](auto& p) { return p.get_future().get(); });
		}));
	}
	for(auto& future : futures) {
		ASSERT_TRUE(future.get());
	}
}

} /* namespace asset */
} /* namespace mce */
