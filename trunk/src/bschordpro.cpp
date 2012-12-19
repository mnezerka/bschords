
#include "bschordpro.h"

BSChordProEventHandlerTxt::BSChordProEventHandlerTxt()
{
;
};

void BSChordProEventHandlerTxt::onLineBegin()
{
    m_chordBuffer.erase();
    m_textBuffer.erase();
};

void BSChordProEventHandlerTxt::onLineEnd()
{
    if (m_chordBuffer.find_first_not_of(L' ') != wstring::npos)
        wcout << endl << m_chordBuffer << endl;

    wcout << m_textBuffer;
    wcout << endl;
};

void BSChordProEventHandlerTxt::onCommand(const wstring& command, const wstring& value)
{
    wcout << L"command: " << command << L" with value: " << value;
};

void BSChordProEventHandlerTxt::onChord(const wstring& chord)
{
    m_chordBuffer.append(chord);
    m_chordBuffer.append(L" ");
};

void BSChordProEventHandlerTxt::onText(const wstring& text)
{
    m_textBuffer.append(text);

    int diff = m_textBuffer.length() - m_chordBuffer.length();

    if (diff > 0)
        m_chordBuffer.append(diff, L' ');
    else
    {
        m_textBuffer.append(-1 * diff, L' ');
    }

};


//////////////////////////////////////////////////////////////////////////////
void BSChordProParser::parse(const wstring& s)
{
    /*wcout << L"size of wchar_t:"<< sizeof(wchar_t) << endl;
    wcout << L"size of buffer: "<< sizeof(s) << endl;
    wcout << L"len of buffer: "<< s.size() << endl;
    wcout << static_cast<unsigned int>(static_cast<unsigned short>(L'\n')) << endl;
    for (size_t i = 0; i < s.size(); i++)
    {
        wcout << i << ": "<< static_cast<unsigned int>(static_cast<unsigned short>(s[i])) << endl;
    }*/

	bool onLineSeparator = false;
	unsigned int lineFrom = 0;
	unsigned int lineTo = 0;

	if (m_eventHandler == NULL)
		return;

    // loop over all characters of parsed wstring (buffer)
	for (size_t i = 0; i < s.length(); i++)
	{
		if (s[i] == L'\n' || s[i] == L'\r')
		{
			onLineSeparator = true;
		}
		else
		{
			if (!onLineSeparator)
			{
				lineTo = i;
			}
			else
			{
				parseLine(s, lineFrom, lineTo);
				onLineSeparator = false;
				lineFrom = lineTo = i;
			}
		}
	}

	// consume last line
	if (lineFrom < s.length())
		parseLine(s, lineFrom, lineTo);
}

void BSChordProParser::parseLine(const wstring& strBuffer, unsigned int lineFrom, unsigned int lineTo)
{
	wstring curText = L"";

    // first check if strBuffer should be parsed
    size_t firstCharPos = strBuffer.find_first_not_of(L' ', lineFrom);

    //wcout << L"from: " << lineFrom << L", to:" << lineTo << L", first char pos:" << firstCharPos << endl;

    // skip emtpy lines (containing only spaces)
    //if (firstCharPos > lineTo)
    //    return;

    // skip comments
    if (strBuffer[firstCharPos] == L'#')
        return;

    m_eventHandler->onLineBegin();

	//cout << "parsing line from " << lineFrom << " to " << lineTo << endl;
	//cout << "parsing line >" << line.substr(lineFrom, lineTo - lineFrom + 1) << "<" << endl << endl;

	unsigned int i = lineFrom;
	while (i <= lineTo)
	{
		// commands and chords (start symbol cannot be at the end of line)
		if ((strBuffer[i] == L'{' || strBuffer[i] == L'[') && i < lineTo)
		{
			int endPos = -1;
			char endChar = 0;
			enum { eCmd, eChord } sectionType;

			switch (strBuffer[i])
			{
				case L'{':
					endChar = L'}';
					sectionType = eCmd;
					break;
				case L'[':
					endChar = L']';
					sectionType = eChord;
					break;
			}

			// look for end character
			for (unsigned int j = i + 1; j <= lineTo; j++)
			{
				if (strBuffer[j] == endChar)
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
				switch (sectionType)
				{
					case eCmd:
						//cout << "we have command " << line.substr(i + 1, pos - i - 1) << endl;
						parseCommand(strBuffer.substr(i + 1, endPos - i - 1));
						break;
					case eChord:
						parseChord(strBuffer.substr(i + 1, endPos - i - 1));
						break;
				}

				i = endPos + 1;
			}
			else
			{
                curText += strBuffer[i];
                i++;
			}
		}
		// space is delimiter of text blocks
		else if (strBuffer[i] == L' ')
		{
		    m_eventHandler->onText(curText);
            curText.erase();
            curText += strBuffer[i];
            m_eventHandler->onText(curText);
            curText.erase();
            i++;
		}
		else {
			//cout << "char" << strBuffer[i] << endl;
			curText += strBuffer[i];
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

void BSChordProParser::parseCommand(const wstring& cmd)
{
    //cout << "parsing >" << cmd << "<" << endl;
    // default values
    //size_t cmdValBegin = -1;
    //size_t cmdEnd = -1;

    // trim cmd wstring
    //size_t cmdIdBegin = cmd.find_first_not_of(' ');
    size_t cmdEnd = cmd.find_last_not_of(L' ');

    wstring cmdId(L"");
    wstring cmdVal(L"");

    // look for optional command value
    size_t sepPos = cmd.find_first_of(L':');

    //cout << "seppos is " << sepPos << endl;

    // if separator is present
    if (sepPos != wstring::npos)
    {
        // get command id
        if (sepPos > 0)
        {
            size_t first = cmd.find_first_not_of(L' ', 0);
            size_t last = cmd.find_last_not_of(L' ', sepPos - 1);
            // set command id only if not empty
            if (last != string::npos && first < last)
            {
                //cout << first <<  " " << last << endl;
                cmdId.assign(cmd, first, last - first + 1);
            }
        }

        // get value
        if (sepPos < cmdEnd)
        {
            size_t first = cmd.find_first_not_of(L' ', sepPos + 1);
            if (first != string::npos && first < cmdEnd)
            {
                //cout << first << " " << cmdEnd << endl;
                cmdVal.assign(cmd, first, cmdEnd - first + 1);
            }
        }
    }

    //cout << "command id is: >" << cmdId << "<" << endl;
    //cout << "value is: >" << cmdVal << "<" << endl;
    //if ()

    if (cmdId.length() > 0)
        m_eventHandler->onCommand(cmdId, cmdVal);
}

void BSChordProParser::parseChord(const wstring& chord)
{
	m_eventHandler->onChord(chord);
}

#ifdef BSCHORDPRO_TEST
int main(int argc, char **argv)
{
	BSChordProEventHandler x;
	BSChordProEventHandlerTxt y;
	//x.onLine("ahoj");

	//BSChordProParser p(&x);
	BSChordProParser p(&y);

	//p.parse("0123456789012345678\nand this is second line.\n\rAnd this is third line\r\r\rAnd last one");
	//       012345678901234567 89 01234567890
	//p.parse("{title: Song1}xxx\n \nmisa[]nez");
	//p.parse("{title: Wild Horses}\n[Hm]Childwood [G]living\n[Hm]Is easy to [G]do");

	//p.parse("All that you [Am]wanted\nis easy [D][G][A] to [C]do.");

    //p.parse("x [A][B][C] [First]misa[C]Chord");

    //p.parse("{title: Song1}");
    p.parse(L"[Em]Hold [D]to a [C]dream, [Em]carry it [D]up and down\n[Em]Fol[D]low a [C]star, [Em]search the [D]world around\n[Em]Hold [D]to a [C]dream, [Em]carry it [D]close to me\n[G]I'm frozen in time, you alone can set me [D]free");
    p.parse(L"\nHold to a dream, carry it up and down\nFollow a star, search the world around\nHold to a dream, carry it close to me\nI'm frozen in time, you alone can set me free");

    /*
    p.parse("{title  :  Song1}");
    p.parse("{title:Song1}");
    p.parse("{  title  :  Song1  }");
    p.parse("{title:}");
    p.parse("{:}");
    p.parse("{ :}");
    p.parse("{}");
    */

    //p.parse("#  ahoj\n#ja jsem misa\n \na ty?");

    //p.parse("[A");

    //p.parse("x\ny\nz");
	return 0;
}
#endif // BSCHORDPRO_TEST

