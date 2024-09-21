/*
 * SPDX-FileCopyrightText: 2024 Dénes Derhán <denes.derhan@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <absl/base/log_severity.h>
#include <absl/log/globals.h>
#include <absl/log/initialize.h>
#include <absl/log/log_sink.h>
#include <absl/log/log_sink_registry.h>
#include <absl/log/log.h>

#include <system_error>
#include <string>
#include <fstream>
#include <mutex>

#include <logbench/force_inline.hpp>
#include <logbench/test_in_param.hpp>
#include <logbench/test_out_param.hpp>

class null_sink final : public absl::LogSink {
public:
	void Send(const absl::LogEntry&) override {}
	void Flush() override {}
};

class file_sink final : public absl::LogSink {
public:
	void open(const char* path){
		std::lock_guard<std::mutex> grd(mutex_);
		if (file_.is_open()) return;
		file_ = std::ofstream(path);
	}

	void Send(const absl::LogEntry& entry) override {
		std::lock_guard<std::mutex> grd(mutex_);
		file_.write(entry.text_message_with_prefix().data(), entry.text_message_with_prefix().size());
		const char e{ '\n' };
		file_.write(&e, 1);
	}
	void Flush() override { 
		std::lock_guard<std::mutex> grd(mutex_);
		file_.flush(); 
	}
private:
	std::ofstream file_;
	std::mutex mutex_;
};

class logger {
public:
	logger(int id = 0) {}
	~logger() {}

	LOGBENCH_FORCEINLINE void log_test1(int id, uint64_t i, uint64_t call_time, double d, float f) {
		LOG(INFO) << __FILE__ << "Thr: " << id << " Log_n: " << i << " Time: " << call_time << ' ' << d << ' ' << f;
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
		out_data.formater_impl = "?";

		if (out_data.sink_type == logbench::sink_typ::lib) {
			out_data.sink_type = logbench::sink_typ::sort;
		}
		if (out_data.sink_type == logbench::sink_typ::sort
			|| out_data.sink_type == logbench::sink_typ::null) 
		{
			//OK
		}
		else {
			throw std::runtime_error("Sink type not supported!");
		}

		if (out_data.sink_type == logbench::sink_typ::sort) {
			
		}
		else if (out_data.sink_type == logbench::sink_typ::null) {
			
		}
		

		absl::InitializeLog();
		absl::SetStderrThreshold(absl::LogSeverityAtLeast::kFatal);
		absl::SetMinLogLevel(absl::LogSeverityAtLeast::kInfo);
		
		if (out_data.sink_type == logbench::sink_typ::sort) {
			std::string temp{init_data.log_path};
			temp = temp + "/log.txt";
			sink_.open(temp.c_str());
			absl::AddLogSink(&sink_);
		}
		else if (out_data.sink_type == logbench::sink_typ::null) {
			absl::AddLogSink(&null_sink_);
		}
	}

	static void sys_stop() noexcept {}

	static constexpr std::string_view lib_name() noexcept {
		return "abseil";
	}

	static std::string_view lib_version() noexcept {
		return abseil_version_;
	}

	inline static file_sink sink_;
	inline static null_sink null_sink_;
	inline static const std::string abseil_version_{
		std::to_string(ABSL_LTS_RELEASE_VERSION)
		+ '.' + std::to_string(ABSL_LTS_RELEASE_PATCH_LEVEL)
	};
};

/*
If you are familiar with the Google Logging (glog) library, 
these examples will look very familiar; that project and this one share a common ancestor.  
The core macro APIs are essentially unchanged, however other interfaces like the extension points 
have changed quite a bit.  While those changes aim to improve interfaces, performance, extensibility, etc., 
the main reason to choose Abseil Logging over glog is for its compatibility and integration 
with Abseil and with Google's internal code now and in the future.
*/
