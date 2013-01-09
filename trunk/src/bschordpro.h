#ifndef BSCHORDPRO_H
#define BSCHORDPRO_H

#include <iostream>
#include <string>

// definition of chord pro format tokens
namespace bschordpro
{
	static const wchar_t *CMD_TITLE 				= L"title";
	static const wchar_t *CMD_SUBTITLE				= L"subtitle";
	static const wchar_t *CMD_CHORUS_START			= L"start_of_chorus";
	static const wchar_t *CMD_CHORUS_START_SHORT	= L"soc";
	static const wchar_t *CMD_CHORUS_END			= L"end_of_chorus";
	static const wchar_t *CMD_CHORUS_END_SHORT		= L"eoc";
	static const wchar_t *CMD_TAB_START				= L"start_of_tab";
	static const wchar_t *CMD_TAB_START_SHORT		= L"sot";
	static const wchar_t *CMD_TAB_END				= L"end_of_tab";
	static const wchar_t *CMD_TAB_END_SHORT			= L"eot";
}

using namespace std;

// interface (abstract class) for all chord pro parsing listeners
class BSChordProEventHandler
{
	public:
		virtual void onBegin() = 0;
		virtual void onEnd() = 0;
		virtual void onLineBegin() = 0;
        virtual void onLineEnd() = 0;
		virtual void onCommand(const wstring& command, const wstring& value) = 0;
		virtual void onChord(const wstring& chord) = 0;
		virtual void onText(const wstring& text) = 0;
		virtual void onLine(const wstring& line) = 0;
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
		virtual void onLine(const wstring& line);
    private:
	    wstring m_chordBuffer;
        wstring m_textBuffer;
};

class BSChordProParser
{
	private:
		// raw mode - no chords are parsed (used for tabs and other preformatted blocks)
		bool m_rawMode;
		BSChordProEventHandler *m_eventHandler;
		void parseLine(const wstring& s, unsigned int lineFrom, unsigned int lineLen);
		void parseCommand(const wstring& cmd);
		void parseChord(const wstring& chord);

    public:
		BSChordProParser(BSChordProEventHandler* eventHandler) : m_rawMode(false),  m_eventHandler(eventHandler) { }
		void parse(const wstring& s);
};

#endif // BSCHORDPRO_H
