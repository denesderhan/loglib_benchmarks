/*
 * SPDX-FileCopyrightText: 2024 Dénes Derhán <denes.derhan@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once
//#define NON_BLOCKING
#define FMT_UNICODE 0
#include <log4cxx/log4cxx.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/appenderskeleton.h>
#include <log4cxx/helpers/stringhelper.h>
#include <log4cxx/logger.h>
#include <log4cxx/xml/domconfigurator.h>
#include <log4cxx/helpers/exception.h>
#include <log4cxx/helpers/transcoder.h>
#if LOG4CXX_USING_STD_FORMAT
#include <format>
#else
#include <fmt/core.h>
#include <fmt/color.h>
#include <fmt/ostream.h>
#endif

#include <system_error>
#include <string>
#include <fstream>

#include <logbench/force_inline.hpp>
#include <logbench/test_in_param.hpp>
#include <logbench/test_out_param.hpp>

namespace LOG4CXX_NS {

	class NullWriterAppender : public AppenderSkeleton {
	public:
		DECLARE_LOG4CXX_OBJECT(NullWriterAppender)
		BEGIN_LOG4CXX_CAST_MAP()
			LOG4CXX_CAST_ENTRY(NullWriterAppender)
			LOG4CXX_CAST_ENTRY_CHAIN(AppenderSkeleton)
		END_LOG4CXX_CAST_MAP()

		NullWriterAppender() {}

		void close() override {}

		bool requiresLayout() const override {
			return false;
		}

		void append(const spi::LoggingEventPtr& event, helpers::Pool& p) override {
			// This gets called whenever there is a valid event for our appender.
		}

		void activateOptions(helpers::Pool& /* pool */) override {
			// Given all of our options, do something useful(e.g. open a file)
		}

		void setOption(const LogString& option, const LogString& value) override {
			if (helpers::StringHelper::equalsIgnoreCase(option,
				LOG4CXX_STR("SOMEVALUE"), LOG4CXX_STR("somevalue"))) {
				// Do something with the 'value' here.
			}
		}
	};

	IMPLEMENT_LOG4CXX_OBJECT(NullWriterAppender)

}

class logger {
public:
	logger(int id = 0) {}
	~logger() {}

	template<typename... Args>
	LOGBENCH_FORCEINLINE void log_test1(Args &&... args) {
		LOG4CXX_INFO_FMT(logger_, "Thr: {} Log_n: {} Time: {} {} {}", args...);
	}

	static void set_log_template(std::string_view templ) {
		log_template_ = templ;
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
		//out_data.lib_buffer_size = ? ? ;
		//out_data.buffer_flush_time = ? ;
		//out_data.sink_flush_time = ? ;

		if (init_data.lib_buffer_size != -1) {
			buffer_size_ = size_t(init_data.lib_buffer_size) * 1024 / 82 * init_data.thread_num;
			out_data.lib_buffer_size = init_data.lib_buffer_size;
		}
		else {
			//fix this what is the default?
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
		out_data.formater_impl = "fmtlib_{fmt}";

		if (out_data.sink_type == logbench::sink_typ::lib) {
			out_data.sink_type = logbench::sink_typ::sort;
		}
		bool null_sink{ false };
		if (out_data.sink_type == logbench::sink_typ::sort) {
		}
		else if (out_data.sink_type == logbench::sink_typ::null) {
			null_sink = true;
		}
		else {
			throw std::runtime_error("Sink type not supported!");
		}
		std::string log_file {init_data.log_path };
		log_file += "/log.txt";
		std::string config_path {init_data.temp_path };
		config_path += "/config.xml";

		std::ofstream config_file(config_path, std::ios::trunc | std::ios::out);
		if (!config_file.good()) {
			throw std::runtime_error("Creating config file failed!");
		}

		config_file <<
			R"(<?xml version="1.0" encoding="UTF-8" ?>
<log4j:configuration xmlns:log4j='http://jakarta.apache.org/log4j/'>
)";
		if (!null_sink) {
			config_file <<
R"(<appender name="FILE" class="org.apache.log4j.FileAppender">
	<param name="file" value=")";
			config_file << log_file << "\" />\n";
			config_file <<
				R"(	<param name="append" value="false" />
	<param name="immediateFlush" value="false" />
	<layout class="org.apache.log4j.PatternLayout">
		<param name="ConversionPattern" value=")";
			config_file << log_template_ << "\" />\n";
			config_file <<
				R"(	</layout>
  </appender>)";
		}
		else {
			config_file <<
R"(<appender name="FILE" class="org.apache.log4j.NullWriterAppender">
  </appender>)";
		}
		config_file <<
			R"(  
  <appender name="ASYNC" class="org.apache.log4j.AsyncAppender">)" "\n";
		if (buffer_size_ != 0) {
			config_file << R"(	<param name="BufferSize" value=")";
			config_file << buffer_size_ << "\" />\n";
		}
		config_file <<
R"(	<param name="Blocking" value="true"/>
    <param name="LocationInfo" value="false"/>
    <appender-ref ref="FILE"/>
  </appender>

  <root>
    <level value="INFO"/>
    <appender-ref ref="ASYNC" />
  </root>
</log4j:configuration>)";
		config_file.close();
		if (!config_file.good()) {
			throw std::runtime_error("Creating config file failed!");
		}

		auto result = log4cxx::xml::DOMConfigurator::configure(config_path);
		if (result == log4cxx::spi::ConfigurationStatus::NotConfigured)
			throw std::runtime_error("Configuration failed!");
	}

	static void sys_stop() noexcept {
		log4cxx::LogString app_str;
		log4cxx::helpers::Transcoder::decode("ASYNC", app_str);
		log4cxx::AppenderPtr app_ptr = logger_->getAppender(app_str); \
		if (app_ptr != NULL) app_ptr->close();
	}

	static constexpr std::string_view lib_name() noexcept {
		return "log4cxx";
	}

	static std::string_view lib_version() noexcept {
		return version_;
	}

	inline static std::string version_{
		std::to_string(LOG4CXX_VERSION_MAJOR) 
		+ '.' + std::to_string(LOG4CXX_VERSION_MINOR) 
		+ '.' + std::to_string(LOG4CXX_VERSION_PATCH) };
	inline static std::string log_template_{"%d{yyyy-MM-dd HH:mm:ss,SSS} %m%n"};
	inline static log4cxx::LoggerPtr logger_ = log4cxx::Logger::getRootLogger();
	inline static size_t buffer_size_{ 0 };
};
