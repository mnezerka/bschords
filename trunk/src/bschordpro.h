#ifndef BSCHORDPRO_H
#define BSCHORDPRO_H

#include <iostream>
#include <string>

using namespace std;

class BSChordProEventHandler
{
	public:
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
