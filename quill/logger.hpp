/*
 * SPDX-FileCopyrightText: 2024 Dénes Derhán <denes.derhan@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <quill/Logger.h>
#include <quill/Frontend.h>
#include <quill/Backend.h>
#include <quill/sinks/NullSink.h>
#include <quill/sinks/FileSink.h>
#include <quill/LogMacros.h>

#include <stdexcept>
#include <string_view>
#include <string>

#include <logbench/force_inline.hpp>
#include <logbench/test_in_param.hpp>
#include <logbench/test_out_param.hpp>

class logger {
public:
	logger(int id = 0) {}
	~logger() {}

	template<typename... Args>
	LOGBENCH_FORCEINLINE void log_test1(Args &&... args) {
		LOG_INFO(logger_instance_, "Thr: {} Log_n: {} Time: {} {} {}" , args...);
	}

	struct log_templ {
		std::string templ;
		std::string time_templ;
		quill::Timezone zone;
	};

	static void set_log_template(log_templ templ)  noexcept {
		log_templ_ = templ;
	}

	static void sys_init(
		logbench::test_in_param const& init_data,
		logbench::test_out_param& out_data)
	{
		quill::BackendOptions  cfg;
		
		out_data.lib_name = lib_name();
		out_data.lib_version = lib_version();
		out_data.formatter_type = init_data.formatter_type;
		out_data.sink_type = init_data.sink_type;
		out_data.garbage_lines = 0;

		if (init_data.lib_buffer_size != -1) {
			throw std::runtime_error("Setting buffer size is only effective when built with blocking queue!");
		}
		else {
			out_data.lib_buffer_size = -1;
		}

		if (init_data.buffer_flush_time != -1) {
			out_data.buffer_flush_time = init_data.buffer_flush_time;
			if (init_data.buffer_flush_time == 0) {
				cfg.sleep_duration = (std::chrono::nanoseconds::max)();
			}
			else {
				cfg.sleep_duration = std::chrono::milliseconds(init_data.buffer_flush_time);
			}
		}
		else {
			size_t sleep = std::chrono::duration_cast<std::chrono::milliseconds>( cfg.sleep_duration ).count();
			if (sleep == 0) sleep = 1;
			else if (sleep > (std::numeric_limits<int>::max)())
				sleep = (std::numeric_limits<int>::max)();
			out_data.buffer_flush_time = static_cast<int>(sleep);
		}

		if (init_data.sink_flush_time != -1) {
			throw std::runtime_error("Can sink flush time be set? To do impl!");
		}
				
		if (out_data.formatter_type != logbench::formatter_typ::txt) {
			throw std::runtime_error("Formatter type not supported!");
		}
		out_data.formater_impl = "fmtlib_{fmt}";

		quill::Backend::start(cfg);

		std::string log_file{ init_data.log_path };
		log_file += "/log.txt";
		if (out_data.sink_type == logbench::sink_typ::lib) {
			out_data.sink_type = logbench::sink_typ::sort;
		}
		auto sink = std::shared_ptr<quill::Sink>{};
		if (out_data.sink_type == logbench::sink_typ::null) {
			sink = quill::Frontend::create_or_get_sink<quill::NullSink>("sink_id_1");
		}
		else if (out_data.sink_type == logbench::sink_typ::sort) {
			sink = quill::Frontend::create_or_get_sink<quill::FileSink>(log_file);
		}
		else {
			throw std::runtime_error("Sink type not supported!");
		} 
		
		logger_instance_ = quill::Frontend::create_or_get_logger("logr", std::move(sink), quill::PatternFormatterOptions(log_templ_.templ, log_templ_.time_templ, log_templ_.zone));
	}

	static void sys_stop() noexcept {
		logger_instance_->flush_log();
		quill::Backend::stop();
	}

	static constexpr std::string_view lib_name() noexcept {
		return "quill";
	}

	static std::string_view lib_version() {
		return quill_version_;
	}

	inline static quill::Logger* logger_instance_;
	inline static log_templ log_templ_{"%(time) %(message)", "%Y-%m-%d %H:%M:%S,%Qms %z", quill::Timezone::LocalTime};
	inline static const std::string quill_version_{
		std::to_string(quill::VersionMajor)
		+ '.' + std::to_string(quill::VersionMinor)
		+ '.' + std::to_string(quill::VersionPatch)
	};
};
