/**
 * @file
 * @author  michal.nezerka@gmail.com
 * @brief   Declaration of various classes related to ChordPro format processing
 */

#ifndef BSCHORDPRO_H
#define BSCHORDPRO_H

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

// definition of chord pro format tokens
namespace bschordpro
{

/** \brief Supported command types */
enum CommandType
{
    CMD_TITLE,          /**< Song Title */
    CMD_SUBTITLE,       /**< Song Subtitle */
    CMD_CHORUS_START,   /**< Start of chorus */
    CMD_CHORUS_END,     /**< End of chorus */
    CMD_TAB_START,      /**< Start of tab section */
    CMD_TAB_END,        /**< End of tab section */
    CMD_STRUCT_START,   /**< Start of song structure section */
    CMD_STRUCT_END,     /**< End of song structure section */
    CMD_COLUMNS,        /**< Number of columns */
    CMD_NEW_SONG,       /**< Start of new song */
    CMD_COMMENT,        /**< Comment */
    CMD_NONE            /**< Not used */
};

/** \brief Auxiliary class representing raw position (start, length) of chordpro token in string (stream of characters) */
struct RawPos
{
    /// position in string (zero based index)
    unsigned int mPos;
    /// length
    unsigned int mLen;
    /// Ctor
    RawPos(unsigned int pos = 0, unsigned int len = 0) : mPos(pos), mLen(len) { };
    /// Setter for position and length
    void setPos(unsigned int pos, unsigned int len)
    {
        mPos = pos;
        mLen = len;
    };
    /// Output to wide char stream
    friend std::wostream& operator<<(std::wostream& os, const RawPos& p)
    {
        os << L"(" << p.mPos << L", " << p.mLen << L")";
        return os;
    };
    /// Output to stream
    friend std::ostream& operator<<(std::ostream& os, const RawPos& p)
    {
        os << "(" << p.mPos << ", " << p.mLen << ")";
        return os;
    };
};

/**
 * \class EventHandler
 * \brief Interface (abstract class) for listening to ChordPro parser events
 *
 * Class acts as base for descendants which are interested in events generated during
 * parsing of text in ChordPro format. Class implements empty bodies of all methods so
 * descendant class can implement only part of provided event methods
 */
class EventHandler
{
public:
    /// called on begin of parsed document
    virtual void onBegin() {};
    /// called after document is parsed
    virtual void onEnd() {};
    /// called at the beginning of parsed line
    virtual void onLineBegin() {};
    /// called at the end of parsed line
    virtual void onLineEnd() {};
    /// called when command is found
    virtual void onCommand(const CommandType command, const std::wstring& value, const RawPos &pos) {};
    /// called when unknown (unsupported) command is found
    virtual void onCommandUnknown(const std::wstring &cmd, const std::wstring &value, const RawPos &pos) {};
    /// called for each chord on line
    virtual void onChord(const std::wstring& chord, const RawPos &pos) {};
    /// called for each text fragment on line (separators are spaces and chords)
    virtual void onText(const std::wstring& text, const RawPos &pos) {};
    /// called for preformatted line (e.g. inside tab section)
    virtual void onLine(const std::wstring& line, const RawPos &pos) {};
};


/**
 * \class Parser
 * \brief ChordPro format parser
 *
 * Create instance of parser class, provide instance of own EventHandler and call parse method. Parser
 * will process text and generate events according to ChordPro tokens found in parsed text.
 */
class Parser
{
public:
    /// Ctor
    Parser(EventHandler* eventHandler) : m_rawMode(false),  m_eventHandler(eventHandler) { }
    /// Parse text buffer (generated events are passed to event handler)
    void parse(const std::wstring& s);

private:
    /// raw mode flag - when true - no chords are parsed (used for tabs and other preformatted blocks)
    bool m_rawMode;
    /// event handler instance which is notified of all events during parsing
    EventHandler *m_eventHandler;
    /// internal method for parsing single line of text
    void parseLine(const std::wstring& s, unsigned int lineFrom, unsigned int lineLen, unsigned int lineIndex);
    /// internal mehtod for parsing single ChordPro command
    void parseCommand(const std::wstring &strBuffer, const RawPos &p);
};

/** \brief Transposer of string in ChordPro musical symbols (chords, etc.) */
class Transposer: public EventHandler
{
public:
    /// Ctor
    Transposer() : mTransposeLines(false) {};
    /// Transpose chords and structure elements in ChordPro formatted text (song)
    std::wstring transpose(std::wstring &song, int distance);
    /// Transpose text - all capitals are replaced according to distance (e.g. C -> Bb)
    std::wstring transposeText(std::wstring &text, int distance);

    /// called by ChordPro parser for commands
    virtual void onCommand(const CommandType command, const std::wstring& value, const RawPos &pos);
    /// called by ChordPro parser for chords
    virtual void onChord(const std::wstring& chord, const RawPos &pos);
    /// called by ChordPro parser for lines
    virtual void onLine(const std::wstring& line, const RawPos &pos);
private:
    /// internal flag set during transposition process indicating that whole line of text should be transposed
    bool mTransposeLines;
    /// internal list of text fragments to be transposed
    std::vector<RawPos> mTransposedFragments;
};

/** \brief Reader of metadata (title, subtitle, etc.) from string in ChordPro format */
class InfoReader: public EventHandler
{
public:
    /// Ctor
    InfoReader(const std::wstring &s) : mSource(s), mParsed(false) {};
    /// Getter for Title parsed from text provided to Ctor
    std::wstring getTitle()
    {
        if (!mParsed) parse();
        return mTitle;
    };
    /// Getter for Subtitle parsed from text provided to Ctor
    std::wstring getSubTitle()
    {
        if (!mParsed) parse();
        return mSubTitle;
    };

private:
    /// internal storage for text to be parsed
    const std::wstring &mSource;
    /// internal storage for parsed title
    std::wstring mTitle;
    /// internal storage for parsed subtitle
    std::wstring mSubTitle;
    /// flag indicating if text is parsed (parsing is done on first read of metadata attribute)
    bool mParsed;

    /// internal method which does parsing of text provided to Ctor
    void parse();
    /// called by ChordPro parser for commands
    virtual void onCommand(const CommandType command, const std::wstring& value, const RawPos &pos);
};

} // namespace

#endif // BSCHORDPRO_H
