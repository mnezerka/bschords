#ifndef BSCHORDPRO_H
#define BSCHORDPRO_H

#include <iostream>
#include <string>

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
		CMD_NONE
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
			virtual void onCommand(const CommandType command, const std::wstring& value) = 0;
			// called for each chord on line
			virtual void onChord(const std::wstring& chord) = 0;
			// called for each text fragment on line (separators are spaces and chords)
			virtual void onText(const std::wstring& text) = 0;
			// called for preformatted line (e.g. inside tab section)
			virtual void onLine(const std::wstring& line) = 0;
	};

	// example implementation for simple text output
	class EventHandlerTxt : public EventHandler
	{
		public:
			EventHandlerTxt();
			virtual void onLineBegin();
			virtual void onLineEnd();
			virtual void onCommand(const CommandType command, const std::wstring& value);
			virtual void onChord(const std::wstring& chord);
			virtual void onText(const std::wstring& text);
			virtual void onLine(const std::wstring& line);
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
			void parseLine(const std::wstring& s, unsigned int lineFrom, unsigned int lineLen);
			void parseCommand(const std::wstring& cmd);
			void parseChord(const std::wstring& chord);

	};
}

#endif // BSCHORDPRO_H
