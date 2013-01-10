#include "bschordpro.h"

using namespace bschordpro;

static const wchar_t *cmdTitle 				= L"title";
static const wchar_t *cmdSubtitle			= L"subtitle";
static const wchar_t *cmdChorusStart		= L"start_of_chorus";
static const wchar_t *cmdChorusStartShort	= L"soc";
static const wchar_t *cmdChorusEnd			= L"end_of_chorus";
static const wchar_t *cmdChorusEndShort		= L"eoc";
static const wchar_t *cmdTabStart			= L"start_of_tab";
static const wchar_t *cmdTabStartShort		= L"sot";
static const wchar_t *cmdTabEnd				= L"end_of_tab";
static const wchar_t *cmdTabEndShort		= L"eot";

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

void EventHandlerTxt::onCommand(const CommandType command, const std::wstring& value)
{
    std::wcout << L"command: " << command << L" with value: " << value;
}

void EventHandlerTxt::onChord(const std::wstring& chord)
{
    m_chordBuffer.append(chord);
    m_chordBuffer.append(L" ");
}

void EventHandlerTxt::onText(const std::wstring& text)
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

void EventHandlerTxt::onLine(const std::wstring& line)
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
				parseLine(s, lineFrom, lineLen);
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
		parseLine(s, lineFrom, lineLen);

	m_eventHandler->onEnd();
}

// called for each text line of parsed buffer
void Parser::parseLine(const std::wstring& strBuffer, unsigned int lineFrom, unsigned int lineLen)
{
	bool lineHasCmd = false;
	std::wstring curText = L"";
	std::wstring line;

	line.assign(strBuffer, lineFrom, lineLen);

    //wcout << L"from: " << lineFrom << L", len:" << lineLen << L" >" << line << L"<" << endl;

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
		//wcout << L"Command found from: " << cmdBegin << " to: " << cmdEnd << endl;
		parseCommand(line.substr(cmdBegin + 1, cmdEnd - cmdBegin - 1));
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
		m_eventHandler->onLine(line);
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
                    m_eventHandler->onText(curText);
                    curText.erase();
                }

				parseChord(line.substr(i + 1, endPos - i - 1));
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
        m_eventHandler->onText(curText);
        curText.erase();
    }

    m_eventHandler->onLineEnd();
}

void Parser::parseCommand(const std::wstring& cmd)
{
    //wcout << L"parsing >" << cmd << L"<" << endl;
    size_t cmdEnd = cmd.find_last_not_of(L' ');

    std::wstring cmdId(L"");
    std::wstring cmdVal(L"");

    // look for optional command value
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
            if (last != std::wstring::npos && first < last)
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
            size_t first = cmd.find_first_not_of(L' ', 0);
            size_t last = cmd.find_last_not_of(L' ');
            // set command id only if not empty
            if (last != std::wstring::npos && first < last)
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
		else if (cmdId == ::cmdChorusStart || cmdId == ::cmdChorusStartShort)
			cmdType = CMD_CHORUS_START;
		else if (cmdId == ::cmdChorusEnd || cmdId == ::cmdChorusEndShort)
			cmdType = CMD_CHORUS_END;
		else if (cmdId == ::cmdTitle)
			cmdType = CMD_TITLE;
		else if (cmdId == ::cmdSubtitle)
			cmdType = CMD_SUBTITLE;

        m_eventHandler->onCommand(cmdType, cmdVal);
    }
}

void Parser::parseChord(const std::wstring& chord)
{
	m_eventHandler->onChord(chord);
}

#ifdef _TEST
int main(int argc, char **argv)
{
	EventHandler x;
	EventHandlerTxt y;
	//x.onLine("ahoj");

	Parser p(&x);
	//Parser p(&y);

	//p.parse("0123456789012345678\nand this is second line.\n\rAnd this is third line\r\r\rAnd last one");
	//       012345678901234567 89 01234567890
	//p.parse("{title: Song1}xxx\n \nmisa[]nez");
	//p.parse("{title: Wild Horses}\n[Hm]Childwood [G]living\n[Hm]Is easy to [G]do");

	//p.parse("All that you [Am]wanted\nis easy [D][G][A] to [C]do.");

    //p.parse("x [A][B][C] [First]misa[C]Chord");

    //p.parse("{title: Song1}");
    //p.parse(L"[Em]Hold [D]to a [C]dream, [Em]carry it [D]up and down\n[Em]Fol[D]low a [C]star, [Em]search the [D]world around\n[Em]Hold [D]to a [C]dream, [Em]carry it [D]close to me\n[G]I'm frozen in time, you alone can set me [D]free");
    //p.parse(L"\nHold to a dream, carry it up and down\nFollow a star, search the world around\nHold to a dream, carry it close to me\nI'm frozen in time, you alone can set me free");
	//p.parse(L"ahoj\nmiso\njani");

    //p.parse(L"{title  :  Song1}");
    //p.parse(L"{title:Song1}");
    //p.parse(L"{  title  :  Song1  }");
    //p.parse(L"{title:}");
    //p.parse(L"{:}");
    //p.parse(L"{ :}");
    //p.parse(L"{}");
    //p.parse(L" {}");
    //p.parse(L"a {cmd:x} a {cmd}");
    p.parse(L"{title: ahoj}\nThis is first text line");

    //p.parse("#  ahoj\n#ja jsem misa\n \na ty?");

    //p.parse("[A");

    //p.parse("x\ny\nz");
	return 0;
}
#endif // _TEST

