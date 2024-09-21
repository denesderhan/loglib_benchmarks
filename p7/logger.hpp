/*
 * SPDX-FileCopyrightText: 2024 Dénes Derhán <denes.derhan@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */
#pragma once

#include <cstddef>
#include <stdexcept>
#include <string_view>
#include <string>
#include <sstream>

#include <P7/P7_Trace.h>
#include <P7/P7_Client.h>
#include <P7/P7_Version.h>
#include <logbench/force_inline.hpp>
#include <logbench/test_in_param.hpp>
#include <logbench/test_out_param.hpp>

class logger {
public:
	logger(int id = 0) {
		p7_client_share_ = P7_Get_Shared(TM("baicalclient"));
		if (!p7_client_share_) {
			throw std::runtime_error("Error creating P7 client!");
		}
		std::basic_string<XCHAR> tr_ch_name{ TM("trace_ch_") };
#ifdef _WIN32
		tr_ch_name += std::to_wstring(id);
#else
		tr_ch_name += std::to_string(id);
#endif
		p7_trace_ = P7_Create_Trace(p7_client_, tr_ch_name.c_str());
		if (!p7_trace_) {
			p7_client_share_->Release();
			throw std::runtime_error("Error creating P7 Trace channel!");
		}
	}
	~logger() {
		p7_trace_->Release();
		p7_client_share_->Release();
	}

	template<typename... Args>
	LOGBENCH_FORCEINLINE void log_test1(Args &&... args) {
		p7_trace_->P7_INFO(NULL, TM("Thr: %d Log_n: %llu Time: %llu %f %f"), args...);
	}

	static void set_log_template(std::string_view templ)  noexcept {
		log_templ_ = std::basic_string<XCHAR>{};
		auto char_ptr = templ.data();
		for (int i = 0; i < templ.size(); i++) {
			log_templ_ += static_cast<XCHAR>(*char_ptr++);
		}
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
			logger_buffer_size_ = uint32_t(init_data.lib_buffer_size) * init_data.thread_num;
			out_data.lib_buffer_size = logger_buffer_size_;
		}
		else {
			//FIX THIS what is the default buffer size per thread?
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
			out_data.sink_flush_time = 1000;
		}
		
		bool null_sink{ false };
		if (out_data.sink_type == logbench::sink_typ::lib) {
			out_data.sink_type = logbench::sink_typ::sort;
		}
		if (out_data.sink_type == logbench::sink_typ::sort) {
			
		}
		else if(out_data.sink_type == logbench::sink_typ::null) {
			null_sink = true;
		}
		else {
			throw std::runtime_error("Sink type not supported!");
		}

		P7_Set_Crash_Handler();
		std::basic_string<XCHAR> log_path;
		auto char_ptr = init_data.log_path.data();
		for (int i = 0; i < init_data.log_path.size(); i++) {
			log_path += static_cast<wchar_t>(*char_ptr++);
		}
#ifdef _WIN32
		std::wstringstream lib_param;
#else
		std::stringstream lib_param;
#endif
		if (out_data.formatter_type == logbench::formatter_typ::txt) {
			if (null_sink) { 
				lib_param << TM("/P7.Sink=Null"); 
			}
			else {
				lib_param << TM("/P7.Sink=FileTxt /P7.Files=1 /P7.Dir=\"");
				lib_param << log_path;
				lib_param << TM("\"");
				lib_param << TM(" /P7.Format=\"");
				lib_param << log_templ_;
				lib_param << TM("\"");
			}
			if (logger_buffer_size_ != 0) {
				lib_param << TM(" /P7.Pool=") << logger_buffer_size_;
			}
			out_data.formater_impl = "?";
		}
		else if (out_data.formatter_type == logbench::formatter_typ::bin) {
			if (null_sink) {
				lib_param << TM("/P7.Sink=Null");
			}
			else {
				lib_param << TM("/P7.Sink=FileBin /P7.Files=1 /P7.Dir=\"");
				lib_param << log_path;
				lib_param << TM("\"");
			}
			if (logger_buffer_size_ != 0) {
				lib_param << TM(" /P7.Pool=") << logger_buffer_size_;
			}
			out_data.formater_impl = "?";
		}
		else{
			throw std::runtime_error("Formatter type not supported!");
		}
		
		p7_client_ = P7_Create_Client(lib_param.str().c_str());
		if (!p7_client_) {
			throw std::runtime_error("Error creating P7 client!");
		}
		if (!p7_client_->Share(TM("baicalclient"))) {
			p7_client_->Release();
			throw std::runtime_error("Error sharing client and/or channel!");
		}
	}

	static void sys_stop() {
		p7_client_->Flush();
		p7_client_->Release();
	}

	static constexpr std::string_view lib_name() noexcept {
		return "p7client";
	}

	static std::string_view lib_version() {
		return version_;
	}

	inline static std::basic_string<XCHAR> log_templ_{TM("%tf %ms")};
	// kbyte / thread min 16 kb / thread
	inline static uint32_t logger_buffer_size_{ 0 };
	inline static const std::string version_{ 
		std::to_string(P7_VERSION_MAJOR)
		+ '.' + std::to_string(P7_VERSION_MINOR) };
	inline static IP7_Client* p7_client_{ nullptr };
	IP7_Trace* p7_trace_{ nullptr };
	IP7_Client* p7_client_share_{ nullptr };
};

/*
“/P7.Format” – set log item format for text sink, consists of next sub-elements
o “%cn” – channel name
o “%id” – message ID
o “%ix” – message index
o “%tf” – full time: YY.MM.DD HH.MM.SS.mils.mics.nans
o “%tm” – medium time: HH.MM.SS.mils.mics.nans
o “%ts” – time short MM.SS.mils.mics.nans
o “%td” – time difference between current and prev. one +SS.mils.mics.nans
o “%tc” – time stamp in 100 nanoseconds intervals
o “%lv” – log level
o “%ti” – thread ID
o “%tn” – thread name (if it was registered)
o “%cc” – CPU core index
o “%mi” – module ID
o “%mn” – module name
o “%ff” – file name + path
o “%fs” – file name
o “%fl” – file line
o “%fn” – function name
o “%ms” – text user message
Example: “/P7.Format=\"{%cn} [%tf] %lv %ms\"”

*/
