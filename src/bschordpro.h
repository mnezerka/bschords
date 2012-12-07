#ifndef BSCHORDPRO_H
#define BSCHORDPRO_H

#include <iostream>
#include <string>

using namespace std;

class BSChordProEventHandler
{
	public:
		virtual void onLineBegin() { cout << "line begin:  " << endl; };
        virtual void onLineEnd() { cout << "line end:  " << endl; };
		virtual void onCommand(const std::string& command, const std::string& value) { cout << "parsed command " << command << " with value " << value << endl; };
		virtual void onChord(const std::string& chord) { cout << "parsed chord: " << chord << endl; };
		virtual void onText(const std::string& text) { cout << "parsed text: " << text << endl; };
};

class BSChordProEventHandlerTxt : public BSChordProEventHandler
{
    public:
        BSChordProEventHandlerTxt();
		virtual void onLineBegin();
        virtual void onLineEnd();
		virtual void onCommand(const std::string& command, const std::string& value);
		virtual void onChord(const std::string& chord);
		virtual void onText(const std::string& text);
    private:
	    string m_chordBuffer;
        string m_textBuffer;
};

class BSChordProParser
{
	private:
		BSChordProEventHandler *m_eventHandler;
		void parseLine(const std::string& s, unsigned int lineFrom, unsigned int lineTo);
		void parseCommand(const std::string& cmd);
		void parseChord(const std::string& chord);

    public:
		BSChordProParser(BSChordProEventHandler* eventHandler) { m_eventHandler = eventHandler; }
		void parse(const std::string& s);
};

#endif // BSCHORDPRO_H
