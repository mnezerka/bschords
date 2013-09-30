#ifndef BSCHORDPRO_H
#define BSCHORDPRO_H

#include <iostream>
#include <string>
#include <vector>

// definition of chord pro format tokens
namespace bschordpro
{
	enum CommandType {
		CMD_TITLE,
		CMD_SUBTITLE,
		CMD_CHORUS_START,
		CMD_CHORUS_END,
		CMD_TAB_START,
		CMD_TAB_END,
		CMD_STRUCT_START,
		CMD_STRUCT_END,
		CMD_NONE
	};

	struct RawPos {
		unsigned int mPos;
		unsigned int mLen;
		RawPos(unsigned int pos = 0, unsigned int len = 0) : mPos(pos), mLen(len) { };
		void setPos(unsigned int pos, unsigned int len) { mPos = pos; mLen = len; };
		friend std::wostream& operator<<(std::wostream& os, const RawPos& p) { os << L"(" << p.mPos << L", " << p.mLen << L")"; return os; };
		friend std::ostream& operator<<(std::ostream& os, const RawPos& p) { os << "(" << p.mPos << ", " << p.mLen << ")"; return os; };
	};
	// interface (abstract class) for all chord pro parsing listeners
	class EventHandler
	{
		public:
			// called on begin of parsed document
			virtual void onBegin() = 0;
			// called after document is parsed
			virtual void onEnd() = 0;
			// called at the beginning of parsed line
			virtual void onLineBegin() = 0;
			// called at the end of parsed line
			virtual void onLineEnd() = 0;
			// called when command is found
			virtual void onCommand(const CommandType command, const std::wstring& value, const RawPos &pos) = 0;
			// called for each chord on line
			virtual void onChord(const std::wstring& chord, const RawPos &pos) = 0;
			// called for each text fragment on line (separators are spaces and chords)
			virtual void onText(const std::wstring& text, const RawPos &pos) = 0;
			// called for preformatted line (e.g. inside tab section)
			virtual void onLine(const std::wstring& line, const RawPos &pos) = 0;
	};

	// example implementation for simple text output
	class EventHandlerTxt : public EventHandler
	{
		public:
			EventHandlerTxt();
			virtual void onBegin() {};
			virtual void onEnd() {};
			virtual void onLineBegin();
			virtual void onLineEnd();
			virtual void onCommand(const CommandType command, const std::wstring& value, const RawPos &pos);
			virtual void onChord(const std::wstring& chord, const RawPos &pos);
			virtual void onText(const std::wstring& text, const RawPos &pos);
			virtual void onLine(const std::wstring& line, const RawPos &pos);
		private:
			std::wstring m_chordBuffer;
			std::wstring m_textBuffer;
	};

	// parser for chordpro format
	class Parser
	{
		public:
			// ctor
			Parser(EventHandler* eventHandler) : m_rawMode(false),  m_eventHandler(eventHandler) { }
			// parse text buffer
			void parse(const std::wstring& s);

		private:
			// raw mode - no chords are parsed (used for tabs and other preformatted blocks)
			bool m_rawMode;
			EventHandler *m_eventHandler;
			void parseLine(const std::wstring& s, unsigned int lineFrom, unsigned int lineLen, unsigned int lineIndex);
			void parseCommand(const std::wstring &strBuffer, const RawPos &p);
	};

	// class for song transpositions
	class Transposer: public EventHandler
	{
		public:
			Transposer() : mTransposeLines(false) {};
			std::wstring transpose(std::wstring &song, int distance);
			std::wstring transposeText(std::wstring &text, int distance);

			virtual void onBegin() {};
			virtual void onEnd() {};
			virtual void onLineBegin() {};
			virtual void onLineEnd() {};
			virtual void onCommand(const CommandType command, const std::wstring& value, const RawPos &pos);
			virtual void onChord(const std::wstring& chord, const RawPos &pos);
			virtual void onText(const std::wstring& text, const RawPos &pos) {};
			virtual void onLine(const std::wstring& line, const RawPos &pos);
		private:
			bool mTransposeLines;
			std::vector<RawPos> mTransposedFragments;
	};
}

#endif // BSCHORDPRO_H
