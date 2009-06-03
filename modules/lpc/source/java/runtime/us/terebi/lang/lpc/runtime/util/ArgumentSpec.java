/* ------------------------------------------------------------------------
 * $Id$
 * Copyright 2009 Tim Vernum
 * ------------------------------------------------------------------------
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * ------------------------------------------------------------------------
 */

package us.terebi.lang.lpc.runtime.util;

import us.terebi.lang.lpc.runtime.ArgumentDefinition;
import us.terebi.lang.lpc.runtime.LpcType;

public class ArgumentSpec implements ArgumentDefinition
{
    private final String _name;
    private final LpcType _type;
    private final boolean _ref;
    private final boolean _varargs;

    public ArgumentSpec(String name, LpcType type)
    {
        this(name, type, false, false);
    }

    public ArgumentSpec(String name, LpcType type, boolean ref, boolean varargs)
    {
        _name = name;
        _type = type;
        _ref = ref;
        _varargs = varargs;
    }

    public String getName()
    {
        return _name;
    }

    public LpcType getType()
    {
        return _type;
    }

    public boolean isRef()
    {
        return _ref;
    }

    public boolean isVarArgs()
    {
        return _varargs;
    }

    public String toString()
    {
        StringBuilder builder = new StringBuilder();
        builder.append(_type);
        if (_ref)
        {
            builder.append(" ref");
        }
        builder.append(" ");
        builder.append(_name);
        if (_varargs)
        {
            builder.append("...");
        }
        return builder.toString();
    }
}
