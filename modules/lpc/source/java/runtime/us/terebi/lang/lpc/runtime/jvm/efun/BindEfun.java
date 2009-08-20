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

package us.terebi.lang.lpc.runtime.jvm.efun;

import java.util.ArrayList;
import java.util.List;

import us.terebi.lang.lpc.runtime.ArgumentDefinition;
import us.terebi.lang.lpc.runtime.Callable;
import us.terebi.lang.lpc.runtime.FunctionSignature;
import us.terebi.lang.lpc.runtime.LpcType;
import us.terebi.lang.lpc.runtime.LpcValue;
import us.terebi.lang.lpc.runtime.ObjectInstance;
import us.terebi.lang.lpc.runtime.jvm.type.Types;
import us.terebi.lang.lpc.runtime.jvm.value.FunctionValue;
import us.terebi.lang.lpc.runtime.util.ArgumentSpec;
import us.terebi.lang.lpc.runtime.util.CallableProxy;

/**
 * 
 */
public class BindEfun extends AbstractEfun implements FunctionSignature, Callable
{
    public static final class BoundFunction extends CallableProxy
    {
        private final ObjectInstance _owner;

        public BoundFunction(Callable func, ObjectInstance owner)
        {
            super(func);
            _owner = owner;
        }

        public ObjectInstance getOwner()
        {
            return _owner;
        }
    }

    protected List< ? extends ArgumentDefinition> defineArguments()
    {
        ArrayList<ArgumentDefinition> list = new ArrayList<ArgumentDefinition>();
        list.add(new ArgumentSpec("func", Types.STRING));
        list.add(new ArgumentSpec("ob", Types.OBJECT));
        return list;
    }

    public LpcType getReturnType()
    {
        return Types.FUNCTION;
    }

    public LpcValue execute(List< ? extends LpcValue> arguments)
    {
        checkArguments(arguments);
        Callable func = arguments.get(0).asCallable();
        ObjectInstance owner = arguments.get(1).asObject();
        // @TODO security check in master object
        return new FunctionValue(new BoundFunction(func, owner));
    }

}
