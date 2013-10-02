#include "bschordpro.h"

using namespace bschordpro;

static const wchar_t *cmdTitle 				= L"title";
static const wchar_t *cmdTitleShort 		= L"t";
static const wchar_t *cmdSubtitle			= L"subtitle";
static const wchar_t *cmdSubtitleShort		= L"su";
static const wchar_t *cmdChorusStart		= L"start_of_chorus";
static const wchar_t *cmdChorusStartShort	= L"soc";
static const wchar_t *cmdChorusEnd			= L"end_of_chorus";
static const wchar_t *cmdChorusEndShort		= L"eoc";
static const wchar_t *cmdTabStart			= L"start_of_tab";
static const wchar_t *cmdTabStartShort		= L"sot";
static const wchar_t *cmdTabEnd				= L"end_of_tab";
static const wchar_t *cmdTabEndShort		= L"eot";
static const wchar_t *cmdStructStart		= L"start_of_struct";
static const wchar_t *cmdStructStartShort	= L"sos";
static const wchar_t *cmdStructEnd			= L"end_of_struct";
static const wchar_t *cmdStructEndShort		= L"eos";
static const wchar_t *cmdColumns			= L"columns";
static const wchar_t *cmdColumnsShort		= L"col";
static const wchar_t *cmdNewSong			= L"new_song";
static const wchar_t *cmdNewSongShort		= L"ns";
static const wchar_t *cmdComment			= L"comment";
static const wchar_t *cmdCommentShort		= L"c";

/*

End of lines
-------------

LF (Line feed, '\n', 0x0A, 10 in decimal)
CR (Carriage return, '\r', 0x0D, 13 in decimal)

LF: Multics, Unix and Unix-like systems (GNU/Linux, Mac OS X, FreeBSD, AIX, Xenix, etc.), BeOS, Amiga, RISC OS and others.
CR+LF: Microsoft Windows, DEC TOPS-10, RT-11 and most other early non-Unix and non-IBM OSes, CP/M, MP/M, DOS (MS-DOS, PC-DOS, etc.), Atari TOS, OS/2, Symbian OS, Palm OS

*/

EventHandlerTxt::EventHandlerTxt() { }

void EventHandlerTxt::onLineBegin()
{
    m_chordBuffer.erase();
    m_textBuffer.erase();
}

void EventHandlerTxt::onLineEnd()
{
    if (m_chordBuffer.find_first_not_of(L' ') != std::wstring::npos)
        std::wcout << std::endl << m_chordBuffer << std::endl;

    std::wcout << m_textBuffer << std::endl;
}

void EventHandlerTxt::onCommand(const CommandType command, const std::wstring& value, const RawPos &pos)
{
    std::wcout << L"command: " << command << L" with value: " << value;
}

void EventHandlerTxt::onChord(const std::wstring& chord, const RawPos &pos)
{
    m_chordBuffer.append(chord);
    m_chordBuffer.append(L" ");
}

void EventHandlerTxt::onText(const std::wstring& text, const RawPos &pos)
{
    m_textBuffer.append(text);

    int diff = m_textBuffer.length() - m_chordBuffer.length();

    if (diff > 0)
        m_chordBuffer.append(diff, L' ');
    else
    {
        m_textBuffer.append(-1 * diff, L' ');
    }
}

void EventHandlerTxt::onLine(const std::wstring& line, const RawPos &pos)
{
    m_textBuffer.append(line);
}

//////////////////////////////////////////////////////////////////////////////
void Parser::parse(const std::wstring& s)
{
	unsigned int lineFrom = 0;
	unsigned int lineLen = 0;

	if (m_eventHandler == NULL)
		return;

	m_eventHandler->onBegin();

    // loop over all characters of parsed std::wstring (buffer) and look for line ends
    // each text line is parsed separately
    size_t i = 0;
	while (i < s.length())
	{
		switch(s[i])
		{
			case L'\r':
				// CR is skipped (removed from line parsing)
				break;

			case L'\n':
				// LF always ends line and forces parser to parse preceeding line
				parseLine(s, lineFrom, lineLen, 0);
				lineFrom = i + 1;
				lineLen = 0;
				break;

			default:
				// shift position of last (non separator) character
				lineLen++;
		}
		i++;
	}

	// consume last line
	if (lineFrom < s.length())
		parseLine(s, lineFrom, lineLen, 0);

	m_eventHandler->onEnd();
}

// called for each text line of parsed buffer
void Parser::parseLine(const std::wstring& strBuffer, unsigned int lineFrom, unsigned int lineLen, unsigned int lineIndex)
{
	bool lineHasCmd = false;
	std::wstring curText = L"";
	std::wstring line;
	RawPos p;

	line.assign(strBuffer, lineFrom, lineLen);

    //std::wcout << L"from: " << lineFrom << L", len:" << lineLen << L" >" << line << L"<" << std::endl;

	// remove commands from line buffer
	while (true)
	{
		size_t cmdBegin = line.find_first_of(L'{');
		if (cmdBegin == std::wstring::npos)
			break;

		size_t cmdEnd = line.find_first_of(L'}', cmdBegin);

		if (cmdEnd == std::wstring::npos)
			break;

		// command found
		//std::wcout << L"Command found from: " << cmdBegin << " to: " << cmdEnd << std::endl;
		p.setPos(lineFrom + cmdBegin + 1, cmdEnd - cmdBegin - 1);
		parseCommand(strBuffer, p);
		line.erase(cmdBegin, cmdEnd - cmdBegin + 1);
		lineHasCmd = true;
	}

	// if line has command, don't parse it for text, chords, etc...
	if (lineHasCmd)
		return;

    //wcout << L"Line to be parsed is: >" << line << L"< (" << line.length() << L")" << endl;

	// use all line character when parser is in raw mode
	if (m_rawMode)
	{
		m_eventHandler->onLineBegin();
		p.setPos(lineFrom, lineLen);
		m_eventHandler->onLine(line, p);
		m_eventHandler->onLineEnd();
		return;
	}

	// don't spend time parsing empty lines
	if (line.length() == 0)
	{
		if (!lineHasCmd)
		{
			m_eventHandler->onLineBegin();
			m_eventHandler->onLineEnd();
		}
		return;
	}

    // skip comments
    if (line.find_first_not_of(L' ') == L'#')
        return;

    m_eventHandler->onLineBegin();

	//cout << "parsing line from " << lineFrom << " to " << lineTo << endl;
	//cout << "parsing line >" << line.substr(lineFrom, lineTo - lineFrom + 1) << "<" << endl << endl;

	unsigned int i = 0;
	while (i < line.length())
	{
		// parse chords (start symbol cannot be at the end of line)
		if (line[i] == L'[' && i < line.length() - 1)
		{
			int endPos = -1;

			// look for end character
			for (unsigned int j = i + 1; j < line.length(); j++)
			{
				if (line[j] == L']')
				{
					endPos = j;
					break;
				}
			}
			// if end character was found
			if (endPos >= 0)
			{
				//cout << "we have section: " << i << " " << endPos << endl;

                if (curText.length() > 0)
                {

                    m_eventHandler->onText(curText, p);
                    curText.erase();
                }

				p.setPos(lineFrom + i + 1, endPos - i - 1);
				m_eventHandler->onChord(strBuffer.substr(p.mPos, p.mLen), p);
				i = endPos + 1;
			}
			else
			{
                curText += line[i];
                i++;
			}
		}
		// space is delimiter of text blocks
		/*else if (line[i] == L' ')
		{
		    m_eventHandler->onText(curText);
            curText.erase();
            curText += line[i];
            m_eventHandler->onText(curText);
            curText.erase();
            i++;
		}*/
		else {
			//cout << "char" << strBuffer[i] << endl;
			curText += line[i];
			i++;
		}
	}
    // empty text buffer
    if (curText.length() > 0)
    {
    	p.setPos(0, 0);
        m_eventHandler->onText(curText, p);
        curText.erase();
    }

    m_eventHandler->onLineEnd();
}

void Parser::parseCommand(const std::wstring &strBuffer, const RawPos &p)
{
    //wcout << L"parsing >" << cmd << L"<" << endl;
    std::wstring cmd = strBuffer.substr(p.mPos, p.mLen);

    // trim right whitespace
    size_t cmdEnd = cmd.find_last_not_of(L' ');

	// default values for command id and command value
    std::wstring cmdId(L"");
    std::wstring cmdVal(L"");

    // look for command value which
    size_t sepPos = cmd.find_first_of(L':');

    //cout << "seppos is " << sepPos << endl;

    // if separator is present
    if (sepPos != std::wstring::npos)
    {
        // get command id
        if (sepPos > 0)
        {
            size_t first = cmd.find_first_not_of(L' ', 0);
            size_t last = cmd.find_last_not_of(L' ', sepPos - 1);
            // set command id only if not empty
            if (last != std::wstring::npos && first <= last)
            {
                //cout << first <<  " " << last << endl;
                cmdId.assign(cmd, first, last - first + 1);
            }
        }

        // get value
        if (sepPos < cmdEnd)
        {
            size_t first = cmd.find_first_not_of(L' ', sepPos + 1);
            if (first != std::wstring::npos && first <= cmdEnd)
            {
                //cout << first << " " << cmdEnd << endl;
                cmdVal.assign(cmd, first, cmdEnd - first + 1);
            }
        }
    }
	// strip command
    else
    {
			// trim left white space
            size_t first = cmd.find_first_not_of(L' ', 0);
            // trim right white space
            size_t last = cmd.find_last_not_of(L' ');
            // set command id only if not empty
            if (last != std::wstring::npos && first <= last)
            {
                //cout << first <<  " " << last << endl;
                cmdId.assign(cmd, first, last - first + 1);
            }
    }

    if (cmdId.length() > 0)
    {
    	CommandType cmdType = CMD_NONE;

    	// switch on or off raw mode (no chords are parsed)
		if (cmdId == cmdTabStart || cmdId == cmdTabStartShort)
		{
			m_rawMode = true;
			cmdType = CMD_TAB_START;
		}

		else if (cmdId == ::cmdTabEnd || cmdId == ::cmdTabEndShort)
		{
			m_rawMode = false;
			cmdType = CMD_TAB_END;
		}
    	// switch on or off raw mode (no chords are parsed)
		else if (cmdId == cmdStructStart || cmdId == cmdStructStartShort)
		{
			m_rawMode = true;
			cmdType = CMD_STRUCT_START;
		}
		else if (cmdId == cmdStructEnd || cmdId == cmdStructEndShort)
		{
			m_rawMode = false;
			cmdType = CMD_STRUCT_END;
		}
		else if (cmdId == ::cmdChorusStart || cmdId == ::cmdChorusStartShort)
			cmdType = CMD_CHORUS_START;
		else if (cmdId == ::cmdChorusEnd || cmdId == ::cmdChorusEndShort)
			cmdType = CMD_CHORUS_END;
		else if (cmdId == ::cmdTitle || cmdId == ::cmdTitleShort)
			cmdType = CMD_TITLE;
		else if (cmdId == ::cmdSubtitle || cmdId == ::cmdSubtitleShort)
			cmdType = CMD_SUBTITLE;
		else if (cmdId == ::cmdNewSong || cmdId == ::cmdNewSongShort)
			cmdType = CMD_NEW_SONG;
		else if (cmdId == ::cmdColumns || cmdId == ::cmdColumnsShort)
			cmdType = CMD_NEW_SONG;
		else if (cmdId == ::cmdComment || cmdId == ::cmdCommentShort)
			cmdType = CMD_COMMENT;


        m_eventHandler->onCommand(cmdType, cmdVal, p);
    }
}

std::wstring Transposer::transpose(std::wstring &song, int distance)
{
	std::wstring result;

	// parse song to get all fragments to be transposed
	mTransposeLines = false;
	mTransposedFragments.clear();
	Parser p(this);
	p.parse(song);

	unsigned int lastPos = 0;
	for (std::vector<RawPos>::iterator it = mTransposedFragments.begin(); it != mTransposedFragments.end(); it++)
	{
		result += song.substr(lastPos, (*it).mPos - lastPos);
		std::wstring toTranspose = song.substr((*it).mPos, (*it).mLen);
		//std::wcout << "Transpose " << *it << " " << lastPos << " " << ((*it).mPos - lastPos) << std::endl;
		result += transposeText(toTranspose, distance);
		lastPos = (*it).mPos + (*it).mLen;
	}
	result += song.substr(lastPos);

	//std::wstring newChord = Transposer::transposeChord(chordBuffer, distance);

	return result;
}

std::wstring Transposer::transposeText(std::wstring &text, int distance)
{
	static std::wstring roots(L"CDEFGAB");
	static std::wstring steps[] = { L"C", L"C#", L"D", L"Eb", L"E", L"F", L"F#", L"G", L"Ab", L"A", L"Bb", L"B" };
	//std::wcout << L"Transpose text <" << text << "> distance: " << distance << std::endl;

	if (text.length() < 1)
		return text;

	std::wstring result;

	unsigned int i = 0;

	while (i < text.length())
	{
		// look for chord root in array of roots
		int rootIndex = -1;
		for (size_t ix = 0; ix < roots.length(); ix++) if (text[i] == roots[ix]) { rootIndex = ix; break; }

		// nothing to do if no predefined capital letter found
		if (rootIndex == -1)
		{
			result += text[i];
			i++;
			continue;
		}

		std::wstring root;
		root += text[i];
		i++;
		// check for root modifiers (#, b)
		if (i + 1 < text.length())
			if (text[i] == '#' || text[i] == 'b')
			{
				i++;
				root += text[i];
			}

		// look for step
		int stepIndex = -1;
		for (int stepIx = 0; stepIx < 12; stepIx++)
		{
			if (root == steps[stepIx])
			{
				stepIndex = stepIx;
				break;
			}
		}

		// transposition
		stepIndex += distance;
		stepIndex %= 12;
		//std::cout << "new index is " << rootIndex << std::endl;
		if (stepIndex < 0)
			stepIndex = 12 + stepIndex;

		result += steps[stepIndex];
	}

	return result;
}

void Transposer::onCommand(const CommandType command, const std::wstring& value, const RawPos &pos)
{
	switch (command)
	{
		case CMD_STRUCT_START:
			//std::wcout << "transposer - struct start" << std::endl;
			mTransposeLines = true;
			break;
		case CMD_STRUCT_END:
			//std::wcout << "transposer - struct end" << std::endl;
			mTransposeLines = false;
			break;
		default:
			;
	};
}
void Transposer::onChord(const std::wstring& chord, const RawPos &pos)
{
	//std::wcout << "transposer - chord: <" << chord << L">" << std::endl;
	mTransposedFragments.push_back(pos);
}
void Transposer::onLine(const std::wstring& line, const RawPos &pos)
{
	//std::wcout << "transposer - line: <" << line << L">" << std::endl;
	if (mTransposeLines)
		mTransposedFragments.push_back(pos);
}

#ifdef _TEST
// example implementation for simple text output
class EventHandlerX1 : public EventHandler
{
	public:
		EventHandlerX1() {};
		virtual void onBegin() {};
		virtual void onEnd() {};
		virtual void onLineBegin() {};
		virtual void onLineEnd() {};
		virtual void onCommand(const CommandType command, const std::wstring& value, const RawPos &pos) { std::wcout << L"command <" << command << L"> pos " << pos << std::endl;};
		virtual void onChord(const std::wstring& chord, const RawPos &pos) { std::wcout << L"chord <" << chord << L"> pos " << pos << std::endl;};
		virtual void onText(const std::wstring& text, const RawPos &pos) {};
		virtual void onLine(const std::wstring& line, const RawPos &pos) {};
};

int main(int argc, char **argv)
{
	//EventHandler x;
	//EventHandlerTxt y;
	EventHandlerX1 x;
	//x.onLine("ahoj");

	//Parser p(&y);
	//Parser p(&y);
	Parser p(&x);

    //p.parse("{title: Song1}");
    //p.parse(L"[Em]Hold [D]to a [C]dream, [Em]carry it [D]up and down\n[Em]Fol[D]low a [C]star, [Em]search the [D]world around\n[Em]Hold [D]to a [C]dream, [Em]carry it [D]close to me\n[G]I'm frozen in time, you alone can set me [D]free");
    //p.parse(L"\nHold to a dream, carry it up and down\nFollow a star, search the world around\nHold to a dream, carry it close to me\nI'm frozen in time, you alone can set me free");
	//p.parse(L"ahoj\nmiso\njani");

    //p.parse("x\ny\nz");

    std::wstring in(L"s[C]w[Dm]e\n"
		L"Text taky\n"
		L"{sos}\n"
		L"/Am /C /Bbm /\n"
		L"{eos}\n"
	);

	std::wcout << in << std::endl;
	Transposer t;
	std::wcout << t.transpose(in, 1) << std::endl;

	return 0;
}
#endif // _TEST

