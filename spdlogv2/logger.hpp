/*
 * SPDX-FileCopyrightText: 2024 Dénes Derhán <denes.derhan@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/null_sink.h>
#include <spdlog/async.h>

#include <stdexcept>
#include <string_view>
#include <string>

#include <logbench/force_inline.hpp>
#include <logbench/test_in_param.hpp>
#include <logbench/test_out_param.hpp>

static_assert(SPDLOG_VER_MAJOR == 2);

class logger {
public:
	logger(int id = 0) {
		std::string logger_name("logger_");
		logger_name += std::to_string(id);
		logger_instance_ = std::make_shared<spdlog::async_logger>(
			logger_name, sink_, spdlog::thread_pool(), spdlog::async_overflow_policy::block);
		spdlog::register_logger(logger_instance_);
	}
	~logger() {}

	template<typename... Args>
	LOGBENCH_FORCEINLINE void log_test1(Args &&... args) {
		logger_instance_->log(spdlog::source_loc{ __FILE__, __LINE__, SPDLOG_FUNCTION }, spdlog::level::info, "Thr: {} Log_n: {} Time: {} {} {}", args...);
		//SPDLOG_LOGGER_INFO(logger_instance_, "Thr: {} Log_n: {} Time: {} {} {}", args...);
	}

	static void set_log_template(std::string_view templ)  noexcept {
		log_templ_ = templ;
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

		if (init_data.lib_buffer_size != -1) {
			//FIX THIS adjust / 82 to desired Mb mem used! (define it in test??)
			spdlog::init_thread_pool((uint64_t(init_data.lib_buffer_size) * 1024  * init_data.thread_num) / 82, 1);
			out_data.lib_buffer_size = init_data.lib_buffer_size;
		}
		else {
			spdlog::init_thread_pool(spdlog::details::default_async_q_size, 1);
			out_data.lib_buffer_size = (spdlog::details::default_async_q_size * 82) / (size_t(1024) * init_data.thread_num);
		}

		if (init_data.buffer_flush_time != -1) {
			throw std::runtime_error("Can buffer polling be set? To do impl!");
		}
		else {
			out_data.buffer_flush_time = -1;
		}

		if (out_data.formatter_type != logbench::formatter_typ::txt) {
			throw std::runtime_error("Formatter type not supported!");
		}
		out_data.formater_impl = "fmtlib_{fmt}";

		std::string log_file{ init_data.log_path };
		log_file += "/log.txt";
		if (out_data.sink_type == logbench::sink_typ::lib) {
			out_data.sink_type = logbench::sink_typ::sort;
		}
		if (out_data.sink_type == logbench::sink_typ::null) {
			sink_ = std::make_shared<spdlog::sinks::null_sink_mt>();
		}
		else if (out_data.sink_type == logbench::sink_typ::sort) {
			sink_ = std::make_shared<spdlog::sinks::basic_file_sink_st>(log_file, true);
			sink_->set_pattern(log_templ_);
		}
		else {
			throw std::runtime_error("Sink type not supported!");
		}

		if (init_data.sink_flush_time != -1) {
			if (init_data.sink_flush_time != 0) {
				// warning: only use if all your loggers are thread-safe ("_mt" loggers)
				//spdlog::flush_every(std::chrono::milliseconds(init_data.sink_flush_time));
				throw std::runtime_error("Setting flush interval not supported!");
			}
			out_data.sink_flush_time = init_data.sink_flush_time;
		}
		else {
			out_data.sink_flush_time = 0;
		}
		
	}

	static void sys_stop() {
		spdlog::shutdown();
	}

	static constexpr std::string_view lib_name() noexcept {
		return "spdlogv2";
	}

	static std::string_view lib_version() {
		return spdlog_version_;
	}

	inline static std::string log_templ_{"%Y-%m-%d %H:%M:%S.%e %v"};
	inline static uint32_t logger_buffer_size_{ 0 };
	inline static spdlog::sink_ptr sink_;
	std::shared_ptr<spdlog::logger> logger_instance_;
	inline static const std::string spdlog_version_{ 
		std::to_string(SPDLOG_VER_MAJOR)
		+ "." + std::to_string(SPDLOG_VER_MINOR)
		+ "." + std::to_string(SPDLOG_VER_PATCH) };
};
