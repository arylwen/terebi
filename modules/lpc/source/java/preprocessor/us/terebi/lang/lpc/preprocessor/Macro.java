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

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

/**
 * A macro object.
 *
 * This encapsulates a name, an argument count, and a token stream
 * for replacement. The replacement token stream may contain the
 * extra tokens {@link Token#M_ARG} and {@link Token#M_STRING}.
 */
public class Macro
{
    private final String _name;
    private final List<Token> _tokens;
    /* It's an explicit decision to keep these around here. We don't
     * need to; the argument token type is M_ARG and the value
     * is the index. The strings themselves are only used in
     * stringification of the macro, for debugging. */
    private List<String> _args;
    private boolean _variadic;

    public Macro(String name)
    {
        this._name = name;
        this._args = null;
        this._variadic = false;
        this._tokens = new ArrayList<Token>();
    }

    /**
     * Returns the name of this macro.
     */
    public String getName()
    {
        return _name;
    }

    /**
     * Sets the arguments to this macro.
     */
    public void setArgs(List<String> args)
    {
        this._args = args;
    }

    /**
     * Returns true if this is a function-like macro.
     */
    public boolean isFunctionLike()
    {
        return _args != null;
    }

    /**
     * Returns the number of arguments to this macro.
     */
    public int getArgs()
    {
        return _args.size();
    }

    /**
     * Sets the variadic flag on this Macro.
     */
    public void setVariadic(boolean b)
    {
        this._variadic = b;
    }

    /**
     * Returns true if this is a variadic function-like macro.
     */
    public boolean isVariadic()
    {
        return _variadic;
    }

    /**
     * Adds a token to the expansion of this macro.
     */
    public void addToken(Token tok)
    {
        this._tokens.add(tok);
    }

    /**
     * Adds a "paste" operator to the expansion of this macro.
     *
     * A paste operator causes the next token added to be pasted
     * to the previous token when the macro is expanded.
     * It is an error for a macro to end with a paste token.
     */
    public void addPaste(Token tok)
    {
        /*
         * Given: tok0 		 * We generate: M_PASTE, tok0, tok1
         * This extends as per a stack language:
         * tok0 		 *   M_PASTE, tok0, M_PASTE, tok1, tok2
         */
        this._tokens.add(_tokens.size() - 1, tok);
    }

    /* pp */List<Token> getTokens()
    {
        return _tokens;
    }

    public String getText()
    {
        StringBuilder buf = new StringBuilder();
        boolean paste = false;
        for (int i = 0; i < _tokens.size(); i++)
        {
            Token tok = _tokens.get(i);
            if (tok.getType() == Token.M_PASTE)
            {
                paste = true;
                continue;
            }
            else
            {
                buf.append(tok.getText());
            }
            if (paste)
            {
                buf.append(" #" + "# ");
                paste = false;
            }
            // buf.append(tokens.get(i));
        }
        return buf.toString();
    }

    public String toString()
    {
        StringBuilder buf = new StringBuilder(_name);
        if (_args != null)
        {
            buf.append('(');
            Iterator<String> it = _args.iterator();
            while (it.hasNext())
            {
                buf.append(it.next());
                if (it.hasNext())
                    buf.append(", ");
                else if (isVariadic())
                    buf.append("...");
            }
            buf.append(')');
        }
        if (!_tokens.isEmpty())
        {
            buf.append(" => ").append(getText());
        }
        return buf.toString();
    }

}
