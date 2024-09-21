/*
 * SPDX-FileCopyrightText: 2024 Dénes Derhán <denes.derhan@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <string_view>

#include <logbench/force_inline.hpp>
#include <logbench/test_in_param.hpp>
#include <logbench/test_out_param.hpp>

class logger {
public:
	logger([[maybe_unused]] int id = 0) {}
	~logger() {}

	static void sys_init(
		logbench::test_in_param const & init_data,
		logbench::test_out_param& out_data) noexcept 
	{
		out_data.lib_name = lib_name();
		out_data.lib_version = lib_version();
		out_data.formatter_type = init_data.formatter_type;
		out_data.sink_type = init_data.sink_type;
		out_data.garbage_lines = 0;
		out_data.lib_buffer_size = 0;
		out_data.buffer_flush_time = 0;
		out_data.sink_flush_time = 0;
	}
	static void sys_stop() noexcept {}

	template<typename... Args>
	LOGBENCH_FORCEINLINE void log_test1([[maybe_unused]] Args &&... args) {}
	
	static constexpr std::string_view lib_name() noexcept {
		return "noop";
	}

	static constexpr std::string_view lib_version() noexcept {
		return "0.0.0";
	}
};
