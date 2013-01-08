#ifndef BSCHORDPRO_H
#define BSCHORDPRO_H

#define BSCH_CMD_TITLE               title
#define BSCH_CMD_SUBTITLE            subtitle
#define BSCH_CMD_CHORUS_START        start_of_chorus
#define BSCH_CMD_CHORUS_START_SHORT  soc
#define BSCH_CMD_CHORUS_END          end_of_chorus
#define BSCH_CMD_CHORUS_END_SHORT    eoc

#include <iostream>
#include <string>

using namespace std;

class BSChordProEventHandler
{
	public:
		virtual void onBegin() { wcout << L"begin" << endl; };
		virtual void onEnd() { wcout << L"end" << endl; };
		virtual void onLineBegin() { wcout << L"* line begin:" << endl; };
        virtual void onLineEnd() { wcout << L"* line end:" << endl; };
		virtual void onCommand(const wstring& command, const wstring& value) { wcout << L"* parsed command >" << command << L"< with value >" << value << L"<" << endl; };
		virtual void onChord(const wstring& chord) { wcout << L"* parsed chord: >" << chord << L"<" << endl; };
		virtual void onText(const wstring& text) { wcout << L"* parsed text: >" << text << L"<"<< endl; };
};

class BSChordProEventHandlerTxt : public BSChordProEventHandler
{
    public:
        BSChordProEventHandlerTxt();
		virtual void onLineBegin();
        virtual void onLineEnd();
		virtual void onCommand(const wstring& command, const wstring& value);
		virtual void onChord(const wstring& chord);
		virtual void onText(const wstring& text);
    private:
	    wstring m_chordBuffer;
        wstring m_textBuffer;
};

class BSChordProParser
{
	private:
		BSChordProEventHandler *m_eventHandler;
		void parseLine(const wstring& s, unsigned int lineFrom, unsigned int lineLen);
		void parseCommand(const wstring& cmd);
		void parseChord(const wstring& chord);

    public:
		BSChordProParser(BSChordProEventHandler* eventHandler) { m_eventHandler = eventHandler; }
		void parse(const wstring& s);
};

#endif // BSCHORDPRO_H
