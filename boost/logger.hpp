/*
 * SPDX-FileCopyrightText: 2024 Dénes Derhán <denes.derhan@gmail.com>
 *
 * SPDX-License-Identifier: BSL-1.0
 */
#pragma once

#include <logbench/force_inline.hpp>
#include <logbench/test_in_param.hpp>
#include <logbench/test_out_param.hpp>

#include <fstream>
#include <string>
#include <system_error>

#include <boost/core/ref.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/trivial.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/version.hpp>

namespace logging = boost::log;
namespace attrs = boost::log::attributes;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;

class null_backend :
	public sinks::basic_sink_backend< sinks::concurrent_feeding >
{
public:
	// The function is called for every log record to be written to log
	void consume(logging::record_view const& rec) {}
};

//BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(
//	global_logger, 
//	src::severity_logger_mt<logging::trivial::severity_level>)

class logger {
public:
	logger([[maybe_unused]] int id = 0) {}
	~logger() {}

	//log function for test1
	LOGBENCH_FORCEINLINE void log_test1(int id, uint64_t i, uint64_t call_time, double d, float f) {
		BOOST_LOG_SEV(logger_, logging::trivial::severity_level::error) 
			<< __FILE__ << ':' << __LINE__ 
			<< " Thr: " << id << " Log_n: " << i << " Time: " 
			<< call_time << ' ' << d << ' ' << f;
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
			throw std::runtime_error("Setting buffer size not supported! (compile time const)");
		}
		else {
			//FIX THIS is this the default?
			out_data.lib_buffer_size = 64;
		}

		if (init_data.buffer_flush_time != -1) {
			throw std::runtime_error("Can buffer polling be set? To do impl!");
		}
		else {
			out_data.buffer_flush_time = 0;
		}

		if (init_data.sink_flush_time != -1) {
			throw std::runtime_error("Can sink flush time be set? To do impl!");
		}
		else {
			out_data.sink_flush_time = 0;
		}

		std::string log_file {init_data.log_path };
		log_file += "/log.txt";
		
		if (out_data.formatter_type != logbench::formatter_typ::txt) {
			throw std::runtime_error("Formatter type not supported!");
		}
		out_data.formater_impl = "insertion_operator";

		if (out_data.sink_type == logbench::sink_typ::lib) {
			out_data.sink_type = logbench::sink_typ::sort;
		}
		if (out_data.sink_type == logbench::sink_typ::null) {
			boost::shared_ptr< sink_t_null > sink(new sink_t_null());
			logging::core::get()->add_sink(sink);
		}
		else if (out_data.sink_type == logbench::sink_typ::sort) {
			boost::shared_ptr< std::ostream > strm(new std::ofstream(log_file.c_str()));
			if (!strm->good()) {
				throw std::runtime_error("Can't create log file!");
			}
			boost::shared_ptr< sink_t > sink(new sink_t(boost::make_shared< backend_txt >()));
			sink->locked_backend()->add_stream(strm);
			sink->set_formatter(expr::format("%1% %2% %3%") 
				% expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S,%f  +????") 
				% logging::trivial::severity
				% expr::smessage);
			logging::core::get()->add_sink(sink);
			logging::core::get()->add_global_attribute("TimeStamp", attrs::local_clock());
			text_sink_ = sink;
		}
		else {
			throw std::runtime_error("Sink type not supported!");
		}
	}

	static void sys_stop() noexcept {
		if (text_sink_ != nullptr) {
			text_sink_->stop();
			text_sink_->flush();
		}
	}

	static constexpr std::string_view lib_name() noexcept {
		return "boost";
	}

	static std::string_view lib_version() noexcept {
		return boost_version_;
	}

	typedef sinks::unlocked_sink< null_backend > sink_t_null;
	static constexpr size_t max_queue_size_ = uint64_t(64) * 1024 / 82;
	typedef sinks::text_ostream_backend backend_txt;
	typedef sinks::asynchronous_sink<
		backend_txt,
		sinks::bounded_fifo_queue<max_queue_size_, sinks::block_on_overflow>
	> sink_t;
	inline static boost::shared_ptr<sink_t> text_sink_;
	src::severity_logger<logging::trivial::severity_level> logger_{};
	inline static const std::string boost_version_{
		std::to_string(BOOST_VERSION / 100000)
		+ "." + std::to_string(BOOST_VERSION / 100 % 1000)
		+ "." + std::to_string(BOOST_VERSION % 100)
	};
};
