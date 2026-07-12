#ifndef _AL_GUITEXTINPUTH_
#define _AL_GUITEXTINPUTH_

constexpr uint32_t npDocumentTextColors_size = 3;
const alColor npDocumentTextColors_light[] =
{
	0xFF000000, // text
	0xFFFF8A23, // numbers
	0xFFFF6E00, // "text"
	0xFF999900, // punct
};

class alGUITextInput : public alGUIElement
{
	alGUIFont* m_font = 0;
	alGUIFont* m_fontUI = 0;
	

	void _goUp();
	void _goDown();
	void _goLeft();
	void _goRight();
	void _delete();
	void _backspace();
	void _goHome();
	void _goEnd();
	void _goEndOfText(); // ctrl + end
	void _goHomeOfText(); // ctrl + home
	void _goPageUp();
	void _goPageDown();
	void _showTextCursor();
	void OnCtrlX();
	void Undo();
	void Redo();

	bool m_isSelected = false;
	size_t m_selectionStart = 0;
	size_t m_selectionEnd = 0;
	size_t m_textCursorPositionNext = 0;

	alVec4f m_editorArea;
	// только для текста
	alVec4f m_editorAreaText;
	alVec4f m_bottomBarBG;
	alVec4f m_lineBar;
	char32_t m_lineBarCharBuffer[50];

	bool m_isEditActive = true;
	bool m_drawTextCursor = false;
	alVec4f m_textCursorRect;
	float32_t m_textCursorTimer = 0.f;
	float32_t m_textCursorTimerLimit = 0.5f;
	size_t m_textCursorPosition = 0;
	bool m_cursorInTextEditor = false;
	size_t m_drawStartIndex = 0;
	bool m_isLastCharVisible = false;

	float m_horizontal_offset = 0.f;
	size_t m_drawLastIndex = 0;
	size_t m_drawStartIndexLine = 1;

	struct scrollbar_data
	{
		bool m_needToDraw = false;
		float32_t m_size = 12.f;
		alVec4f m_rectBG;
		alVec4f m_rectangle;
		alVec4f m_btn1Rect;
		alVec4f m_btn2Rect;
		bool m_cursorHold = false;
		float32_t m_saveOnClick = 0.f;
		float32_t m_saveOnClickMouseCoord = 0.f;
	};
	scrollbar_data m_scrollV;
	scrollbar_data m_scrollH;

	bool m_clickedLMB = false;

	float32_t m_maxLineWidth = 0.f;
	int32_t m_fontEditorSize = 10;

	uint32_t m_tabCounter = 0;
	
	uint32_t m_textCursorLine = 1;
	//uint32_t m_textCursorLineOnClick = 1; // save when click lmb
	size_t m_textCursorPosOnClick = 0; // for double\triple click
	uint32_t m_textCursorCol = 1;
	float32_t m_hscrollRightSideIndent = 150.f;
	float32_t m_hscroll_widthLen = 0.f;
	size_t m_numOfVisibleLines = 0;
	float32_t m_nextCharPositionOffset = 0.f;
	float32_t m_nextLinePositionOffset = 0.f;
	alString_base<char32_t> m_textData;
	uint32_t m_textCursorCol_prefer = 1;
	bool m_getCursorColPrefer = false;
	bool m_find_horizontal_offset = false;
	bool m_textFindMode = false;
	uint32_t m_charForMousePick = 0;
	float32_t m_2clickTimer = 0.f;
	uint32_t m_2clickCount = 0;
	float32_t m_3clickTimer = 0.f;
	uint32_t m_3clickCount = 0;
	void _updateRects();
	void _updateData();
	void _updateData_textData(size_t index);
	float32_t m_bottomBarHeight = 14.f;
	float32_t m_textAreaHeight = 0.f;
	size_t m_lineNum = 1;
	void _moveViewToLine(uint32_t);
	void _moveUpView(uint32_t);
	void _moveDownView(uint32_t);
	size_t _moveIndexIntoCol1(size_t index);
	size_t _moveIndexIntoEndOfTheLine(size_t index);
	void _findNeedToDrawHScroll();
	void _showTextEditPopup();
	bool m_textMouseScroll = false;
	void _findHOffsetFromTextCursorPosition();
	size_t m_textCursorPositionOnClick = 0;
	void _moveViewIntoTextCursor();
	float32_t m_textMouseScrollTimerLimit = 0.1f;
	bool m_textMouseScrollUp = true;
	float32_t m_textMouseScrollTimer = 0.f;
	

	void _onTab();
	void _onTab_multiInsert(size_t lineNum, uint8_t* set, uint8_t* get);
	bool _onTab_multiDelete(size_t lineNum, uint8_t* set);
	void InsertChar(char32_t c);
	void _findCurLineWidth();
	void _moveTextCursorToColPrefer();
	void _moveTextCursorUp(uint32_t);
	void _moveTextCursorDown(uint32_t);
	//void _moveTextUp();
	//void _moveTextDown();
	struct HistoryNode
	{
		char32_t* m_string = 0;
		size_t m_charNum = 0;
		size_t m_charPosition = 0;
		bool m_backspace = false;

		// Вместо m_tabsLine проще использовать индекс
		// это должен быть m_selectionStart
		size_t m_tabsIndex = 0;
		//size_t m_tabsLine = 1;
		size_t m_tabsNum = 0;
		uint8_t* m_tabsBuffer = 0;

		enum
		{
			type_insert,
			type_delete,
			type_tabsInsert,
			type_tabsDelete,
		};
		uint32_t m_type = 0;
	};
	class History
	{
		/*npArray<HistoryNode> m_undoData;
		npArray<HistoryNode> m_redoData;*/
		alList<HistoryNode> m_undoData;
		alList<HistoryNode> m_redoData;

		char32_t m_prevChar = 0;

		void _addText()
		{
			if (m_buffer.Size())
			{
				HistoryNode node;
				node.m_type = node.type_insert;
				node.m_charPosition = m_firstCharPosition;
				node.m_charNum = (m_buffer.Size());

				size_t sz = (node.m_charNum + 1) * sizeof(char32_t);
				node.m_string = (char32_t*)malloc(sz);
				memcpy(node.m_string, m_buffer.c_str(), sz);

				m_undoData.push_back(node);

				m_buffer.Clear();
			}

			m_firstCharPosition = -1;
			//	m_lastTextPosition = 0;
		}

		void _deleteRedo()
		{
			for (auto o : m_redoData)
			{
				if (o.m_tabsBuffer)
					free(o.m_tabsBuffer);

				if (o.m_string)
					free(o.m_string);
			}
			m_redoData.clear();
		}
	public:
		History() {}
		~History()
		{
			_deleteRedo();
			for (auto o : m_undoData)
			{
				if (o.m_tabsBuffer)
					free(o.m_tabsBuffer);
				if (o.m_string)
					free(o.m_string);
			}
		}
		alUnicodeString m_buffer;
		size_t m_firstCharPosition = -1;

		bool IsHasUndo() { return m_undoData.size() > 0; }
		bool IsHasRedo() { return m_redoData.size() > 0; }

		void AddUndo()
		{
			_addText();
		}

		void AddUndo(const HistoryNode& node)
		{
			m_undoData.push_back(node);
		}

		void OnAddChar(
			char32_t c,
			size_t charPosition
		)
		{
			_deleteRedo();

			//m_cursorPositionAfter = cursorPositionAfter;

			if (m_firstCharPosition == -1)
				m_firstCharPosition = charPosition;
			//	m_lastTextPosition = textCursorPosition;

			m_buffer.PushBack(c);
			if (c == ' '
				|| c == '\t'
				|| c == '\r'
				|| c == '\n')
			{
				if (m_prevChar != '\t'
					&& m_prevChar != ' '
					&& m_prevChar != '\r')
				{
					_addText();
				}
				else if (c == '\n')
				{
					_addText();
				}
			}

			m_prevChar = c;
		}
		void OnPasteText(char32_t c, size_t charPosition)
		{
			_deleteRedo();

			if (m_firstCharPosition == -1)
				m_firstCharPosition = charPosition;
			//	m_lastTextPosition = textCursorPosition;
			m_buffer.PushBack(c);

			m_prevChar = c;
		}

		void OnDeleteSelected(const char32_t* string, size_t num, size_t textCursorPosition)
		{
			_deleteRedo();
			HistoryNode node;
			node.m_type = node.type_delete;
			node.m_charPosition = textCursorPosition;
			node.m_backspace = false;
			node.m_charNum = num;
			size_t sz = num * sizeof(char32_t);
			node.m_string = (char32_t*)malloc(sz);
			for (size_t i = num - 1, o = 0; i >= 0;)
			{
				node.m_string[o] = string[i];
				if (!i)
					break;
				--i;
				++o;
			}
			m_undoData.push_back(node);
			m_buffer.Clear();
		}

		void OnDelete(char32_t c, size_t textCursorPosition, bool backspace)
		{
			_deleteRedo();

			HistoryNode node;
			node.m_type = node.type_delete;
			node.m_charPosition = textCursorPosition;
			node.m_backspace = backspace;
			node.m_charNum = 1;
			size_t sz = 1 * sizeof(char32_t);
			node.m_string = (char32_t*)malloc(sz);
			node.m_string[0] = c;
			m_undoData.push_back(node);
			m_buffer.Clear();
		}

		bool Undo(HistoryNode* node)
		{
			bool r = false;
			if (m_buffer.Size())
			{
				_addText();
			}

			if (m_undoData.size())
			{
				*node = m_undoData.back();
				r = true;
				m_undoData.pop_back();
				m_redoData.push_back(*node);
			}

			return r;
		}

		bool Redo(HistoryNode* node)
		{
			bool r = false;
			if (m_redoData.size())
			{
				*node = m_redoData.back();
				r = true;
				m_redoData.pop_back();
				m_undoData.push_back(*node);
			}

			return r;
		}
	};
	History m_history;
public:
	alGUITextInput(alGUIContext* ct);
	virtual ~alGUITextInput();

	virtual void Draw(float32_t dt) override;
	virtual void Update(float32_t dt) override;
	virtual void Rebuild() override;

	void SetFont(alGUIFont* text, alGUIFont* ui);
	void SetText(const char32_t* text, ...);
	void SetText(const char32_t* text, size_t sz);
	void Clear();
	
	bool IsSelected() { return m_isSelected; }
	bool IsActive() { return m_isEditActive; }

	void Activate();
	void Deactivate(bool isEnter);
	void DeleteSelected(bool addToHistory = true);
	void DeleteAll();
	void DeselectAll();
	void SelectAll();
	void CutToClipboard();
	void CopyToClipboard();
	void PasteFromClipboard();
	void GoTo(uint32_t line, uint32_t col, bool tabs);
	void Select(size_t begin, size_t end);
	void Select(size_t num);

	alUnicodeString m_text;

	bool m_drawBG = true;
	float32_t m_lineSpacing = 2.f;
	uint32_t m_tabSize = 4;

	bool m_useHorizontalScrollbar = true;
	bool m_useVerticalScrollbar = true;
	bool m_useLinebar = true;
	bool m_useBottombar = true;


	// if true then only draw
	bool m_ignoreInput = false;

	// 0 - unlimited number of characters
	uint32_t m_charLimit = 0;

	// for multiline
	bool m_deactivateOnEscape = false;

	bool m_oneLine = false;

	alUnicodeString m_textDefault;
	alColor m_colorDefaultText;

	// When we enter character or paste text
	virtual bool OnCharacter(char32_t c) { if (c == '\r')return false; return true; }

	// This is for one line Text Input
	// When we press Enter or click outside
	virtual void OnAccept() {}
	// When we press Escape
	// This is just notification, reset text by yourself
	virtual void OnCancel() {}

	enum class WordTypeBasic
	{
		unknown,
		space, // кроме \n
		punct,
		word,
		number,
		new_line,
	};
	struct WordInfo
	{
		size_t m_begin = 0;
		size_t m_end = 0;
		WordTypeBasic m_type = WordTypeBasic::unknown;
		uint32_t m_color = 0;
	};
	void GetWordInfo(size_t pos, WordInfo*);
};

#endif

