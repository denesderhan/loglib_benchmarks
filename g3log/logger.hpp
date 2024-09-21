/*
 * SPDX-FileCopyrightText: 2024 Dénes Derhán <denes.derhan@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>
#include <g3log/logmessage.hpp>

#include <stdexcept>
#include <string_view>
#include <string>
#include <cassert>

#include <logbench/force_inline.hpp>
#include <logbench/test_in_param.hpp>
#include <logbench/test_out_param.hpp>

struct null_sink {
	void sink_message(g3::LogMessageMover logEntry) {}
};

class logger {
public:
	logger(int id = 0) {}
	~logger() {}

	template<typename... Args>
	LOGBENCH_FORCEINLINE void log_test1(Args &&... args) {
		LOGF(INFO, "Thr: %d Log_n: %llu Time: %llu %f %f", args...);
	}

	static void set_time_template(std::string_view templ)  noexcept {
		time_templ_ = templ;
	}

	static void sys_init(
		logbench::test_in_param const& init_data,
		logbench::test_out_param& out_data)
	{
		out_data.lib_name = lib_name();
		out_data.lib_version = lib_version();
		out_data.formatter_type = init_data.formatter_type;
		out_data.sink_type = init_data.sink_type;
		out_data.garbage_lines = 2;
		//out_data.lib_buffer_size = ? ? ;
		//out_data.buffer_flush_time = ? ;
		//out_data.sink_flush_time = ? ;
		
		if (init_data.lib_buffer_size != -1) {
			throw std::runtime_error("Can buffer size be set? To do impl!");
		}
		else {
			out_data.lib_buffer_size = -1;
		}
		
		if (init_data.buffer_flush_time != -1) {
			throw std::runtime_error("Can buffer polling be set? To do impl!");
		}
		else {
			out_data.buffer_flush_time = -1;
		}

		if (init_data.sink_flush_time != -1) {
			throw std::runtime_error("Can sink flush time be set? To do impl!");
		}
		else {
			out_data.sink_flush_time = 0;
		}
		
		if (out_data.formatter_type != logbench::formatter_typ::txt) {
			throw std::runtime_error("Formatter type not supported!");
		}
		out_data.formater_impl = "printf?";

		assert(worker_ != nullptr);
		if (out_data.sink_type == logbench::sink_typ::lib) {
			out_data.sink_type = logbench::sink_typ::sort;
		}
		if (out_data.sink_type == logbench::sink_typ::null) {
			auto sink = worker_->addSink(std::make_unique<null_sink>(),
				&null_sink::sink_message);
		}
		else if (out_data.sink_type == logbench::sink_typ::sort) {
			std::string log_prefix {"log"};
			std::string log_path {init_data.log_path };
			auto sink = worker_->addDefaultLogger(log_prefix, log_path);
			//sink->call(&g3::FileSink::overrideLogDetails, &MinimalLogDetailsToString);
			sink->call(&g3::FileSink::overrideLogHeader, "");
		}
		else {
			throw std::runtime_error("Sink type not supported!");
		}

		g3::initializeLogging(worker_);
	}

	static void sys_stop() {}

	static constexpr std::string_view lib_name() noexcept {
		return "g3log";
	}

	static std::string_view lib_version() {
		return  "0.0.0";
	}

	static std::string MinimalLogDetailsToString(const g3::LogMessage& msg) {
		std::string out;
		out.append(msg.timestamp(time_templ_));
		return out;
	};

	inline static std::string time_templ_{"%Y-%m-%d %H:%M:%S.%f3 "};
	inline static uint32_t logger_buffer_size_{ 0 };
	inline static g3::LogWorker* worker_;
};
