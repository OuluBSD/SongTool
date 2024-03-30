#include "Task.h"
#include <TextDatabase/TextDatabase.h>

#ifdef flagLLAMACCP
#include <LlamaCpp/LlamaCpp.h>
#endif


BEGIN_TEXTLIB_NAMESPACE


void AiTask::Store(bool force) {
	if (output.IsEmpty()) return;
	if (!changed) return;
	String dir = ConfigFile("share" DIR_SEPS "ai_results" DIR_SEPS);
	RealizeDirectory(dir);
	String filename = GetInputHash() + ".txt";
	String file = dir + filename;
	//ASSERT(force || !FileExists(file));
	SaveFileBOMUtf8(file, output);
	changed = false;
}

void AiTask::Load() {
	if (skip_load)
		return;
	String dir = ConfigFile("share" DIR_SEPS "ai_results" DIR_SEPS);
	RealizeDirectory(dir);
	String filename = GetInputHash() + ".txt";
	String file = dir + filename;
	if (FileExists(file))
		output = LoadFileBOM(file);
	if (image_task) {
		String dir = ConfigFile("images");
		Vector<String> rel_paths = Split(output, "\n");
		recv_images.Clear();
		bool fail = false;
		for (String& rel_path : rel_paths) {
			String path = AppendFileName(dir, rel_path);
			Image& img = recv_images.Add();
			img = StreamRaster::LoadFileAny(path);
			if (img.IsEmpty())
				fail = true;
		}
		if (fail) {
			recv_images.Clear();
			output.Clear();
		}
	}
}

void AiTask::SetError(String s) {
	failed = true;
	error = s;
	WhenError();
}

String AiTask::GetInputHash() const {
	String input =
		raw_input.GetCount() ?
			raw_input :
			this->input.AsString();
	hash_t h = input.GetHashValue();
	return HexString((void*)&h, sizeof(h));
}

String AiTask::GetOutputHash() const {
	hash_t h = output.GetHashValue();
	return HexString((void*)&h, sizeof(h));
}

String AiTask::GetDescription() const {
	String s;
	s << GetTypeString();
	return s;
}

String AiTask::GetTypeString() const {
	return TaskRule::name;
}

bool AiTask::ProcessInput() {
	bool ok = true;
	
	if (raw_input.GetCount()) {
		Load();
	}
	else {
		// Return if this task won't have input function
		if (!TaskRule::input)
			return ok;
		
		// Create input with given function
		if (TaskRule::input) {
			input.Clear();
			(this->*TaskRule::input)();
			if (fast_exit)
				return true;
			
			if (failed)
				return false;
			
			if (1) {
				String in = input.AsString();
				if (TaskRule::debug_input) {
					LOG(in);
					LOG("Last 3 chars: \"" + in.Right(3) + "\"");
					TODO
				}
			}
			
			Load();
		}
	}
	
	// Remove Win32 uselessness (\r in newline)
	output = TrimBoth(output);
	
	// Request output from completion-mode AI
	if ((!raw_input.IsEmpty() || !input.IsEmpty()) && output.IsEmpty()) {
		ok = RunOpenAI();
	}
	
	// Remove Win32 uselessness (\r in newline)
	output.Replace("\r","");
	
	if (output.IsEmpty())
		return false;
	
	return ok;
}

void AiTask::Process() {
	//LOG("AiTask::Process: begin of " << TaskRule::name);
	processing = true;
	
	bool ok = true;
	
	EnterAppMode(appmode);
	
	ok = ok && ProcessInput();
	
	if (fast_exit) {
		ready = true;
	}
	else if (ok) {
		if (TaskRule::process) {
			try {
				(this->*TaskRule::process)();
			}
			catch (NoPointerExc e) {
				LOG("error: " << e);
			}
		}
		
		if (wait_task) {
			wait_task = false;
		}
		else {
			if (!failed) {
				ready = true;
			}
			changed = true;
		}
	}
	
	fast_exit = false;
	skip_load = false;
	processing = false;
	
	if (ready)
		WhenDone();
	
	LeaveAppMode();
	//LOG("AiTask::Process: end of " << rule->name);
}

bool AiTask::RunOpenAI() {
	if (image_task)
		return RunOpenAI_Image();
	else
		return RunOpenAI_Completion();
}

bool AiTask::RunOpenAI_Image() {
	output.Clear();
	
	String prompt =
		raw_input.GetCount() ?
			raw_input :
			input.AsString();
	
	prompt.Replace("\\", "\\\\");
	prompt.Replace("\n", " ");
	prompt.Replace("\t", " ");
	prompt.Replace("\"", "\\\"");
	prompt = TrimBoth(prompt);
	
	prompt = FixInvalidChars(prompt); // NOTE: warning: might break something
	
	if (image_n.IsEmpty() || image_sz.IsEmpty()) {
		SetError("No image arguments set");
		return false;
	}
	
	String recv;
	try {
		if (TaskRule::imageedit_task) {
			if (send_images.GetCount() != 1) {
				SetError("expected sendable images");
				return false;
			}
			String file_path0 = ConfigFile("tmp0.png");
			PNGEncoder().SaveFile(file_path0, send_images[0]);
			
			openai::Json json({
			    { "image", file_path0.Begin()},
			    { "prompt", prompt.Begin()},
			    { "n", StrInt(image_n) },
			    { "size", image_sz },
			    { "response_format", "b64_json" }
			});
			auto img = openai::image().edit(json);
			recv = String(img.dump(2));
		}
		else if (TaskRule::imagevariate_task){
			if (send_images.GetCount() != 1) {
				SetError("expected sendable images");
				return false;
			}
			String file_path0 = ConfigFile("tmp0.png");
			PNGEncoder().SaveFile(file_path0, send_images[0]);
			
			openai::Json json({
			    { "image", file_path0.Begin()},
			    { "n", StrInt(image_n) },
			    { "size", image_sz },
			    { "response_format", "b64_json" }
			});
			auto img = openai::image().variation(json);
			recv = String(img.dump(2));
		}
		else {
			openai::Json json({
			    { "prompt", prompt.Begin()},
			    { "n", StrInt(image_n) },
			    { "size", image_sz },
			    { "response_format", "b64_json" }
			});
			auto img = openai::image().create(json);
			recv = String(img.dump(2));
		}
	}
	catch (std::runtime_error e) {
		if (keep_going) {output = " "; return true;}
		LOG(prompt);
		fatal_error = true;
		SetError(e.what());
		return false;
	}
	catch (std::string e) {
		if (keep_going) {output = " "; return true;}
		LOG(prompt);
		fatal_error = true;
		SetError(e.c_str());
		return false;
	}
	catch (NLOHMANN_JSON_NAMESPACE::detail::parse_error e) {
		if (keep_going) {output = " "; return true;}
		LOG(prompt);
		LOG(e.what());
		fatal_error = true;
		SetError(e.what());
		return false;
	}
	catch (std::exception e) {
		if (keep_going) {output = " "; return true;}
		LOG(prompt);
		SetError(e.what());
		fatal_error = true;
		return false;
	}
    DalleResponse response;
    LoadFromJson(response, recv);
    
    output.Clear();
    recv_images.Clear();
    
    for(int i = 0; i < response.data.GetCount(); i++) {
	    String img_str = Base64Decode(response.data[i].b64_json);
	    
		Image& in = recv_images.Add();
		in = StreamRaster::LoadStringAny(img_str);
		
		// Get file path
		String part_str = " " + IntStr(i+1) + "/" + IntStr(response.data.GetCount());
		if (TaskRule::imageedit_task || TaskRule::imagevariate_task)
			part_str << " " << IntStr64(Random64()); // add never-matching random number to name for editing and variation creation purposes
		String dir = ConfigFile("images");
		String filename = Base64Encode(prompt + part_str) + ".png";
		String rel_path = AppendFileName(image_sz, filename);
		String path = AppendFileName(dir, rel_path);
		RealizeDirectory(GetFileDirectory(path));
		
		// Store to file
		PNGEncoder enc;
		enc.SaveFile(path, in);
		
		// Add file path to output
		output << rel_path << "\n";
    }
	
	changed = true;
	Store();
	return output.GetCount() > 0;
}

bool AiTask::RunOpenAI_Completion() {
	output.Clear();
	
	if (!input.response_length) {
		LOG("warning: no response length set");
		input.response_length = 1024;
	}
	String prompt =
		raw_input.GetCount() ?
			raw_input :
			input.AsString();
	//LOG(prompt);
	
	#ifdef flagLLAMACPP
	if (1) {
		LlamaCppResponse response;
		response.SetPrompt(prompt);
		response.SetMaxTokens(input.response_length);
		response.Process();
		output = response.GetOutput();
	} else
	#endif
	{
		/*prompt.Replace("\r", "");
		prompt.Replace("\n", "\\n");
		prompt.Replace("\t", "\\t");
		prompt.Replace("\"", "\\\"");*/
		EscapeString(prompt);
		
		//prompt = FixInvalidChars(prompt); // NOTE: warning: might break something
		//prompt.Replace("\'", "\\\'");
		
		if (GetDefaultCharset() != CHARSET_UTF8)
			prompt = ToCharset(CHARSET_UTF8, prompt, CHARSET_DEFAULT);
		
	    // "model": "text-davinci-003",
	    
		String txt = R"_({
	    "model": "gpt-3.5-turbo-instruct",
	    "prompt": ")_" + prompt + R"_(",
	    "max_tokens": )_" + IntStr(input.response_length) + R"_(,
	    "temperature": 1
	})_";
	    //LOG(txt);
		
		try {
			nlohmann::json json = nlohmann::json::parse(txt.Begin(), txt.End());
			auto completion = openai::completion().create(json);
		    //LOG("Response is:\n" << completion.dump(2));
		    
		    OpenAiResponse response;
		    
		    LoadFromJson(response, String(completion.dump(2)));
		    //LOG(response.ToString());
		    
		    if (response.choices.GetCount())
				output = response.choices[0].text;
		    else
		        output.Clear();
		}
		catch (std::runtime_error e) {
			if (keep_going) {output = " "; return true;}
			LOG(prompt);
			LOG(txt);
			fatal_error = true;
			SetError(e.what());
			return false;
		}
		catch (std::string e) {
			if (keep_going) {output = " "; return true;}
			LOG(prompt);
			LOG(txt);
			fatal_error = true;
			SetError(e.c_str());
			return false;
		}
		catch (NLOHMANN_JSON_NAMESPACE::detail::parse_error e) {
			if (keep_going) {output = " "; return true;}
			LOG(prompt);
			LOG(txt);
			LOG(e.what());
			fatal_error = true;
			SetError(e.what());
			return false;
		}
		catch (std::exception e) {
			if (keep_going) {output = " "; return true;}
			LOG(prompt);
			LOG(txt);
			SetError(e.what());
			fatal_error = true;
			return false;
		}
		/*catch (...) {
			SetError("unknown error");
			return false;
		}*/
		
		//LOG(IntStr64(input.AsString().GetHashValue()));
		
		// Fix unicode formatting
		output = ToUnicode(output, CHARSET_UTF8).ToString();
	}
	
	changed = true;
	Store();
	return output.GetCount() > 0;
}


void AiTask::Retry(bool skip_prompt, bool skip_cache) {
	if (!skip_prompt) {
		input.Clear();
		output.Clear();
	}
	skip_load = skip_cache;
	failed = false;
	fatal_error = false;
	ready = false;
	error.Clear();
	changed = true;
	tries = 0;
}

TaskMgr& AiTask::GetTaskMgr() {
	return GetPipe();
}

TaskMgr& AiTask::GetPipe() {
	return TaskMgr::Single();
}


END_TEXTLIB_NAMESPACE
