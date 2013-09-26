#ifndef BSCHORDS_SONG_H
#define BSCHORDS_SONG_H

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <vector>
#include "bschordpro.h"

namespace bschords
{
	struct SongLineItem
	{
		enum LineItemType { BLTYPE_NONE, BLTYPE_TEXT, BLTYPE_CHORD };
		wxString *txt;
		virtual LineItemType getType() = 0;
	};

	struct SongLineText: public SongLineItem
	{
		virtual LineItemType getType() { return BLTYPE_TEXT; };
	};

	struct SongLineChord: public SongLineItem
	{
		virtual LineItemType getType() { return BLTYPE_CHORD; };
	};

	struct SongLine
	{
		std::vector<SongLineItem*> mItems;
		virtual ~SongLine()
		{
			while (!mItems.empty()) { delete mItems.back(); mItems.pop_back(); }
		}
	};

	struct SongBlock
	{
		enum BlockType { BLTYPE_NONE, BLTYPE_TEXT, BLTYPE_CHORUS, BLTYPE_HSPACE, BLTYPE_TITLE, BLTYPE_TAB, BLTYPE_STRUCT, BLTYPE_VERSE };

		virtual ~SongBlock() { }
		virtual BlockType getType() { return BLTYPE_NONE; };
	};

	struct SongHSpace : public SongBlock
	{
	};

	struct SongTitle : public SongBlock
	{
		wxString mTitle;
		virtual ~SongTitle() { }
		virtual BlockType getType() { return BLTYPE_TITLE; };
	};

	struct SongText : public SongBlock
	{
		std::vector<SongLine*> mLines;
		virtual ~SongText() { while (!mLines.empty()) { delete mLines.back(); mLines.pop_back(); } }
		virtual BlockType getType() { return BLTYPE_TEXT; };
		virtual bool hasChords();
		virtual bool hasNumbering() { return false; };
	};

	struct SongVerse : public SongText
	{
		unsigned int m_counter;
		virtual BlockType getType() { return BLTYPE_VERSE; };
		virtual bool hasNumbering();
	};

	struct SongChorus : public SongText
	{

		virtual BlockType getType() { return BLTYPE_CHORUS; };
	};

	struct SongTab : public SongBlock
	{
		std::vector<wxString> mLines;
				virtual BlockType getType() { return BLTYPE_TAB; };
	};

	struct SongStructItem {
		SongStructItem(wxString &chord, float size) : mChord(chord), mSize(size) { };
		wxString mChord;
		float mSize;
	};

	struct SongStruct : public SongBlock
	{
		std::vector<wxString> mLines;
		virtual BlockType getType() { return BLTYPE_STRUCT; };
		private:
			//std::vector<wxString> prepare();
			std::vector< std::vector<SongStructItem> > prepare();
			void getSizeParams(std::vector< std::vector<SongStructItem> > items, size_t &numCellSize, float &numMinSize, float &numMaxLineSize, size_t &numSeparatorSize);
	};

	class Song: public bschordpro::EventHandler
	{
		public:
			void load(wxString filePath);
			void save(wxString filePath);
		private:
			std::vector<SongBlock> mBlocks;
			virtual void onBegin();
			virtual void onEnd();
			virtual void onLineBegin();
			virtual void onLineEnd();
			virtual void onCommand(const bschordpro::CommandType command, const std::wstring& value);
			virtual void onChord(const std::wstring& chord);
			virtual void onText(const std::wstring& text);
			virtual void onLine(const std::wstring& line);
	};
}

#endif // BSCHORDS_SONG_H
