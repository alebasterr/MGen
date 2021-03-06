#include "stdafx.h"
#include "GVar.h"


CGVar::CGVar()
{
	// Init constant length arrays
	instr.resize(MAX_VOICE);
	instr_type.resize(MAX_INSTR);
	instr_channel.resize(MAX_INSTR);
	instr_nmin.resize(MAX_INSTR);
	instr_nmax.resize(MAX_INSTR);
	instr_tmin.resize(MAX_INSTR);
	instr_tmax.resize(MAX_INSTR);
	CC_steps.resize(MAX_INSTR);
	CC_dyn.resize(MAX_INSTR);
	CC_dyn_ma.resize(MAX_INSTR);
	CC_retrigger.resize(MAX_INSTR);
	retrigger_freq.resize(MAX_INSTR);
	max_slur_count.resize(MAX_INSTR);
	max_slur_interval.resize(MAX_INSTR);
	slur_ks.resize(MAX_INSTR);
	legato_ahead.resize(MAX_INSTR);
	nonlegato_freq.resize(MAX_INSTR);
	nonlegato_minlen.resize(MAX_INSTR);
	lengroup2.resize(MAX_INSTR);
	lengroup3.resize(MAX_INSTR);
	lengroup4.resize(MAX_INSTR);
	lengroup_edt1.resize(MAX_INSTR);
	lengroup_edt2.resize(MAX_INSTR);
	rand_start.resize(MAX_INSTR);
	rand_end.resize(MAX_INSTR);
	retrigger_min_len.resize(MAX_INSTR);
	retrigger_rand_end.resize(MAX_INSTR);
	retrigger_rand_max.resize(MAX_INSTR);
	vel_harsh.resize(MAX_INSTR);
	vel_immediate.resize(MAX_INSTR);
	vel_normal.resize(MAX_INSTR);
	vel_gliss.resize(MAX_INSTR);
	vel_normal_minlen.resize(MAX_INSTR);
	gliss_minlen.resize(MAX_INSTR);
	gliss_freq.resize(MAX_INSTR);
	rand_start_max.resize(MAX_INSTR);
	rand_end_max.resize(MAX_INSTR);
	max_ahead_note.resize(MAX_INSTR);
	// Set instrument
	instr[0] = 5;
	instr[1] = 6;
}

CGVar::~CGVar()
{
}

void CGVar::InitVectors()
{
	// Create vectors
	pause = vector<vector<unsigned char>>(t_allocated, vector<unsigned char>(v_cnt));
	note = vector<vector<unsigned char>>(t_allocated, vector<unsigned char>(v_cnt));
	len = vector<vector<unsigned char>>(t_allocated, vector<unsigned char>(v_cnt));
	coff = vector<vector<unsigned char>>(t_allocated, vector<unsigned char>(v_cnt));
	poff = vector<vector<unsigned char>>(t_allocated, vector<unsigned char>(v_cnt));
	noff = vector<vector<unsigned char>>(t_allocated, vector<unsigned char>(v_cnt));
	dyn = vector<vector<unsigned char>>(t_allocated, vector<unsigned char>(v_cnt));
	vel = vector<vector<unsigned char>>(t_allocated, vector<unsigned char>(v_cnt));
	artic = vector<vector<unsigned char>>(t_allocated, vector<unsigned char>(v_cnt));
	lengroup = vector<vector<char>>(t_allocated, vector<char>(v_cnt));
	comment = vector<vector<CString>>(t_allocated, vector<CString>(v_cnt));
	adapt_comment = vector<vector<CString>>(t_allocated, vector<CString>(v_cnt));
	color = vector<vector<Color>>(t_allocated, vector<Color>(v_cnt));
	tempo = vector<double>(t_allocated);
	stime = vector<double>(t_allocated);
	etime = vector<double>(t_allocated);
	dstime = vector<vector<double>>(t_allocated, vector<double>(v_cnt));
	detime = vector<vector<double>>(t_allocated, vector<double>(v_cnt));
	// Init ngv
	for (int v = 0; v < MAX_VOICE; v++) {
		ngv_min[v] = 1000;
		ngv_max[v] = 0;
	}
	// Init color
	for (int i = 0; i < t_allocated; i++) {
		for (int v = 0; v < v_cnt; v++) {
			color[i][v] = Color(0);
		}
	}
}

void CGVar::ResizeVectors(int size, int vsize)
{
	milliseconds time_start = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
	if (!mutex_output.try_lock_for(chrono::milliseconds(5000))) {
		WriteLog(1, new CString("Critical error: ResizeVectors mutex timed out"));
	}
	if (vsize == -1) vsize = v_cnt;
	pause.resize(size);
	note.resize(size);
	len.resize(size);
	coff.resize(size);
	poff.resize(size);
	noff.resize(size);
	tempo.resize(size);
	stime.resize(size);
	etime.resize(size);
	dstime.resize(size);
	detime.resize(size);
	dyn.resize(size);
	vel.resize(size);
	artic.resize(size);
	lengroup.resize(size);
	comment.resize(size);
	adapt_comment.resize(size);
	color.resize(size);
	int start = t_allocated;
	if (vsize != v_cnt) start = 0;
	for (int i = start; i < size; i++) {
		pause[i].resize(vsize);
		note[i].resize(vsize);
		len[i].resize(vsize);
		coff[i].resize(vsize);
		poff[i].resize(vsize);
		noff[i].resize(vsize);
		dyn[i].resize(vsize);
		vel[i].resize(vsize);
		artic[i].resize(vsize);
		lengroup[i].resize(vsize);
		comment[i].resize(vsize);
		adapt_comment[i].resize(vsize);
		dstime[i].resize(vsize);
		detime[i].resize(vsize);
		color[i].resize(vsize, Color(0));
	}
	// Count time
	milliseconds time_stop = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
	CString* st = new CString;
	st->Format("ResizeVectors from %d to %d steps, from %d to %d voices (in %d ms)", t_allocated, size, v_cnt, vsize, time_stop - time_start);
	WriteLog(0, st);

	t_allocated = size;
	v_cnt = vsize;
	mutex_output.unlock();
}

void CGVar::LoadConfig(CString fname)
{
	CString st, st2, st3;
	ifstream fs;
	// Check file exists
	if (!CGVar::fileExists(fname)) {
		CString* est = new CString;
		est->Format("LoadConfig cannot find file: %s", fname);
		WriteLog(1, est);
		return;
	}
	fs.open(fname);
	char pch[2550];
	int pos = 0;
	int i = 0;
	while (fs.good()) {
		i++;
		// Get line
		fs.getline(pch, 2550);
		st = pch;
		// Remove unneeded
		pos = st.Find("#");
		if (pos != -1) st = st.Left(pos);
		st.Trim();
		pos = st.Find("=");
		if (pos != -1) {
			// Get variable name and value
			st2 = st.Left(pos);
			st3 = st.Mid(pos + 1);
			st2.Trim();
			st3.Trim();
			st2.MakeLower();
			// Load general variables
			int idata = atoi(st2);
			double fdata = atof(st3);
			CheckVar(&st2, &st3, "v_cnt", &v_cnt);
			CheckVar(&st2, &st3, "t_cnt", &t_cnt);
			CheckVar(&st2, &st3, "t_allocated", &t_allocated);
			CheckVar(&st2, &st3, "t_send", &t_send);
			CheckVar(&st2, &st3, "midifile_tpq_mul", &midifile_tpq_mul);
			CheckVar(&st2, &st3, "sleep_ms", &sleep_ms);
			CheckVar(&st2, &st3, "adapt_enable", &adapt_enable);
			CheckVar(&st2, &st3, "comment_adapt", &comment_adapt);
			LoadVarInstr(&st2, &st3, "instruments", instr);
			// Load algorithm-specific variables
			LoadConfigLine(&st2, &st3, idata, fdata);
		}
	}
	fs.close();
	CString* est = new CString;
	est->Format("LoadConfig loaded %d lines from %s", i, fname);
	WriteLog(0, est);
}

void CGVar::LoadInstruments()
{
	// Load strings
	ifstream fs;
	fs.open("instruments.pl");
	CString st, st2, st3;
	char pch[2550];
	int pos = 0;
	int i = -1, x = 0;
	while (fs.good()) {
		x++;
		fs.getline(pch, 2550);
		st = pch;
		// Remove comments
		pos = st.Find("#");
		if (pos != -1) st = st.Left(pos);
		st.Trim();
		// Find equals
		pos = st.Find("=");
		if (pos != -1) {
			st2 = st.Left(pos);
			st3 = st.Mid(pos + 1);
			st2.Trim();
			st3.Trim();
			st2.MakeLower();
			int idata = atoi(st3);
			if (st2 == "instrument") {
				for (int x = 0; x<MAX_INSTR; x++) {
					if (InstName[x] == st3) i = x;
				}
			}
			LoadVarInstr(&st2, &st3, "instruments", instr);
			if (i > -1) {
				LoadNote(&st2, &st3, "n_min", &instr_nmin[i]);
				LoadNote(&st2, &st3, "n_max", &instr_nmax[i]);
				CheckVar(&st2, &st3, "t_min", &instr_tmin[i]);
				CheckVar(&st2, &st3, "t_max", &instr_tmax[i]);
				CheckVar(&st2, &st3, "type", &instr_type[i]);
				CheckVar(&st2, &st3, "channel", &instr_channel[i]);
				CheckVar(&st2, &st3, "cc_steps", &CC_steps[i]);
				CheckVar(&st2, &st3, "cc_dynamics", &CC_dyn[i]);
				CheckVar(&st2, &st3, "cc_dyn_ma", &CC_dyn_ma[i]);
				CheckVar(&st2, &st3, "cc_retrigger", &CC_retrigger[i]);
				CheckVar(&st2, &st3, "retrigger_freq", &retrigger_freq[i]);
				CheckVar(&st2, &st3, "max_slur_count", &max_slur_count[i]);
				CheckVar(&st2, &st3, "max_slur_interval", &max_slur_interval[i]);
				CheckVar(&st2, &st3, "slur_ks", &slur_ks[i]);
				CheckVar(&st2, &st3, "legato_ahead", &legato_ahead[i]);
				CheckVar(&st2, &st3, "nonlegato_minlen", &nonlegato_minlen[i]);
				CheckVar(&st2, &st3, "nonlegato_freq", &nonlegato_freq[i]);
				CheckVar(&st2, &st3, "lengroup2", &lengroup2[i]);
				CheckVar(&st2, &st3, "lengroup3", &lengroup3[i]);
				CheckVar(&st2, &st3, "lengroup4", &lengroup4[i]);
				CheckVar(&st2, &st3, "lengroup_edt1", &lengroup_edt1[i]);
				CheckVar(&st2, &st3, "lengroup_edt2", &lengroup_edt2[i]);
				CheckVar(&st2, &st3, "rand_start", &rand_start[i]);
				CheckVar(&st2, &st3, "rand_end", &rand_end[i]);
				CheckVar(&st2, &st3, "retrigger_min_len", &retrigger_min_len[i]);
				CheckVar(&st2, &st3, "retrigger_rand_end", &retrigger_rand_end[i]);
				CheckVar(&st2, &st3, "retrigger_rand_max", &retrigger_rand_max[i]);
				CheckVar(&st2, &st3, "vel_harsh", &vel_harsh[i]);
				CheckVar(&st2, &st3, "vel_immediate", &vel_immediate[i]);
				CheckVar(&st2, &st3, "vel_normal", &vel_normal[i]);
				CheckVar(&st2, &st3, "vel_gliss", &vel_gliss[i]);
				CheckVar(&st2, &st3, "vel_normal_minlen", &vel_normal_minlen[i]);
				CheckVar(&st2, &st3, "gliss_minlen", &gliss_minlen[i]);
				CheckVar(&st2, &st3, "gliss_freq", &gliss_freq[i]);
				CheckVar(&st2, &st3, "rand_start_max", &rand_start_max[i]);
				CheckVar(&st2, &st3, "rand_end_max", &rand_end_max[i]);
				CheckVar(&st2, &st3, "max_ahead_note", &max_ahead_note[i]);
				//CGVar::LoadVar(&st2, &st3, "save_format_version", &save_format_version);
			}
		}
	}
	st2 = "instruments";
	// After loading global mapping of voices to instruments, load algorithm-specific mapping
	LoadVarInstr(&st2, &m_algo_insts, "instruments", instr);
	fs.close();
	CString* est = new CString;
	est->Format("LoadInstruments loaded %d lines from instruments.pl", x);
	WriteLog(0, est);
}

void CGVar::SaveVector2C(ofstream & fs, vector< vector<unsigned char> > &v2D, int i) {
	const char* pointer = reinterpret_cast<const char*>(&v2D[i][0]);
	size_t bytes = v_cnt * sizeof(v2D[i][0]);
	fs.write(pointer, bytes);
}

void CGVar::SaveVector2Color(ofstream & fs, vector< vector<Color> > &v2D, int i) {
	size_t bytes = 4;
	for (int v = 0; v < v_cnt; v++) {
		DWORD color = v2D[i][v].GetValue();
		fs.write((char*)&color, bytes);
	}
}

void CGVar::SaveVector2ST(ofstream & fs, vector< vector<CString> > &v2D, int i) {
	for (int v = 0; v < v_cnt; v++) {
		unsigned short len = v2D[i][v].GetLength();
		fs.write((char*)&len, sizeof(len));
		if (len != 0) fs.write(v2D[i][v].GetBuffer(), len);
	}
}

void CGVar::SaveVectorD(ofstream &fs, vector<double> &v) {
	const char* pointer = reinterpret_cast<const char*>(&v[0]);
	size_t bytes = t_generated * sizeof(v[0]);
	fs.write(pointer, bytes);
}

void CGVar::SaveResults(CString dir, CString fname)
{
	milliseconds time_start = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
	CreateDirectory(dir, NULL);
	ofstream fs;
	fs.open(dir + "\\" + fname + ".mgr", std::ofstream::binary);
	for (size_t i = 0; i < t_generated; i++)
	{
		SaveVector2C(fs, pause, i);
		SaveVector2C(fs, note, i);
		SaveVector2C(fs, len, i);
		SaveVector2C(fs, coff, i);
		SaveVector2C(fs, dyn, i);
		SaveVector2ST(fs, comment, i);
		SaveVector2Color(fs, color, i);
	}
	SaveVectorD(fs, tempo);
	fs.close();
	// Save strings
	CString st;
	fs.open(dir + "\\" + fname + ".txt");
	fs << "save_format_version = 1.2 # This is version of format used to save these files\n";
	fs << "m_config = " + m_config + " # Name of config file used for generation\n";
	st.Format("m_algo_id = %d\n", m_algo_id);
	fs << st;
	st.Format("t_cnt = %d # Number of steps I had to generate\n", t_cnt);
	fs << st;
	st.Format("v_cnt = %d # Number of voices\n", v_cnt);
	fs << st;
	st.Format("t_generated = %d # Number of steps I generated\n", t_generated);
	fs << st;
	st.Format("t_sent = %d # Number of steps I finished and sent (can be less than generated)\n", t_sent);
	fs << st;
	st.Format("t_send = %d # Send window\n", t_send);
	fs << st;
	st.Format("ng_min = %d # Minimum generated note\n", ng_min);
	fs << st;
	st.Format("ng_max = %d # Maximum generated note\n", ng_max);
	fs << st;
	st.Format("tg_min = %f # Minimum generated tempo\n", tg_min);
	fs << st;
	st.Format("tg_max = %f # Maximum generated tempo\n", tg_max);
	fs << st;
	st.Format("time_started = %d # Unix timestamp of generation start\n", time_started);
	fs << st;
	st.Format("time_stopped = %d # Unix timestamp of generation finish\n", time_stopped);
	fs << st;
	st.Format("need_exit = %d\n", need_exit);
	fs << st;
	fs.close();
	// Count time
	milliseconds time_stop = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
	CString* est = new CString;
	est->Format("Saved results to file in %d ms", time_stop - time_start);
	WriteLog(0, est);
}

void CGVar::LoadVector2C(ifstream& fs, vector< vector<unsigned char> > &v2D, int i) {
	v2D[i].resize(v_cnt);
	char* pointer = reinterpret_cast<char*>(&(v2D[i][0]));
	size_t bytes = v_cnt * sizeof(v2D[i][0]);
	fs.read(pointer, bytes);
}

void CGVar::LoadVector2Color(ifstream & fs, vector< vector<Color> > &v2D, int i) {
	size_t bytes = 4;
	for (int v = 0; v < v_cnt; v++) {
		DWORD color;
		fs.read((char*)&color, bytes);
		v2D[i][v].SetValue(color);
	}
}

void CGVar::LoadVector2ST(ifstream & fs, vector< vector<CString> > &v2D, int i) {
	for (int v = 0; v < v_cnt; v++) {
		unsigned short len;
		char buf[5000];
		fs.read((char*)&len, sizeof(len));
		if (len != 0) {
			fs.read((char*)&buf, len);
			v2D[i][v] = buf;
		}
	}
}

void CGVar::LoadVectorD(ifstream &fs, vector<double> &v) {
	v.clear();
	v.resize(t_generated);
	char* pointer = reinterpret_cast<char*>(&v[0]);
	size_t bytes = t_generated * sizeof(v[0]);
	fs.read(pointer, bytes);
}

void CGVar::LoadResults(CString dir, CString fname)
{
	milliseconds time_start = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
	// Load strings
	ifstream fs;
	fs.open(dir + "\\" + fname + ".txt");
	CString st, st2, st3;
	char pch[2550];
	int pos = 0;
	while (fs.good()) {
		fs.getline(pch, 2550);
		st = pch;
		pos = st.Find("#");
		if (pos != -1) st = st.Left(pos);
		st.Trim();
		pos = st.Find("=");
		if (pos != -1) {
			st2 = st.Left(pos);
			st3 = st.Mid(pos + 1);
			st2.Trim();
			st3.Trim();
			st2.MakeLower();
			int idata = atoi(st3);
			CGVar::CheckVar(&st2, &st3, "t_cnt", &t_cnt);
			CGVar::CheckVar(&st2, &st3, "m_algo_id", &m_algo_id);
			CGVar::CheckVar(&st2, &st3, "v_cnt", &v_cnt);
			CGVar::CheckVar(&st2, &st3, "t_generated", &t_generated);
			CGVar::CheckVar(&st2, &st3, "t_sent", &t_sent);
			CGVar::CheckVar(&st2, &st3, "t_send", &t_send);
			CGVar::CheckVar(&st2, &st3, "need_exit", &need_exit);
			CGVar::CheckVar(&st2, &st3, "time_started", &time_started);
			CGVar::CheckVar(&st2, &st3, "time_stopped", &time_stopped);
			CGVar::LoadVar(&st2, &st3, "m_config", &m_config);
			CGVar::LoadVar(&st2, &st3, "save_format_version", &save_format_version);
		}
	}
	fs.close();
	// Allocate
	t_allocated = t_generated;
	InitVectors();
	// Load binary
	fs.open(dir + "\\" + fname + ".mgr", std::ofstream::binary);
	fs.unsetf(std::ios::skipws);
	if (t_generated != 0)
		for (size_t i = 0; i < t_generated; i++)
		{
			LoadVector2C(fs, pause, i);
			LoadVector2C(fs, note, i);
			LoadVector2C(fs, len, i);
			LoadVector2C(fs, coff, i);
			LoadVector2C(fs, dyn, i);
			LoadVector2ST(fs, comment, i);
			LoadVector2Color(fs, color, i);
		}
	LoadVectorD(fs, tempo);
	CountOff(0, t_generated - 1);
	CountTime(0, t_generated - 1);
	UpdateNoteMinMax(0, t_generated - 1);
	UpdateTempoMinMax(0, t_generated - 1);
	fs.close();
	// Count time
	milliseconds time_stop = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
	CString* est = new CString;
	est->Format("Loaded results from files in %d ms", time_stop - time_start);
	WriteLog(0, est);
}

void CGVar::CountOff(int step1, int step2)
{
	for (int i = step1; i <= step2; i++) {
		for (int v = 0; v < v_cnt; v++) {
			noff[i][v] = len[i][v] - coff[i][v];
			if (i - coff[i][v] - 1 >= 0) poff[i][v] = len[i - coff[i][v] - 1][v] - coff[i][v];
			else poff[i][v] = 0;
		}
	}
}

void CGVar::CountTime(int step1, int step2)
{
	for (int i = step1; i <= step2; i++) {
		if (i > 0) stime[i] = stime[i - 1] + 30000.0 / tempo[i - 1];
		else stime[i] = 0;
		etime[i] = stime[i] + 30000.0 / tempo[i];
	}
}

void CGVar::CopyVoice(int v1, int v2, int step1, int step2, int interval)
{
	for (int i = step1; i <= step2; i++) {
		pause[i][v2] = pause[i][v1];
		if (!pause[i][v1]) note[i][v2] = note[i][v1] + interval;
		len[i][v2] = len[i][v1];
		dyn[i][v2] = dyn[i][v1];
		coff[i][v2] = coff[i][v1];
		poff[i][v2] = poff[i][v1];
		noff[i][v2] = noff[i][v1];
	}
}

void CGVar::UpdateNoteMinMax(int step1, int step2)
{
	for (int i = step1; i <= step2; i++) {
		for (int v = 0; v < v_cnt; v++) if ((pause[i][v] == 0) && (note[i][v] != 0)) {
			if (ng_min > note[i][v]) ng_min = note[i][v];
			if (ng_max < note[i][v]) ng_max = note[i][v];
			if (ngv_min[v] > note[i][v]) ngv_min[v] = note[i][v];
			if (ngv_max[v] < note[i][v]) ngv_max[v] = note[i][v];
		}
	}
}

void CGVar::UpdateTempoMinMax(int step1, int step2)
{
	for (int i = step1; i <= step2; i++) {
		if (tg_min > tempo[i]) tg_min = tempo[i];
		if (tg_max < tempo[i]) tg_max = tempo[i];
	}
}

