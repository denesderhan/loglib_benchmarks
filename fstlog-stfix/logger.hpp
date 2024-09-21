/*
 * SPDX-FileCopyrightText: 2024 Dénes Derhán <denes.derhan@gmail.com>
 *
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */
#pragma once
#include <string_view>
#include <stdexcept>
#include <string>

#include <logbench/force_inline.hpp>
#include <logbench/test_in_param.hpp>
#include <logbench/test_out_param.hpp>

#include <fstlog/version.hpp>
#include <fstlog/core.hpp>
#include <fstlog/logger/log_macro.hpp>
#include <fstlog/logger/logger_st_fix.hpp>
#include <fstlog/formatter/formatter_txt.hpp>
#include <fstlog/output/output_file.hpp>
#include <fstlog/sink/sink_null.hpp>
#include <fstlog/sink/sink_sort.hpp>
#include <fstlog/sink/sink_unsort.hpp>

class logger {
public:
	logger(int id = 0) noexcept
		:logger_instance_{ 
			core_instance_,
			buffer_size_}
	{}
	~logger() noexcept = default;
	
	//FIX THIS use std::forward args?
	template<typename... Args>
	LOGBENCH_FORCEINLINE void log_test1(Args const& ...args) {
		LOG_INFO(logger_instance_, "Thr: {} Log_n: {} Time: {} {} {}", args...);
	}

	static void set_log_template_txt(std::string_view templ) noexcept {
		log_templ_ = templ;
	}
	static void set_log_template_bin(std::underlying_type_t<fstlog::log_metaargs> bin_flags) noexcept {
		log_templ_bin_ = bin_flags;
	}

	static void sys_init(
		logbench::test_in_param const& init_data,
		logbench::test_out_param& out_data)
	{
		out_data.lib_name = lib_name();
		out_data.lib_version = lib_version();
		out_data.formatter_type = init_data.formatter_type;
		out_data.sink_type = init_data.sink_type;
		out_data.garbage_lines = 0;
		
		if (init_data.buffer_flush_time != -1) {
			core_instance_.poll_interval(
				std::chrono::milliseconds(init_data.buffer_flush_time));
			out_data.buffer_flush_time = init_data.buffer_flush_time;
		}
		else {
			out_data.buffer_flush_time = static_cast<int>(core_instance_.poll_interval().count());
		}

		if (init_data.lib_buffer_size != -1) {
			out_data.lib_buffer_size = init_data.lib_buffer_size;
		}
		else {
			//FIX THIS method to get default buffer size??
			out_data.lib_buffer_size = 0;
		}
		buffer_size_ = std::uint32_t(out_data.lib_buffer_size) * 1024;
		
		if (init_data.sink_flush_time != -1) {
			out_data.sink_flush_time = init_data.sink_flush_time;
		}
		else {
			//FIX THIS (add method to get default flush time!)
			out_data.sink_flush_time = 0;
		}

		std::string log_file{ init_data.log_path };

		fstlog::formatter formatter;
		if (out_data.formatter_type == logbench::formatter_typ::txt) {
			out_data.formater_impl = "own_fill_align_charconv";
			formatter = fstlog::formatter_txt(log_templ_);
			log_file += std::string_view{"/log.txt"};
		}
		else {
			throw std::runtime_error("Formatter type not supported!");
		}

		fstlog::filter filter{ fstlog::level::All, 1};

		auto out_file = fstlog::output_file(log_file.c_str(), true);
		
		if (out_data.sink_type == logbench::sink_typ::lib) {
			out_data.sink_type = logbench::sink_typ::sort;
		}
		fstlog::sink sink;
		if (out_data.sink_type == logbench::sink_typ::sort) {
			sink = fstlog::sink_sort(
				formatter,
				out_file,
				filter,
				std::chrono::milliseconds(out_data.sink_flush_time)
			);
		}
		else if (out_data.sink_type == logbench::sink_typ::unsort) {
			sink = fstlog::sink_unsort(
				formatter,
				out_file,
				filter,
				std::chrono::milliseconds(out_data.sink_flush_time)
			);
		}
		else if (out_data.sink_type == logbench::sink_typ::null) {
			sink = fstlog::sink_null();
		}
		else {
			throw std::runtime_error("Sink type not supported!");
		}

		core_instance_.add_sink(sink);

		if (init_data.log_self) {
			std::string self_log_file{init_data.temp_path };
			self_log_file += "/background.log.txt";
			fstlog::filter filter_bck{ fstlog::level::All, 0 };
			core_instance_.add_sink(
				fstlog::sink_sort(
					fstlog::formatter_txt(),
					fstlog::output_file(self_log_file.c_str(), true),
					filter_bck,
					std::chrono::milliseconds(out_data.sink_flush_time)));
		}
	}

	static void sys_stop() noexcept {
		core_instance_.stop();
	}

	static constexpr std::string_view lib_name() noexcept {
		return "fstlog-logger-stfix";
	}

	static constexpr std::string_view lib_version() noexcept {
		return FSTLOG_VERSION;
	}
	
private:
	inline static std::uint32_t buffer_size_{ 0 };
	inline static std::underlying_type_t<fstlog::log_metaargs> log_templ_bin_{
		fstlog::ut_cast(fstlog::log_metaargs::All)};
	inline static std::string_view log_templ_{
		"{timestamp:.6%Y-%m-%d %H:%M:%S %z} {message}"};
	inline static fstlog::core core_instance_;
	fstlog::logger_st_fix < 
		fstlog::small_string<16>{"logger"}, 
		fstlog::level::Trace,
		1> logger_instance_;
};
