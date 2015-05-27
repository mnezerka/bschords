# ChordPro Song File Format Reference #

The Chordpro format is used for the notation of lyrics with chords in plain text format.

A ChordPro data file is a simple text file, which is interpreted line by line. Line endings should obey the rules of the platform. Lines that start with a '#' symbol are ignored. Lines that start with { and end with } (curly brackets or braces) are directives. They are used to pass instructions to the program that processes the data.

Lines starting with `#` are treated as comments and are ignored

## Chords ##

Chords are delimited by square brackets `[` and `]`. Sharp is indicated by a `#` (hash) and flat by a `b` (lowercase `B`).

## Song lines ##

Lines that are not directives are song lines. They contain the lyrics of the song. Between the lyrics the chords are specified between `[` and `]` (square brackets). Chords should immediately precede the syllable of text they apply to. For example:

```
Swing [D]low, sweet [G]chari[D]ot,
```

When processed, this results in:

```
      D          G    D
Swing low, sweet chariot,
```

## Directives ##

Directives are written within curly brackets `{` and `}`. Following set of directives is recoginzed by bschords application (upper or lower case is not significant):

  * `{new_song}`, `{ns}` - Starts a new song. This is implied at the start of a ChordPro data file.
  * `{title: text}`, `{t:text}` - Specifies the title of the song. The title is used to sort the songs in the user interface. It usually appears at the top of the song, and may be repeated if the song overflows onto a new column.
  * `{subtitle: text}`, `{su: text}` - Specifies a subtitle for the song. This string will be usually visible printed close to the song title.
  * `{comment: text}`, `{c: text}` - Prints the string following the colon as a comment. It can be used for example to call a chorus.
  * `{start_of_chorus}`, `{soc}` - Indicates the start of a chorus. Songsheet Generator can apply a special marking to highlight the chorus, depending on the setting of the output destination. The marking is applied until the end\_of\_chorus directive appears.
  * `{end_of_chorus}`, `{eoc}` - Marks the end of a chorus.
  * `{start_of_tab}`, `{sot}` - Indicates the start of a guitar tab section. The text will be formatted in a fixed-width font until the end\_of\_tab directive appears. This can be useful for creating a tab drawing with ASCII text.
  * `{end_of_tab}`, `{eot}` - Marks the end of the guitar tab section.


Not supported directives. Such directives are ignored and appropriate text lines are removed before showing (printing) the song.

  * `{comment_italic: text}`, `{ci: text}` - same directive as `comment`, but shown most likely using an italic font.
  * `{comment_box: text}`, `{cb: text}` - same directive as `comment`, but comments have a coloured background.
  * `{album: text}, `{a: text}` - Album. Usually composer or artist
  * `{guitar_comment: text}`, `{gc:string}` - Prints the string following the colon as a comment. This comment will only be printed if chords are also printed; it should be used for comments to performers, or for other notes that are unneccessary for lyrics-only song sheets (or projection).
  * `{new_page}`, `{np}` - This directive will cause a "cell break" in the Two and Four Discrete Cells per Page printing modes, and a column break in the Two Flowing Columns printing mode. It will cause a physical page break otherwise. It has no effect in the Text and HTML File output destinations.
  * `{define ... }` - Defines a guitar chord.
  * `{textfont: fontname}` - Sets the font used to print text to `fontname`.
  * `{chordfont: fontname}` - Sets the font used to print chords to `fontname`.
  * `{textsize: fontsize}` - Sets the size of the font used for texts.
  * `{chordsize: fontsize}` - Sets the size of the font used for chords.
  * `{no_grid}`, `{ng}` - Suppresses printing of the list of chords at the end of the current song.
  * `{grid}`, `{g}` - Enables printing of the list of chords at the end of the current song.
  * `{titles: flush}` - If `flush` is `left`, song titles are printed to the left of the page. If flush is `center`, or if this directive is missing, titles are printed centered on top of the page.
  * `{new_physical_page}`, `{npp}` - Forces a physical page break.
  * `{columns: number}`, `{col: number}` - Specifies the number of columns to print the current song in.
  * `{column_break}`, `{colb}` - Forces a column break.
  * `{pagetype: type}` - Sets the page type for the output.
  * `{tuning: text}` -
  * `{key: text}` -
  * `{tempo: text}` -






