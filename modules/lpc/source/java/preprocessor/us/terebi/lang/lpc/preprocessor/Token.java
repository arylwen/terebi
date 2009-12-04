/*
 * Anarres C Preprocessor
 * Copyright (c) 2007-2008, Shevek
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
 * or implied.  See the License for the specific language governing
 * permissions and limitations under the License.
 */

package us.terebi.lang.lpc.preprocessor;

/**
 * A Preprocessor token.
 *
 * @see Preprocessor
 */
public final class Token {

	// public static final int	EOF        = -1;

	private int		type;
	private int		line;
	private int		column;
	private Object	value;
	private String	text;

	public Token(int type, int line, int column,
					String text, Object value) {
		this.type = type;
		this.line = line;
		this.column = column;
		this.text = text;
		this.value = value;
	}

	public Token(int type, int line, int column, String text) {
		this(type, line, column, text, null);
	}

	/* pp */ Token(int type, String text, Object value) {
		this(type, -1, -1, text, value);
	}

	/* pp */ Token(int type, String text) {
		this(type, text, null);
	}

	/* pp */ Token(int type) {
		this(type, texts[type]);
	}

	/**
	 * Returns the semantic type of this token.
	 */
	public int getType() {
		return type;
	}

	/* pp */ void setLocation(int line, int column) {
		this.line = line;
		this.column = column;
	}

	/**
	 * Returns the line at which this token started.
	 *
	 * Lines are numbered from zero.
	 */
	public int getLine() {
		return line;
	}

	/**
	 * Returns the column at which this token started.
	 *
	 * Columns are numbered from zero.
	 */
	public int getColumn() {
		return column;
	}

	/**
	 * Returns the original or generated text of this token.
	 *
	 * This is distinct from the semantic value of the token.
	 *
	 * @see #getValue()
	 */
	public String getText() {
		return text;
	}

	/**
	 * Returns the semantic value of this token.
	 *
	 * For strings, this is the parsed String.
	 * For integers, this is an Integer object.
	 * For other token types, as appropriate.
	 *
	 * @see #getText()
	 */
	public Object getValue() {
		return value;
	}

	/**
	 * Returns a description of this token, for debugging purposes.
	 */
	public String toString() {
		StringBuilder	buf = new StringBuilder();

		buf.append('[').append(getTokenName(type));
		if (line != -1) {
			buf.append('@').append(line);
			if (column != -1)
				buf.append(',').append(column);
		}
		buf.append("]:");
		if (text != null)
			buf.append('"').append(text).append('"');
		else if (type > 3 && type < 256)
			buf.append( (char)type );
		else
			buf.append('<').append(type).append('>');
		if (value != null)
			buf.append('=').append(value);
		return buf.toString();
	}

	/**
	 * Returns the descriptive name of the given token type.
	 *
	 * This is mostly used for stringification and debugging.
	 */
	public static final String getTokenName(int type) {
		if (type < 0)
			return "Invalid" + type;
		if (type >= names.length)
			return "Invalid" + type;
		if (names[type] == null)
			return "Unknown" + type;
		return names[type];
	}

	/** The token type AND_EQ. */
	public static final int AND_EQ = 257;
	/** The token type ARROW. */
	public static final int ARROW = 258;
	/** The token type CHARACTER. */
	public static final int CHARACTER = 259;
	/** The token type COMMENT. */
	public static final int COMMENT = 260;
	/** The token type DEC. */
	public static final int DEC = 261;
	/** The token type DIV_EQ. */
	public static final int DIV_EQ = 262;
	/** The token type ELLIPSIS. */
	public static final int ELLIPSIS = 263;
	/** The token type EOF. */
	public static final int EOF = 264;
	/** The token type EQ. */
	public static final int EQ = 265;
	/** The token type GE. */
	public static final int GE = 266;
	/** The token type HASH. */
	public static final int HASH = 267;
	/** The token type HEADER. */
	public static final int HEADER = 268;
	/** The token type IDENTIFIER. */
	public static final int IDENTIFIER = 269;
	/** The token type INC. */
	public static final int INC = 270;
	/** The token type INTEGER. */
	public static final int INTEGER = 271;
	/** The token type LAND. */
	public static final int LAND = 272;
	/** The token type LAND_EQ. */
	public static final int LAND_EQ = 273;
	/** The token type LE. */
	public static final int LE = 274;
	/** The token type LITERAL. */
	public static final int LITERAL = 275;
	/** The token type LOR. */
	public static final int LOR = 276;
	/** The token type LOR_EQ. */
	public static final int LOR_EQ = 277;
	/** The token type LSH. */
	public static final int LSH = 278;
	/** The token type LSH_EQ. */
	public static final int LSH_EQ = 279;
	/** The token type MOD_EQ. */
	public static final int MOD_EQ = 280;
	/** The token type MULT_EQ. */
	public static final int MULT_EQ = 281;
	/** The token type NE. */
	public static final int NE = 282;
	/** The token type NL. */
	public static final int NL = 283;
	/** The token type OR_EQ. */
	public static final int OR_EQ = 284;
	/** The token type PASTE. */
	public static final int PASTE = 285;
	/** The token type PLUS_EQ. */
	public static final int PLUS_EQ = 286;
	/** The token type RANGE. */
	public static final int RANGE = 287;
	/** The token type RSH. */
	public static final int RSH = 288;
	/** The token type RSH_EQ. */
	public static final int RSH_EQ = 289;
	/** The token type STRING. */
	public static final int STRING = 290;
	/** The token type SUB_EQ. */
	public static final int SUB_EQ = 291;
	/** The token type WHITESPACE. */
	public static final int WHITESPACE = 292;
	/** The token type XOR_EQ. */
	public static final int XOR_EQ = 293;
	/** The token type M_ARG. */
	public static final int M_ARG = 294;
	/** The token type M_PASTE. */
	public static final int M_PASTE = 295;
	/** The token type M_STRING. */
	public static final int M_STRING = 296;
	/** The token type P_LINE. */
	public static final int P_LINE = 297;
	/** The token type ERROR. */
	public static final int ERROR = 298;
	/**
	 * The number of possible semantic token types.
	 *
	 * Please note that not all token types below 255 are used.
	 */
	public static final int _TOKENS = 299;

	/** The position-less space token. */
	/* pp */ static final Token	 SPACE = new Token(WHITESPACE, -1, -1, " ");

	private static final String[] names = new String[_TOKENS];
	private static final String[] texts = new String[_TOKENS];
	static {
		for (int i = 0; i < 255; i++) {
			texts[i] = String.valueOf(new char[] { (char)i });
			names[i] = texts[i];
		}

		texts[AND_EQ]      = "&=";
		texts[ARROW]       = "->";
		texts[DEC]         = "--";
		texts[DIV_EQ]      = "/=";
		texts[ELLIPSIS]    = "...";
		texts[EQ]          = "==";
		texts[GE]          = ">=";
		texts[HASH]        = "#";
		texts[INC]         = "++";
		texts[LAND]        = "&&";
		texts[LAND_EQ]     = "&&=";
		texts[LE]          = "<=";
		texts[LOR]         = "||";
		texts[LOR_EQ]      = "||=";
		texts[LSH]         = "<<";
		texts[LSH_EQ]      = "<<=";
		texts[MOD_EQ]      = "%=";
		texts[MULT_EQ]     = "*=";
		texts[NE]          = "!=";
		texts[NL]          = "\n";
		texts[OR_EQ]       = "|=";
		/* We have to split the two hashes or Velocity eats them. */
		texts[PASTE]       = "#" + "#";
		texts[PLUS_EQ]     = "+=";
		texts[RANGE]       = "..";
		texts[RSH]         = ">>";
		texts[RSH_EQ]      = ">>=";
		texts[SUB_EQ]      = "-=";
		texts[XOR_EQ]      = "^=";

		names[AND_EQ] = "AND_EQ";
		names[ARROW] = "ARROW";
		names[CHARACTER] = "CHARACTER";
		names[COMMENT] = "COMMENT";
		names[DEC] = "DEC";
		names[DIV_EQ] = "DIV_EQ";
		names[ELLIPSIS] = "ELLIPSIS";
		names[EOF] = "EOF";
		names[EQ] = "EQ";
		names[GE] = "GE";
		names[HASH] = "HASH";
		names[HEADER] = "HEADER";
		names[IDENTIFIER] = "IDENTIFIER";
		names[INC] = "INC";
		names[INTEGER] = "INTEGER";
		names[LAND] = "LAND";
		names[LAND_EQ] = "LAND_EQ";
		names[LE] = "LE";
		names[LITERAL] = "LITERAL";
		names[LOR] = "LOR";
		names[LOR_EQ] = "LOR_EQ";
		names[LSH] = "LSH";
		names[LSH_EQ] = "LSH_EQ";
		names[MOD_EQ] = "MOD_EQ";
		names[MULT_EQ] = "MULT_EQ";
		names[NE] = "NE";
		names[NL] = "NL";
		names[OR_EQ] = "OR_EQ";
		names[PASTE] = "PASTE";
		names[PLUS_EQ] = "PLUS_EQ";
		names[RANGE] = "RANGE";
		names[RSH] = "RSH";
		names[RSH_EQ] = "RSH_EQ";
		names[STRING] = "STRING";
		names[SUB_EQ] = "SUB_EQ";
		names[WHITESPACE] = "WHITESPACE";
		names[XOR_EQ] = "XOR_EQ";
		names[M_ARG] = "M_ARG";
		names[M_PASTE] = "M_PASTE";
		names[M_STRING] = "M_STRING";
		names[P_LINE] = "P_LINE";
		names[ERROR] = "ERROR";
	}

}
