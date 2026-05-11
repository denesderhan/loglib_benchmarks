/*
 * SPDX-FileCopyrightText: 2024 Dénes Derhán <denes.derhan@gmail.com>
 *
 * SPDX-License-Identifier: BSL-1.0
 */
#pragma once
#include <logbench/test_in_param.hpp>
#include <logbench/test_out_param.hpp>

#include <stdexcept>
#include <string_view>
#include <string>

#include <Poco/Environment.h>
#include <Poco/AsyncChannel.h>
#include <Poco/FormattingChannel.h>
#include <Poco/Logger.h>
#include <Poco/Message.h>
#include <Poco/NullChannel.h>
#include <Poco/PatternFormatter.h>
#include <Poco/SimpleFileChannel.h>

class logger {
public:
	logger(int id = 0) 
		: logger_instance_{Poco::Logger::get(std::to_string(id))}
	{}
	~logger() {}

	auto& logger_impl() {
		return logger_instance_;
	}

	static void set_log_template(std::string_view templ)  noexcept {
		log_templ_ = templ;
	}

	static void sys_init(
		logbench::test_in_param const& init_data,
		logbench::test_out_param& out_data)
	{
		auto version = Poco::Environment::libraryVersion();
		/// Returns the POCO C++ Libraries version as a hexadecimal
		/// number in format 0xAABBCCDD, where
		///    - AA is the major version number,
		///    - BB is the minor version number,
		///    - CC is the revision number, and
		///    - DD is the patch level number.
		int patch = version & 0xFF;
		int rev = (version >> 8) & 0xFF;
		int minor = (version >> 16) & 0xFF;
		int major = (version >> 24) & 0xFF;
		version_ = std::to_string(major) + "."
			+ std::to_string(minor) + "."
			+ std::to_string(rev) + "." +
			std::to_string(patch);
		out_data.lib_name = lib_name();
		out_data.lib_version = lib_version();
		out_data.formatter_type = init_data.formatter_type;
		out_data.sink_type = init_data.sink_type;
		out_data.garbage_lines = 0;

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

		if (out_data.formatter_type != logbench::formatter_typ::txt) {
			throw std::runtime_error("Formatter type not supported!");
		}
		out_data.formater_impl = "?";

		auto patt_form(new Poco::PatternFormatter);
		patt_form->setProperty("pattern", log_templ_);

		std::string log_file{ init_data.log_path };
		log_file += "/log.txt";
		if (out_data.sink_type == logbench::sink_typ::lib) {
			out_data.sink_type = logbench::sink_typ::sort;
		}
		if (out_data.sink_type == logbench::sink_typ::null) {
			auto chan_null(new Poco::NullChannel);
			Poco::Logger::root().setChannel(chan_null);
		}
		else if (out_data.sink_type == logbench::sink_typ::sort) {
			auto chan_file(new Poco::SimpleFileChannel);
			chan_file->setProperty("path", log_file);
			chan_file->setProperty("flush", "false");
			auto form_chan(new Poco::FormattingChannel(patt_form, chan_file));
			auto chan_async(new Poco::AsyncChannel(form_chan));
			chan_async->setProperty("queueSize", "unlimited");
			Poco::Logger::root().setChannel(chan_async);
		}
		else {
			throw std::runtime_error("Sink type not supported!");
		}

		if (init_data.sink_flush_time != -1) {
			throw std::runtime_error("Can sink flush time be set? To do impl!");
		}
		else {
			out_data.sink_flush_time = 0;
		}
	}

	static void sys_stop() {
		Poco::Logger::root().shutdown();
	}

	static constexpr std::string_view lib_name() noexcept {
		return "poco";
	}

	static std::string_view lib_version() {
		return version_;
	}

	inline static std::string log_templ_{"%Y-%m-%d %H:%M:%S.%i %t"};
	inline static std::string version_{"0.0.0.0"};
	Poco::Logger& logger_instance_;
};

#define LOGBENCH_LOGCALL_PRINTF_CUSTOM

#define CUSTOM_FMT_INT32 "%?d"
#define CUSTOM_FMT_UINT64 "%?d"
#define CUSTOM_FMT_FLOAT "%hf"
#define CUSTOM_FMT_DOUBLE "%f"

#define LOGBENCH_LOG_TRACE(logger, ...) logger.logger_impl().trace(Poco::format(__VA_ARGS__), __FILE__, __LINE__)
#define LOGBENCH_LOG_DEBUG(logger, ...) logger.logger_impl().debug(Poco::format(__VA_ARGS__), __FILE__, __LINE__)
#define LOGBENCH_LOG_INFO(logger, ...) logger.logger_impl().information(Poco::format(__VA_ARGS__), __FILE__, __LINE__)
#define LOGBENCH_LOG_WARN(logger, ...) logger.logger_impl().warning(Poco::format(__VA_ARGS__), __FILE__, __LINE__)
#define LOGBENCH_LOG_ERROR(logger, ...) logger.logger_impl().error(Poco::format(__VA_ARGS__), __FILE__, __LINE__)
#define LOGBENCH_LOG_FATAL(logger, ...) logger.logger_impl().fatal(Poco::format(__VA_ARGS__), __FILE__, __LINE__)

