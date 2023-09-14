#include "SongTool.h"


void Task::CreateInput_StoryArc() {
	Database& db = Database::Single();
	if (!p.pipe) {
		SetFatalError("no song pointer set");
		return;
	}
	
	PipePtrs& p = this->p;
	Pipe& pipe = *p.pipe;
	SnapArg a = p.a;
	ASSERT(a.mode != MODE_INVALID);
	
	if (pipe.parts.IsEmpty()) {
		SetFatalError("no parts in song");
		return;
	}
	
	TaskTitledList& lyrics = input.AddSub();
	lyrics.Title("Lyrics");
	for(int i = 0; i < pipe.parts.GetCount(); i++) {
		Part& part = pipe.parts[i];
		TaskTitledList& part_lyrics = lyrics.AddSub();
		part_lyrics.Title(part.name);
		part_lyrics.NoColon();
		part_lyrics.NoListChar();
		
		Array<Line>& lines = part.lines;
		for(int j = 0; j < lines.GetCount(); j++) {
			Line& line = lines[j];
			part_lyrics.Add(line.Get(a).txt);
		}
	}
	
	TaskTitledList& data_topics = input.AddSub();
	data_topics.Title("Data topics");
	data_topics.Add("Story arc");
	data_topics.Add("Shortened absolute story arc");
	data_topics.Add("Shortened absolute story arc of whole song");
	data_topics.Add("Theme of the whole song");
	data_topics.Add("Storyline of separate parts in separate sections (" << pipe.parts[0].name << ", " << pipe.parts[1].name << ", etc.)");
	
	TaskTitledList& pre_answer = input.PreAnswer();
	pre_answer.Title("Results for data topics");
	pre_answer.NoListChar();
	pre_answer.Add("Story arc");
	
	//LOG(input.GetTreeString());
	//LOG(input.AsString());
}

void Task::CreateInput_StoryArcWeighted() {
	PipePtrs& p = this->p;
	Pipe& pipe = *p.pipe;
	
	SnapArg a0 = p.a;
	SnapArg a1 = p.a;
	a0.mode = MALE;
	a1.mode = FEMALE;
	
	input.AddSub()
		.Title("An example of the process")
		.NoListChar()
		.ValueInQuotes()
		.Add("Input: Description A of X", "The narrator is feeling sarcastic and isolated")
		.Add("Input: Description B of X", "The protagonist reflects on her past relationship.")
		.Add("Result: Self-aware meta-transition from A to B", "The narrator's former lightheadedness is replaced by a sarcastically disaffected, almost-forgotten reflection of her past relationship.");
	
	
	TaskTitledList& input_data = input.AddSub();
	input_data.Title("Inputs");
	input_data.CountLines();
	
	Index<String> song_keys;
	song_keys << "shortened absolute story arc";
	song_keys << "shortened absolute story arc of whole song";
	song_keys << "theme of the whole song";
	song_keys << "storyline";
	
	int item = 0;
	for(int i = 0; i < song_keys.GetCount(); i++) {
		String key = song_keys[i];
		const PatternSnap& snap0 = pipe.snap[a0];
		const PatternSnap& snap1 = pipe.snap[a1];
		String value0 = snap0.data.Get(key, "");
		String value1 = snap1.data.Get(key, "");
		if (!value0.IsEmpty() && !value1.IsEmpty()) {
			TaskTitledList& line = input_data.AddSub();
			line.InlineList();
			line.ValueInQuotes();
			line.Add("Description A of X", value0);
			line.Add("Description B of X", value1);
			line.Arg(key);
			line.Context(static_cast<SnapContext*>(&pipe));
			item++;
		}
	}
	for(int i = 0; i < pipe.parts.GetCount(); i++) {
		Part& part = pipe.parts[i];
		{
			const PatternSnap& snap0 = part.snap[a0];
			const PatternSnap& snap1 = part.snap[a1];
			
			String key = "storyline";
			String storyline0 = snap0.data.Get(key, "");
			String storyline1 = snap1.data.Get(key, "");
			if (!storyline0.IsEmpty() && !storyline1.IsEmpty()) {
				TaskTitledList& line = input_data.AddSub();
				line.InlineList();
				line.ValueInQuotes();
				line.NoListChar();
				line.Add("Description A of X", storyline0);
				line.Add("Description B of X", storyline1);
				line.Arg(key);
				line.Context(static_cast<SnapContext*>(&part));
				item++;
			}
		}
	}
	
	TaskTitledList& pre_answer = input.PreAnswer();
	pre_answer
		.Title("List of results")
		.CountLines()
		.Add("Result", "");
	
	if (!item) {
		SetFatalError("no storylines");
		return;
	}
	
}

void Task::CreateInput_Impact() {
	Database& db = Database::Single();
	if (!p.line) {
		SetFatalError("no line pointer set");
		return;
	}
	
	PipePtrs& p = this->p;
	Pipe& pipe = *p.pipe;
	Line& line = *p.line;
	SnapArg a = p.a;
	ASSERT(a.mode != MODE_INVALID);
	
	TaskTitledList& in_breaks = input.AddSub();
	in_breaks.Title("Lyrics with breaks [br]");
	in_breaks.Separator(" [br] ");
	in_breaks.InlineList();
	for(int i = 0; i < line.breaks.GetCount(); i++) {
		in_breaks.Add(line.breaks[i].Get(a).txt);
	}
	
	TaskTitledList& in_parts = input.AddSub();
	in_parts.Title("Lyrics divided in part");
	in_parts.CountLines("Part");
	in_parts.ValueInQuotes();
	for(int i = 0; i < line.breaks.GetCount(); i++) {
		Break& brk = line.breaks[i];
		in_parts.Add(brk.Get(a).txt);
	}
	
	TaskTitledList& pre_answer = input.PreAnswer();
	pre_answer.Title("How the listener is impacted in short (no lyrics lines in answers! Don't start with the lyrics input!)");
	pre_answer.CountLines("Part");
	pre_answer.ValueInQuotes();
	//pre_answer.Separator(":");
	//pre_answer.Add(line.breaks[0].Get(a).txt);
	pre_answer.EmptyLine();
	
}

void Task::CreateInput_ImpactWeighted() {
	PipePtrs& p = this->p;
	if (!p.line) {
		SetFatalError("no line pointer set");
		return;
	}
	Pipe& pipe = *p.pipe;
	Line& line = *p.line;
	
	SnapArg a0 = p.a;
	SnapArg a1 = p.a;
	a0.mode = MALE;
	a1.mode = FEMALE;
	SnapArg a_other = a0 == p.a ? a1 : a0;
	
	input.AddSub()
		.Title("An example of the process")
		.NoListChar()
		.ValueInQuotes()
		.Add("Input: Description A of X", "The narrator is feeling sarcastic and isolated")
		.Add("Input: Description B of X", "The protagonist reflects on her past relationship.")
		.Add("Result: Self-aware meta-transition from A to B", "The narrator's former lightheadedness is replaced by a sarcastically disaffected, almost-forgotten reflection of her past relationship.");
	
	
	TaskTitledList& input_data = input.AddSub();
	input_data.Title("Inputs");
	input_data.CountLines();
	
	int item = 0;
	bool waiting = false;
	for(int i = 0; i < line.breaks.GetCount(); i++) {
		Break& brk = line.breaks[i];
		PatternSnap& snap0 = brk.snap[a0];
		PatternSnap& snap1 = brk.snap[a1];
		
		if (!snap0.impact.IsEmpty() && !snap1.impact.IsEmpty()) {
			TaskTitledList& input_line = input_data.AddSub();
			input_line.ValueInQuotes();
			input_line.InlineList();
			input_line.Add("Description A of X", snap0.impact);
			input_line.Add("Description B of X", snap1.impact);
			input_line.Context(static_cast<SnapContext*>(&brk));
			item++;
		}
		else if (brk.snap[a_other].impact.IsEmpty())
			waiting = true;
	}
	
	TaskTitledList& pre_answer = input.PreAnswer();
	pre_answer
		.Title("List of results")
		.CountLines()
		.Add("Result", "");
	
	if (!item) {
		if (waiting)
			SetWaiting();
		else
			SetFatalError("no impacts");
		return;
	}
	
}

void Task::CreateInput_ForwardLyricsWeighted() {
	PipePtrs& p = this->p;
	if (!p.line) {
		SetFatalError("no line pointer set");
		return;
	}
	Pipe& pipe = *p.pipe;
	Line& line = *p.line;
	String s;
	String parts;
	
	SnapArg a1 = p.a;
	a1.mode = COMMON;
	
	TaskTitledList& example_list = input.AddSub();
	TaskTitledList& ex1 = example_list.AddSub();
	TaskTitledList& ex2 = example_list.AddSub();
	TaskTitledList& ex3 = example_list.AddSub();
	TaskTitledList& ex4 = example_list.AddSub();
	example_list	.Title("Examples of the process")
					.NoListChar()
					.ValueInQuotes();
	ex1.InlineList().ValueInQuotes()
					.Add("Input: Words", "2")
					.Add("Input: Impact", "The somber atmosphere invites the listener to consider the emotional repercussions of shielding themselves from pain, while simultaneously evoking a sense of looming danger around the narrator.")
					.Add("Result", "Open walls");
	ex2.InlineList().ValueInQuotes()
					.Add("Input: Words", "5")
					.Add("Input: Impact", "The listener's contemplation on their own defensive behavior as a result of not expressing emotions juxtaposes with the narrator's sly intent to inspire fear in them.")
					.Add("Result", "Beware what's coming next");
	ex3.InlineList().ValueInQuotes()
					.Add("Input: Words", "5")
					.Add("Input: Impact", "The invitation to contemplate one's emotional wounds combined with the narrator's cunningly crafted fear-inducing attitude, creates an atmosphere of distress and confusion.")
					.Add("Result", "Go dig your graves");
	ex4.InlineList().ValueInQuotes()
					.Add("Input: Words", "3")
					.Add("Input: Impact", "The listener's anticipation and excitement are slowly shifted to a more restful, self-assured comfort as they accept the assurance of safety that comes with the speaker's promise of affection.")
					.Add("Result", "Trust in me");
	
	TaskTitledList& inputs = input.AddSub();
	inputs.Title("List of inputs");
	inputs.CountLines();
	
	int item = 0;
	bool waiting = false;
	for(int i = 0; i < line.breaks.GetCount(); i++) {
		Break& brk = line.breaks[i];
		PatternSnap& snap0 = brk.snap[p.a];
		PatternSnap& snap1 = brk.snap[a1];
		
		if (!snap1.txt.IsEmpty() && !snap0.impact.IsEmpty()) {
			int words = CountWords(snap1.txt);
			TaskTitledList& line = inputs.AddSub();
			line.InlineList().ValueInQuotes();
			line.Add("Input: Words", IntStr(words + 2));
			line.Add("Input: Impact", snap0.impact);
			line.Context(static_cast<SnapContext*>(&brk));
			item++;
		}
		else if (!snap0.impact.IsEmpty())
			waiting = true;
	}
	
	TaskTitledList& pre_answer = input.PreAnswer();
	pre_answer
		.Title("List of results")
		.CountLines()
		.Add("Result", "");
	
	if (!item) {
		if (waiting)
			SetWaiting();
		else
			SetFatalError("no impacts");
		return;
	}
}

void Task::CreateInput_ImpactScoring() {
	Database& db = Database::Single();
	PipePtrs& p = this->p;
	Pipe& pipe = *p.pipe;
	Attributes& g = pipe;
	AttrScore& as = pipe;
	SnapArg a = p.a;
	ASSERT(a.mode != MODE_INVALID);
	int sc = g.attr_scorings.GetCount();
	
	
	// List of axes:
	// -a Integrity: +honest/-twisted
	TaskTitledList& axes = input.AddSub();
	axes			.Title("List of axes")
					.CountLinesAlpha();
	for (const Attr::ScoringType& t : g.attr_scorings)
		axes.Add(t.klass, "+" + t.axes0 + "/-" + t.axes1);
	
	
	//Combination string from results:
	// - a (Integrity: +honest/-twisted) b (Social: +libertarian/-authoritarian) c (Economic: +liberal/-conservative) etc.
	TaskTitledList& result_example = input.AddSub();
	result_example	.Title("Combination string from results")
					.CountLinesAlpha()
					.InlineList()
					.NoSeparator();
	for(int i = 0; i < 3 && i < g.attr_scorings.GetCount(); i++) {
		const Attr::ScoringType& t = g.attr_scorings[i];
		result_example.Add(t.klass, "+" + t.axes0 + "/-" + t.axes1);
	}
	
	
	// Instruction
	input.AddSub().NoColon().Title("Combination integer values are allowed to be between -3 and +3.");
	
	Vector<int> example_values;
	example_values		<< 1
						<< 0
						<< 0
						<< 1
						
						<< 0
						<< 1
						<< 0
						<< 0
						
						<< 1
						<< 1
						<< 1
						<< 1
						
						<< 1
						<< 0
						<< 1
						<< 1
						
						<< 0
						<< 0
						<< 0
						<< 0
						;
	
	// Scores per entry:
	// Line 1, "The narrator is happy and likes trains"
	// -a Integrity: honest/twisted: +1 honest
	TaskTitledList& detailed = input.AddSub();
	detailed		.Title("Scores per entry")
					.CountSub().CountLines()
					.ValueInQuotes();
	TaskTitledList& ex1 = detailed.AddSub();
	ex1				.Title("The narrator is happy and likes trains")
					.TitleInQuotes()
					.CountLinesAlpha()
					.Separator(" ")
					.CombinationString();
	// Line 1, "The narrator is happy and likes trains"
	// Combination string: a+1 b0 c0 d+1 e0 f+1 g0 h0 i+1 j+1 k+1 l+1 m+1 n0 o+1 p+1 q0 r0 s0 t0
	TaskTitledList& summary = input.AddSub();
	summary			.CountSub().CountLines()
					.ValueInQuotes();
	TaskTitledList& ex1s = summary.AddSub();
	ex1s			.Title("The narrator is happy and likes trains")
					.TitleInQuotes()
					.Separator(" ")
					.CombinationString();
	for(int i = 0; i < example_values.GetCount() && i < g.attr_scorings.GetCount(); i++) {
		const Attr::ScoringType& t = g.attr_scorings[i];
		int value = example_values[i];
		String line = "+" + t.axes0 + "/-" + t.axes1 + ": ";
		if      (value > 0) line << "+" << value << " " << t.axes0;
		else if (value > 0) line << "-" << value << " " << t.axes1;
		else line << "0";
		ex1.Add(t.klass, line);
		ex1.scores << value;
		ex1s.scores << value;
	}
	
	
	// Entries
	TaskTitledList& entries = input.AddSub();
	entries		.Title("Entries")
				.CountLines()
				.LineBegin(2)
				.ValueInQuotes();
	TaskTitledList& results = input.PreAnswer();
	results		.Title("Lines with resulting combination strings")
				.CountLines()
				.LineBegin(2)
				.ValueInQuotes()
				.CombinationString();
	
	int arg_count = args.GetCount();
	int entry_count = 0;
	for(int i = 0; i < arg_count; i++) {
		String impact = ToLower(args[i]);
		ASSERT(impact.GetCount());
		
		PatternSnap* snap = pipe.FindSnapByImpact(impact);
		ASSERT(snap);
		if (snap && snap->impact_score.GetCount() == sc)
			continue;
		
		// Add line to ai prompt
		entries.Add(impact);
		
		// Make FIRSTENTRY prompt on first seen value
		if (!entry_count)
			results.Add(impact);
		entry_count++;
	}
	
	// This task shouldn't exist, if there is nothing to solve
	if (entry_count == 0) {
		SetFastExit();
		return;
	}
	
	input.response_length = 2*1024;
}

void Task::CreateInput_PatternMask() {
	//CombineHash hash;
	Database& db = Database::Single();
	if (!p.pipe) {
		SetFatalError("no song pointer set");
		return;
	}
	
	PipePtrs& p = this->p;
	Pipe& pipe = *p.pipe;
	SnapArg a = p.a;
	String type = args[0];
	String ai_txt = GetGroupContextNaturalDescription(p.a.ctx);
	
	// Titled list
	TaskTitledList& attr_groups = input.AddSub();
	attr_groups.Title("Groups of %s attributes");
	attr_groups.FormatArg(type);
	
	int count = 0;
	for(Attr::Group& g : pipe.attr_groups) {
		Attr::GroupType& gt = pipe.group_types[g.type_i];
		
		// Skip different group context
		if (gt.group_ctx != p.a.ctx)
			continue;
		
		// Skip unmanaged groups to avoid asking ai again and again
		if (!g.managed)
			continue;
		
		if (g.type == type) {
			String key = Capitalize(g.description);
			attr_groups.Add(key);
			count++;
		}
	}
	
	if (!count && type == GetUnknownText(a.ctx)) {SetFastExit(); return;}
	if (!count) {SetFatalError("internal error"); return;}
	
	
	// Titled list
	TaskTitledList& lyrics = input.AddSub();
	lyrics.Title("Lyrics");
	lyrics.NoListChar();
	
	String parts;
	bool any_has_txt = false;
	{
		Part& part = *p.part;
		
		Array<Line>& lines = part.lines;
		for(int j = 0; j < lines.GetCount(); j++) {
			Line& line = lines[j];
			String txt = line.Get(a).txt;
			if (!txt.IsEmpty())
				any_has_txt = true;
			lyrics.Add(txt);
		}
	}
	
	if (!any_has_txt) {
		SetFatalError("no text is in lines");
		return;
	}
	
	// Formatting string
	TaskTitledList& formatting = input.AddSub();
	formatting.Title("Groups with %s in format");
	formatting.FormatArg(ai_txt);
	formatting.Add(Capitalize(type) << ": Value 1, Value 2");
	formatting.Add("Other: Value 1, Value 2");
	
	// Pre-answer text
	TaskTitledList& pre_answer = input.PreAnswer();
	pre_answer.Title("Attributes of lyrics");
	pre_answer.EmptyLine();
	
	input.response_length = 2*1024;
}

void Task::CreateInput_PatternMaskWeighted() {
	PipePtrs& p = this->p;
	Pipe& pipe = *p.pipe;
	Part& part = *p.part;
	String s;
	String parts;
	
	SnapArg a0 = p.a;
	SnapArg a1 = p.a;
	a0.mode = MALE;
	a1.mode = FEMALE;
	
	PatternMask& mask0 = part.snap[a0];
	PatternMask& mask1 = part.snap[a1];
	
	TaskTitledList& example = input.AddSub();
	example
		.Title("Example of the process")
		.NoListChar();
	
	TaskTitledList& ex1 = example.AddSub();
	ex1		.Title("Attribute-list A describing sentence X")
			.Add("noun", "I")
			.Add("noun", "you")
			.Add("verb", "like");
	
	TaskTitledList& ex2 = example.AddSub();
	ex2		.Title("Attribute-list B describing sentence X")
			.Add("noun", "you")
			.Add("verb", "are")
			.Add("adjective", "nice");
	
	TaskTitledList& ex3 = example.AddSub();
	ex3		.Title("Difference-aware meta-transition attribute-list from A to B")
			.Add("pronoun", "I")
			.Add("verb", "admire")
			.Add("adjective", "your");
	
	input.AddSub().Title("Task 1");
	
	TaskTitledList& task = input.AddSub();
	
	TaskTitledList& in_a = task.AddSub();
	TaskTitledList& in_b = task.AddSub();
	TaskTitledList& result = input.PreAnswer();
	
	in_a.Title("Attribute-list A describing sentence X");
	for(const SnapAttrStr& sa : mask0.mask) {
		ASSERT(sa.group.GetCount() && sa.item.GetCount());
		in_a.Add(sa.group, sa.item);
	}
	
	in_b.Title("Attribute-list B describing sentence X");
	for(const SnapAttrStr& sa : mask1.mask) {
		ASSERT(sa.group.GetCount() && sa.item.GetCount());
		in_b.Add(sa.group, sa.item);
	}
	
	result.Title("Difference-aware meta-transition attribute-list from A to B");
	result.EmptyLine();
	
}

void Task::CreateInput_Pattern() {
	Database& db = Database::Single();
	if (!p.pipe || !p.part) {
		SetFatalError("no song pointer set");
		return;
	}
	
	PipePtrs& p = this->p;
	Pipe& pipe = *p.pipe;
	Part& part = *p.part;
	SnapArg a = p.a;
	ASSERT(a.mode != MODE_INVALID);
	PatternSnap& snap = *this->snap;
	PatternMask& mask = snap.part->snap[a];
	
	auto& unique_lines = part.unique_lines[a];
	
	String ai_txt = GetGroupContextNaturalDescription(p.a.ctx);
	int offset_begin = StrInt(args[0]);
	int offset_end = StrInt(args[1]);
	const Attr::Group* first_key = 0;
	
	TaskTitledList& input_groups = input.AddSub();
	input_groups.Title("Groups of attributes and allowed values");
	input_groups.Capitalize(); // values
	
	Index<int> mask_groups;
	for(const SnapAttrStr& sa : mask.mask) {
		ASSERT(sa.has_id);
		mask_groups.FindAdd(sa.group_i);
	}
	
	for(int i = 0; i < mask_groups.GetCount(); i++) {
		int group_i = mask_groups[i];
		const Attr::Group& gg = pipe.attr_groups[group_i];
		
		// Skip different group context
		const Attr::GroupType& gt = pipe.group_types[gg.type_i];
		if (gt.group_ctx != p.a.ctx)
			continue;
		
		input_groups.Add(gg.description);
		input_groups.groups << &gg;
	}
	
	TaskTitledList& input_group_values = input.AddSub();
	input_group_values.Title("Group values");
	for(int i = 0; i < mask_groups.GetCount(); i++) {
		int group_i = mask_groups[i];
		const Attr::Group& gg = pipe.attr_groups[group_i];
		
		TaskTitledList& values = input_group_values.AddSub();
		values.Title(gg.description);
		values.Capitalize();
		for(const SnapAttrStr& sa : mask.mask) {
			if (sa.group_i != group_i)
				continue;
			const String& v = gg.values[sa.item_i];
			ASSERT(v.Find(",") < 0);
			values.Add(v);
		}
		if (!first_key)
			first_key = &gg;
	}
	
	if (!first_key) {
		/*if (type == GetUnknownText(a.ctx))
			SetFastExit();
		else
			SetFatalError("No attribute groups available with the type of " + type);*/
		SetFatalError("No first key");
		return;
	}
	
	TaskTitledList& input_lyrics = input.AddSub();
	input_lyrics	.Title("Lyrics")
					.ValueInQuotes()
					.CountLines();
				
	String first_line;
	for(int i = 0, j = 0; i < unique_lines.GetCount(); i++) {
		const String& l = unique_lines.GetKey(i);
		if (i < offset_begin || i >= offset_end)
			continue;
		if (!j)
			first_line = l;
		input_lyrics.Add(l);
		j++;
	}
	
	// Instruction
	input.AddSub().NoColon().Title("Multiple answers are required.");
	
	// Result
	TaskTitledList& result = input.PreAnswer();
	result			.Title("Attributes (in format \"Group: Attribute\") for all lines")
					.CountLines()
					.NoListChar()
					.ValueInQuotes()
					.Add(first_line);
	
	input.response_length = 1024*3/2;
}

void Task::CreateInput_PatternWeighted() {
	PipePtrs& p = this->p;
	Pipe& pipe = *p.pipe;
	Part& part = *p.part;
	String s;
	String parts;
	
	SnapArg a0 = p.a;
	SnapArg a1 = p.a;
	a0.mode = MALE;
	a1.mode = FEMALE;
	
	PatternSnap& snap0 = this->ctx->snap[a0];
	PatternSnap& snap1 = this->ctx->snap[a1];
	
	TaskTitledList& example = input.AddSub();
	example	.Title("Example of the process");
	
	TaskTitledList& ex1 = example.AddSub();
	TaskTitledList& ex2 = example.AddSub();
	TaskTitledList& ex3 = example.AddSub();
	
	ex1.Title("Attribute-list A describing sentence X");
	ex2.Title("Attribute-list B describing sentence X");
	ex3.Title("Difference-aware meta-transition attribute-list from A to B");
			
	ex1.Add("noun", "I");
	ex1.Add("noun", "you");
	ex1.Add("verb", "like");
	
	ex2.Add("noun", "you");
	ex2.Add("verb", "are");
	ex2.Add("adjective", "nice");
	
	ex3.Add("pronoun", "I");
	ex3.Add("verb", "admire");
	ex3.Add("adjective", "your");
	
	TaskTitledList& task = input.AddSub();
	task.Title("Task 1");
	
	TaskTitledList& in1 = task.AddSub();
	TaskTitledList& in2 = task.AddSub();
	TaskTitledList& res = input.PreAnswer();
	in1.Title("Attribute-list A describing sentence X");
	in2.Title("Attribute-list B describing sentence X");
	res.Title("Difference-aware meta-transition attribute-list from A to B");
	res.EmptyLine();
	if (snap0.attributes.IsEmpty()) {
		SetFatalError("Snapshot from male is empty");
		return;
	}
	if (snap1.attributes.IsEmpty()) {
		SetFatalError("Snapshot from female is empty");
		return;
	}
	for(const SnapAttrStr& sa : snap0.attributes) {
		ASSERT(sa.group.GetCount() && sa.item.GetCount());
		in1.Add(sa.group, sa.item);
	}
	for(const SnapAttrStr& sa : snap1.attributes) {
		ASSERT(sa.group.GetCount() && sa.item.GetCount());
		in2.Add(sa.group, sa.item);
	}
}

void Task::CreateInput_Analysis() {
	Database& db = Database::Single();
	PipePtrs& p = this->p;
	Pipe& pipe = *p.pipe;
	SnapArg a = p.a;
	a.Chk();
	
	if (!p.pipe) {
		SetFatalError("no song pointer set");
		return;
	}
	
	if (pipe.parts.IsEmpty()) {
		SetFatalError("Empty song");
		return;
	}
	
	String vocalist_visual = pipe.vocalist_visual;
	String title = args[0];
	String first;
	
	TaskTitledList& title_list = input.AddSub();
	title_list.Title(title);
	title_list.Capitalize();
	for(int i = 1; i < args.GetCount(); i++) {
		title_list.Add(ToLower(args[i]));
	}
	
	TaskTitledList& lyrics = input.AddSub();
	lyrics.Title("Lyrics");
	lyrics.NoListChar();
	for(int i = 0; i < pipe.parts.GetCount(); i++) {
		Part& part = pipe.parts[i];
		
		if (!whole_song) {
			TaskTitledList& input_part = lyrics.AddSub();
			input_part.Title(part.name);
			input_part.NoListChar();
			
			Array<Line>& lines = part.lines;
			for(int j = 0; j < lines.GetCount(); j++) {
				Line& line = lines[j];
				const String& txt = line.Get(a).txt;
				ASSERT(txt.GetCount());
				input_part.Add(txt);
			}
		}
		else {
			//if (!parts.IsEmpty()) parts << ", ";
			//parts << part.name;
			
			Array<Line>& lines = part.lines;
			for(int j = 0; j < lines.GetCount(); j++) {
				Line& line = lines[j];
				const String& txt = line.Get(a).txt;
				ASSERT(txt.GetCount());
				lyrics.Add(txt);
			}
		}
	}
	
	// Instructions
	input.AddSub().NoColon().Title(vocalist_visual);
	input.AddSub().NoColon().Title("Multiple answers are required. Separate to all parts are required.");
	
	// Formatting
	TaskTitledList& format = input.AddSub();
	format	.Title("Formatting example")
			.Add("Literary Devices", "Alliteration, rhyme, metaphor");
	
	// Result
	if (whole_song) {
		TaskTitledList& result = input.PreAnswer();
		result.FormatArg(title);
		result.Title("Verbose %s of lyrics for whole lyrics");
		result.EmptyLine();
	}
	else {
		TaskTitledList& info = input.AddSub();
		info.FormatArg(title);
		info.Title("Verbose %s of lyrics for all parts");
		for(int i = 0; i < pipe.parts.GetCount(); i++) {
			String key = pipe.parts[i].name;
			info.Add(key);
		}
		
		TaskTitledList& result = input.PreAnswer();
		result.Title(pipe.parts[0].name);
		result.EmptyLine();
	}
	
	input.response_length = 2*1024;
}

const char* attrscore_prompt1 = R"ATRSCROO(

Scores per entry:
Line 1, Pronouns: "I":
-a Integrity: +honest/-twisted: +1 honest
-b Social: libertarian/authoritarian: +1 libertarian
-c Economic: liberal/conservative: 0
-d Culture: individualism/collective: +1 individualism

-e Human strength: strong/weak: +1 strong
-f Motivation: rewarding/punishing: +1 rewarding
-g Sexualization: sexual/non-sexual: 0
-h Beliefs: spiritual/ secular: 0

-i Expectations: acceptance/perfection: +1 acceptance
-j Mood: joyful/melancholic: +1 joyful
-k Mood: playful/serious: +1 playful
-l Mood: uplifting/heavy: +1 uplifting

-m Mood: lighthearted/somber: +1 lighthearted
-n Mood: humorous/dramatic: +1 humorous
-o Attitude: hopeful/despair: +1 hopeful
-p Attitude: optimistic/pessimistic: +1 optimistic

-q Attitude: open/closed: +1 open
-r Sexual Orientation: heterosexual/homosexual: 0
-s Sexual Preference: kinky/normal: 0
-t Physical Preference: enhancement/natural: 0
Combination string: a+1 b+1 c0 d+1 e+1 f+1 g0 h0 i+1 j+1 k+1 l+1 m+1 n+1 o+1 p+1 q+1 r0 s0 t0
)ATRSCROO";

const char* attrscore_prompt2 = R"ATRSCROO(
Line 1, Pronouns: "I":
Combination string: a+1 b+1 c0 d+1 e+1 f+1 g0 h0 i+1 j+1 k+1 l+1 m+1 n+1 o+1 p+1 q+1 r0 s0 t0

Entries:
${ENTRIES}

${FIRSTENTRY}:
Combination string:)ATRSCROO";

void Task::CreateInput_AttrScores() {
	Database& db = Database::Single();
	PipePtrs& p = this->p;
	Pipe& pipe = *p.pipe;
	Attributes& g = pipe;
	AttrScore& as = pipe;
	SnapArg a = p.a;
	ASSERT(a.mode != MODE_INVALID);
	
	String type = args[0];
	String ai_txt = GetGroupContextNaturalDescription(p.a.ctx);
	
	// List of axes:
	// -a Integrity: +honest/-twisted
	TaskTitledList& axes = input.AddSub();
	axes			.Title("List of axes")
					.CountLinesAlpha();
	for (const Attr::ScoringType& t : g.attr_scorings)
		axes.Add(t.klass, "+" + t.axes0 + "/-" + t.axes1);
	
	
	//Combination string from results:
	// - a (Integrity: +honest/-twisted) b (Social: +libertarian/-authoritarian) c (Economic: +liberal/-conservative) etc.
	TaskTitledList& result_example = input.AddSub();
	result_example	.Title("Combination string from results")
					.CountLinesAlpha()
					.NoSeparator()
					.InlineList();
	for(int i = 0; i < 3 && i < g.attr_scorings.GetCount(); i++) {
		const Attr::ScoringType& t = g.attr_scorings[i];
		result_example.Add(t.klass, "+" + t.axes0 + "/-" + t.axes1);
	}
	
	
	// Instruction
	input.AddSub().NoColon().Title("Combination integer values are allowed to be between -3 and +3.");
	
	Vector<int> example_values;
	example_values		<< 1
						<< 1
						<< 0
						<< 1
						
						<< 1
						<< 1
						<< 0
						<< 0
						
						<< 1
						<< 1
						<< 1
						<< 1
						
						<< 1
						<< 1
						<< 1
						<< 1
						
						<< 1
						<< 0
						<< 0
						<< 0
						;
	
	// Scores per entry:
	// Line 1, Pronouns: "I"
	// -a Integrity: honest/twisted: +1 honest
	TaskTitledList& detailed = input.AddSub();
	detailed		.Title("Scores per entry")
					.CountSub().CountLines();
	TaskTitledList& ex1 = detailed.AddSub();
	ex1				.Title("Pronouns", "I")
					.CountLinesAlpha()
					.Separator(" ")
					.CombinationString();
	// Line 1, Pronouns: "I"
	// Combination string: a+1 b0 c0 d+1 e0 f+1 g0 h0 i+1 j+1 k+1 l+1 m+1 n0 o+1 p+1 q0 r0 s0 t0
	TaskTitledList& summary = input.AddSub();
	summary			.CountSub().CountLines();
	TaskTitledList& ex1s = summary.AddSub();
	ex1s			.Title("Pronouns", "I")
					.Separator(" ")
					.CombinationString();
	for(int i = 0; i < example_values.GetCount() && i < g.attr_scorings.GetCount(); i++) {
		const Attr::ScoringType& t = g.attr_scorings[i];
		int value = example_values[i];
		String line = "+" + t.axes0 + "/-" + t.axes1 + ": ";
		if      (value > 0) line << "+" << value << " " << t.axes0;
		else if (value > 0) line << "-" << value << " " << t.axes1;
		else line << "0";
		ex1.Add(t.klass, line);
		ex1.scores << value;
		ex1s.scores << value;
	}
	
	
	// Try making prompt with errors first
	Index<SnapAttrStr> attrs;
	pipe.GetMaskAttributes(a, attrs); // get attrs from masks
	
	
	// Entries
	TaskTitledList& entries = input.AddSub();
	entries		.Title("Entries")
				.CountLines()
				.LineBegin(2);
	TaskTitledList& results = input.PreAnswer();
	results		.Title("Lines with resulting combination strings")
				.CountLines()
				.LineBegin(2)
				.CombinationString();
	int entry_count = 0;
	for (const SnapAttrStr& a : attrs.GetKeys()) {
		ASSERT(a.has_id);
		
		// Skip attributes with known score values
		int score = as.attr_to_score[a.group_i][a.item_i];
		if (score >= 0)
			continue;
		
		// Skip groups, which doesn't match this task
		const Attr::Group& gg = g.attr_groups[a.group_i];
		if (gg.type != type)
			continue;
		
		// Skip different group context
		const Attr::GroupType& gt = g.group_types[gg.type_i];
		if (gt.group_ctx != p.a.ctx)
			continue;
		
		
		// Get attribute's name
		String key = gg.values[a.item_i];
		
		// Add line to ai prompt
		entries.Add(gg.description, key);
		
		// Make first entry prompt on first seen value
		if (!entry_count)
			results.Add(gg.description, key);
		
		entry_count++;
	}
	
	
	// This task shouldn't exist, if there is nothing to solve
	if (entry_count == 0) {
		SetFastExit();
		return;
	}
	
	input.response_length = 1024*3/2;
}

void Task::CreateInput_Lyrics() {
	Database& db = Database::Single();
	
	if (!this->p.part) {
		SetFatalError("no part pointers");
		return;
	}
	
	SnapArg a = p.a;
	ASSERT(a.mode != MODE_INVALID);
	PipePtrs& ptrs = this->p;
	
	
	input.AddSub()	.Title("Example structured lyrics")
					.NoListChar()
					.Add("part verse1{line(0:3) {line(0:1) {pronouns {i (m);}types of sentences {observations;}contrast and unexpected elements {exaggeration and surreal situations;}acting styles {funny;}tones {melancholic;}dramatic scenarios {tragic death;}voiceover tones {casual;}comedic sentences {satirical;}comedic scenarios {absurd and exaggerated scenarios;}humorous expressions {irony;}moral interactions {respect;}religiously moral {playful ia. with god;}interactions {social;}interactions with {man;}place {my bed (m);}place's visibility {is mentioned;}verbs {wish;}idea {a free spirited individual who is optimistic;playful;and joyful;yet realistic and accepting of imperfection.;}setting {world;}}line(1:2) {line(1:1) {pronouns {you (m);}types of sentences {statements;}contrast and unexpected elements {subversion of expectations;}acting styles {dramatic;}tones {pleading;}dramatic scenarios {a marriage in crisis;}voiceover tones {contrasting;}comedic sentences {pun;}comedic scenarios {physical comedy;}humorous expressions {playful wordplay;}moral interactions {honesty;}religiously moral {mutual antagonism ia.;}interactions {atheistic;}interactions with {woman;}place {my bed (f);}place's visibility {is said between the lines;}verbs {say;}idea {building strong relationships;}setting {space;}}line(2:1) {pronouns {i (m);}types of sentences {declarative;}contrast and unexpected elements {exaggeration and surreal situations;}moral interactions {respect;}religiously moral {playful ia. with god;}interactions {social;}acting styles {funny;}tones {melancholic;}dramatic scenarios {tragic death;}voiceover tones {casual;}comedic sentences {puns;}comedic scenarios {absurd and exaggerated scenarios;}humorous expressions {irony;}moral interactions mode {promote someone's;}place {my bed (m);}place's visibility {is mentioned;}verbs {wish;}idea {a free spirited individual who is optimistic;playful;and joyful;yet realistic and accepting of imperfection.;}setting {world;}}}}}");
	
	input.AddSub()	.Title("Example lyrics (from structured lyrics)")
					.NoListChar()
		.AddSub().Title("verse1").NoListChar()
					.Add("Scar tissue that I wish you saw")
					.Add("Sarcastic mister know-it-all")
					.Add("Close your eyes and I'll kiss you, 'cause with the birds I'll share (lonely view)");
	
	
	const Song& s = *ptrs.pipe->song;
	ASSERT(s.EditorPtrs::artist);
	const Artist& ar = *s.EditorPtrs::artist;
	const Release& r = *s.EditorPtrs::release;
	Part& p = *ptrs.part;
	
	ASSERT(a.mode >= 0);
	
	TaskTitledList& info = input.AddSub();
	info.Add("Artist name", ar.native_name);
	info.Add("Year of birth", ar.year_of_birth);
	info.Add("Year of beginning of the career", ar.year_of_career_begin);
	info.Add("Music genre", ar.musical_style);
	info.Add("Voice", ar.vibe_of_voice);
	info.Add("Vocalist visually", ar.vocalist_visual);
	info.Add("Title of lyrics", s.native_title);
	info.Add("Year", (int)r.date.year);
	/*info.Add("Meaning", s.meaning);
	info.Add("Literary devices", s.devices);
	info.Add("Emotion", s.emotion);
	info.Add("References", s.references);
	info.Add("Structure", s.structure);
	info.Add("History", s.history);
	info.Add("Storyline", s.storyline);
	info.Add("Implications", s.implications);
	info.Add("Title of music composition", c.title);
	info.Add("Year", c.year);
	info.Add("Tempo", c.tempo info.Add(" bpm\n"
	info.Add("Beat/Rhythm", c.beat);
	info.Add("Melody", c.melody);
	info.Add("Chord progression", c.chord_progression);
	info.Add("Key and mode", c.key_and_mode);
	info.Add("Texture", c.texture);
	info.Add("Structure", c.structure);
	info.Add("Genre/Style", c.genre_style);
	*/
	
	//LOG(p.GetStructuredText(mode, true));
	//LOG(p.GetStructuredText(mode, false));
	
	TaskTitledList& lyrics = input.AddSub();
	lyrics.Title("Structured lyrics");
	lyrics.Add(p.GetStructuredText(a, false));
	
	TaskTitledList& result = input.AddSub();
	result.Title("Lyrics");
}

void Task::CreateInput_LyricsTranslate() {
	bool rev_snap = args[0] == "rev";
	SnapArg a = p.a;
	ASSERT(a.mode != MODE_INVALID);
	Pipe& pipe = *p.pipe;
	
	String lng = args[1].Left(5);
	if (lng == LNGAsText(LNG_('F','I','F','I')))
		lng = "Finnish";
	String key = "gen.lyrics";
	
	TaskTitledList& in_english = input.AddSub().Title("In English");
	
	for (Part& part : pipe.parts) {
		TaskTitledList& lyrics = in_english.AddSub();
		lyrics.Title(part.name);
		lyrics.NoListChar();
		lyrics.Add(part.Get(a).data.GetAdd(key));
	}
	
	TaskTitledList& in_trans = input.AddSub().Title("In " + lng);
	
	// Done!
}

void Task::CreateInput_TranslateSongData() {
	String orig_lng = args[0];
	String orig_key = args[1];
	String trans_lng = args[2];
	String trans_key = args[3];
	
	Song& song = *p.pipe->song;
	String orig_txt = song.data.Get(orig_key, "");
	orig_txt.Replace("\r", "");
	Vector<String> lines = Split(orig_txt, "\n", false);
	
	TaskTitledList& in_orig = input.AddSub().Title("Text 1 in " + orig_lng);
	in_orig		.NoListChar();
	for (const String& line : lines)
		in_orig		.Add(line);
	
	TaskTitledList& results = input.PreAnswer();
	results		.Title("Text 1 in " + trans_lng);
	
	input.response_length = 1024*2;
}


const char* struct_unpack_orig = R"_(
chorus:
I didn't want to fight, I don't want to break up
I never imagined this would end.
I didn't mean to break you, make us like this
but no matter how much I love, one of us hurts the other again.)_";


const char* struct_unpack_trans = R"_(
chorus {
	me {
		not wanting {
			to fight
			to break up
		}
		never ( imagined ) {
			this ( end )
		}
		not meaning {
			to break ( my romantic partner )
			to make ( me and my romantic partner ) {
				like ( this )
			}
		}
	}
	me and my romantic partner {
		loving {
			one ( of me and my romantic partner ) {
				hurting ( other ) {
					again
				}
			}
		}
	}
})_";



void Task::CreateInput_UnpackStructureSongData() {
	String orig_key = args[0];
	String struct_key = args[1];
	
	{
		Vector<String> lines = Split(struct_unpack_orig, "\n", true);
		TaskTitledList& list = input.AddSub().Title("Original lyrics");
		list		.NoListChar();
		for (const String& line : lines)
			list		.Add(line);
	}
	{
		Vector<String> lines = Split(struct_unpack_trans, "\n", true);
		TaskTitledList& list = input.AddSub().Title("Structurally deconstructed lyrics");
		list		.NoListChar();
		for (const String& line : lines)
			list		.Add(line);
	}
	
	{
		Song& song = *p.pipe->song;
		String orig_txt = song.data.Get(orig_key, "");
		orig_txt.Replace("\r", "");
		Vector<String> lines = Split(orig_txt, "\n", true);
		TaskTitledList& list = input.AddSub().Title("Original lyrics");
		list		.NoListChar();
		for (const String& line : lines)
			list		.Add(line);
	}
	
	TaskTitledList& results = input.PreAnswer();
	results		.Title("Structurally deconstructed lyrics");
	
	input.response_length = 1024*2;
}

const char* check_song_struct_errors_ex = R"_(
me {
	not wanting {
		to fight
		to break up
	}
})_";

void Task::CreateInput_CheckSongStructureErrors() {
	String main_key = args[0];
	
	{
		Vector<String> lines = Split(struct_unpack_orig, "\n", true);
		TaskTitledList& list = input.AddSub().Title("Original lyrics \"Text A\"");
		list		.NoListChar();
		list		.Add("I didn't want to fight, I don't want to break up");
	}
	{
		Vector<String> lines = Split(check_song_struct_errors_ex, "\n", true);
		TaskTitledList& list = input.AddSub().Title("Structurally deconstructed lyrics \"Text A\"");
		list		.NoListChar();
		for (const String& line : lines)
			list		.Add(line);
	}
	
	{
		Song& song = *p.pipe->song;
		String orig_txt = song.data.Get(main_key, "");
		orig_txt.Replace("\r", "");
		Vector<String> lines = Split(orig_txt, "\n", true);
		TaskTitledList& list = input.AddSub()
			// SOMEHOW THE CORRECT TITLE GIVES WORSE RESULTS!
				//.Title("Structurally deconstructed lyrics \"Text B\"");
			.Title("Original lyrics \"Text B\"");
		list		.NoListChar();
		for (const String& line : lines)
			list		.Add(line);
	}
	
	int list_len = 0;
	{
		TaskTitledList& list = input.AddSub().Title("rate 0-5");
		//list		.NoListChar()
		//			.InlineList();
		list		.NumberedLines()
					//.NoNumberedLinesSpace() // Somehow this improves the result SIGNIFICANTLY
					;
		list		.Add("lyrical fumbling")
					.Add("confusion")
					.Add("unnecessary words")
					.Add("looseness")
					.Add("pronoun incorrectness")
					.Add("unnecessary adjectives")
					.Add("overuse of verbs")
					.Add("repetition")
					.Add("overdramatic")
					.Add("awkwardness")
					.Add("syntax errors")
					.Add("odd phrasing")
					.Add("poor transitions")
					.Add("long-windedness")
					.Add("abrupt endings")
					.Add("redundancy")
					.Add("choppiness")
					.Add("lack of coherence")
					.Add("thing cliche")
					.Add("unclear")
					.Add("misused words")
					.Add("unnatural")
					;
		list_len = list.values.GetCount();
	}
	
	{
		TaskTitledList& list = input.AddSub().Title("Example answer");
		list	.CountSub()
				.NumberedLines();
		
		TaskTitledList& first = list.AddSub();
		first.Title("Lines with lyrical fumbling (and improvements) in \"Text C\"");
		
		first	.Add("\"for ( look )\" -> \"for a ( look )\"")
				.Add("\"other bad 1\" -> \"other better 1\"")
				.Add("\"other bad 2\" -> \"other better 2\"")
				.Add("etc")
				;
	}
	
	if (0) {
		TaskTitledList& results = input.PreAnswer();
		results		.Title("Results in \"Text B\"")
					.CountLines();
		TaskTitledList& first = results.AddSub();
		first		.Title("Lines with 'lyrical fumbling' (and improvements)");
		first		.EmptyLine();
	}
	else if (0) {
		TaskTitledList& results = input.PreAnswer();
		results		.Title("1. Lines with lyrical fumbling (and improvements) in \"Text B\"");
		results		.EmptyLine();
	}
	else {
		TaskTitledList& results = input.PreAnswer();
		results.Title("All 1-" + IntStr(list_len));
		
		TaskTitledList& first = results.AddSub();
		first		.Title("1. Lines with lyrical fumbling (and improvements) in \"Text B\"");
		first		.EmptyLine();
	}
	
	input.response_length = 1024*2;
}

void Task::CreateInput_ConvertSongStructureToEnglish() {
	String src_key = args[0];
	String dst_key = args[1];
	
	{
		Vector<String> lines = Split(struct_unpack_trans, "\n", true);
		TaskTitledList& list = input.AddSub().Title("Structurally deconstructed lyrics");
		list		.NoListChar();
		for (const String& line : lines)
			list		.Add(line);
	}
	{
		Vector<String> lines = Split(struct_unpack_orig, "\n", true);
		TaskTitledList& list = input.AddSub().Title("Original lyrics");
		list		.NoListChar();
		for (const String& line : lines)
			list		.Add(line);
	}
	
	{
		Song& song = *p.pipe->song;
		String orig_txt = song.data.Get(src_key, "");
		orig_txt.Replace("\r", "");
		Vector<String> lines = Split(orig_txt, "\n", true);
		TaskTitledList& list = input.AddSub().Title("Structurally deconstructed lyrics");
		list		.NoListChar();
		for (const String& line : lines)
			list		.Add(line);
	}
	
	TaskTitledList& results = input.PreAnswer();
	results		.Title("Original lyrics");
	
	input.response_length = 1024*2;
}

void Task::CreateInput_EvaluateSongAudience() {
	String src_key = args[0];
	String dst_key = args[1];
	
	int person_count = args.GetCount()-2;
	String first_person = args[2];
	
	{
		Song& song = *p.pipe->song;
		String orig_txt = song.data.Get(src_key, "");
		orig_txt.Replace("\r", "");
		Vector<String> lines = Split(orig_txt, "\n", true);
		TaskTitledList& list = input.AddSub().Title("Lyrics");
		list		.NoListChar();
		for (const String& line : lines)
			list		.Add(line);
	}
	{
		TaskTitledList& list = input.AddSub().Title("Persons");
		list.NumberedLines();
		for(int i = 2; i < args.GetCount(); i++) {
			list.Add(args[i]);
		}
	}
	{
		
		TaskTitledList& results = input.PreAnswer();
		results.Title("All 1-" + IntStr(person_count));
		
		TaskTitledList& first = results.AddSub();
		//first		.Title("1. What kind of 'reaction' would " + first_person + " get and what would she/he 'think' of the song");
		//first.Add("reaction:");
		first		.Title("1. What " + first_person + " would think while hearing the song with these lyrics. Only about lyrics");
		first		.EmptyLine();
	}
	
	input.response_length = 1024*2;
}
