
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
    cout << m_chordBuffer << endl;
    cout << m_textBuffer;
    cout << endl << endl;
};

void BSChordProEventHandlerTxt::onCommand(const std::string& command, const std::string& value)
{
    cout << "command: " << command << " with value: " << value;
};

void BSChordProEventHandlerTxt::onChord(const std::string& chord)
{
    m_chordBuffer.append(chord);
    m_chordBuffer.append(" ");
};

void BSChordProEventHandlerTxt::onText(const std::string& text)
{
    m_textBuffer.append(text);

    int diff = m_textBuffer.length() - m_chordBuffer.length();

    if (diff > 0)
        m_chordBuffer.append(diff, ' ');
    else
    {
        m_textBuffer.append(-1 * diff, ' ');
    }

};


//////////////////////////////////////////////////////////////////////////////
void BSChordProParser::parse(const std::string& s)
{
	bool onSeparator = false;
	unsigned int lineFrom = 0;
	unsigned int lineTo = 0;

	if (m_eventHandler == NULL)
		return;

	for (unsigned int i = 0; i < s.length(); i++)
	{
		if (s[i] == '\n' || s[i] == '\r')
		{
			onSeparator = true;
		}
		else
		{
			if (!onSeparator)
			{
				lineTo = i;
			}
			else
			{
				parseLine(s, lineFrom, lineTo);
				onSeparator = false;
				lineFrom = lineTo = i;
			}
		}
	}

	// consume last line
	if (lineFrom < s.length())
		parseLine(s, lineFrom, s.length() - 1);
}

void BSChordProParser::parseLine(const std::string& line, unsigned int lineFrom, unsigned int lineTo)
{
	string curText = "";

    m_eventHandler->onLineBegin();

	//cout << "parsing line from " << lineFrom << " to " << lineTo << endl;
	//cout << "parsing line >" << line.substr(lineFrom, lineTo - lineFrom + 1) << "<" << endl << endl;

	unsigned int i = lineFrom;
	while (i <= lineTo)
	{
		// commands and chords (start symbol cannot be at the end of line)
		if ((line[i] == '{' || line[i] == '[') && i < lineTo)
		{
			int endPos = -1;
			char endChar = 0;
			enum { eCmd, eChord } sectionType;

			switch (line[i])
			{
				case '{':
					endChar = '}';
					sectionType = eCmd;
					break;
				case '[':
					endChar = ']';
					sectionType = eChord;
					break;
			}

			// look for end character
			for (unsigned int j = i + 1; j <= lineTo; j++)
			{
				if (line[j] == endChar)
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
						parseCommand(line.substr(i + 1, endPos - i - 1));
						break;
					case eChord:
						parseChord(line.substr(i + 1, endPos - i - 1));
						break;
				}

				i = endPos + 1;
			}
			else
			{
                curText += line[i];
                i++;
			}
		}
		// space is delimiter of text blocks
		else if (line[i] == ' ')
		{
		    m_eventHandler->onText(curText);
            curText.erase();
            curText += line[i];
            m_eventHandler->onText(curText);
            curText.erase();
            i++;
		}
		else {
			//cout << "char" << line[i] << endl;
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

void BSChordProParser::parseCommand(const std::string& cmd)
{
    //cout << "parsing >" << cmd << "<" << endl;
    // default values
    //size_t cmdValBegin = -1;
    //size_t cmdEnd = -1;

    // trim cmd string
    //size_t cmdIdBegin = cmd.find_first_not_of(' ');
    size_t cmdEnd = cmd.find_last_not_of(' ');

    std::string cmdId("");
    std::string cmdVal("");

    // look for optional command value
    size_t sepPos = cmd.find_first_of(':');

    //cout << "seppos is " << sepPos << endl;

    // if separator is present
    if (sepPos != std::string::npos)
    {
        // get command id
        if (sepPos > 0)
        {
            size_t first = cmd.find_first_not_of(' ', 0);
            size_t last = cmd.find_last_not_of(' ', sepPos - 1);
            // set command id only if not empty
            if (last != std::string::npos && first < last)
            {
                //cout << first <<  " " << last << endl;
                cmdId.assign(cmd, first, last - first + 1);
            }
        }

        // get value
        if (sepPos < cmdEnd)
        {
            size_t first = cmd.find_first_not_of(' ', sepPos + 1);
            if (first != std::string::npos && first < cmdEnd)
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

void BSChordProParser::parseChord(const std::string& chord)
{
	m_eventHandler->onChord(chord);
}

#ifdef BSCHORDPRO_TEST
int main(int argc, char **argv)
{
	BSChordProEventHandler x;
	BSChordProEventHandlerTxt y;
	//x.onLine("ahoj");

	BSChordProParser p(&y);

	//p.parse("0123456789012345678\nand this is second line.\n\rAnd this is third line\r\r\rAnd last one");
	//       012345678901234567 89 01234567890
	//p.parse("{title: Song1}xxx\n \nmisa[]nez");
	//p.parse("{title: Wild Horses}\n[Hm]Childwood [G]living\n[Hm]Is easy to [G]do");

	//p.parse("All that you [Am]wanted\nis easy [D][G][A] to [C]do.");

    //p.parse("x [A][B][C] [First]misa[C]Chord");

    p.parse("{title: Song1}");
    //p.parse("[Em]Hold [D]to a [C]dream, [Em]carry it [D]up and down\n[Em]Fol[D]low a [C]star, [Em]search the [D]world around\n[Em]Hold [D]to a [C]dream, [Em]carry it [D]close to me\n[G]I'm frozen in time, you alone can set me [D]free");

    p.parse("{title  :  Song1}");
    p.parse("{title:Song1}");
    p.parse("{  title  :  Song1  }");
    p.parse("{title:}");
    p.parse("{:}");
    p.parse("{ :}");
    p.parse("{}");


    //p.parse("[A");

    //p.parse("x\ny\nz");
	return 0;
}
#endif // BSCHORDPRO_TEST

