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
import java.util.Map;

import us.terebi.lang.lpc.runtime.ArgumentDefinition;
import us.terebi.lang.lpc.runtime.ArgumentSemantics;
import us.terebi.lang.lpc.runtime.Callable;
import us.terebi.lang.lpc.runtime.FunctionSignature;
import us.terebi.lang.lpc.runtime.LpcType;
import us.terebi.lang.lpc.runtime.LpcValue;
import us.terebi.lang.lpc.runtime.MethodDefinition;
import us.terebi.lang.lpc.runtime.ObjectInstance;
import us.terebi.lang.lpc.runtime.jvm.context.CallStack;
import us.terebi.lang.lpc.runtime.jvm.context.RuntimeContext;
import us.terebi.lang.lpc.runtime.jvm.type.Types;
import us.terebi.lang.lpc.runtime.jvm.value.NilValue;
import us.terebi.lang.lpc.runtime.util.ArgumentSpec;

/**
 * 
 */
public class CallOtherEfun extends AbstractEfun implements FunctionSignature, Callable
{
    public List< ? extends ArgumentDefinition> getArguments()
    {
        ArrayList<ArgumentDefinition> list = new ArrayList<ArgumentDefinition>();
        list.add(new ArgumentSpec("object", Types.OBJECT));
        list.add(new ArgumentSpec("method", Types.STRING));
        list.add(new ArgumentSpec("arguments", Types.MIXED, ArgumentSemantics.BY_VALUE, true));
        return list;
    }

    public LpcType getReturnType()
    {
        return Types.MIXED;
    }

    public LpcValue execute(List< ? extends LpcValue> arguments)
    {
        checkArguments(arguments);
        return callOther(arguments);
    }

    private LpcValue callOther(List< ? extends LpcValue> arguments)
    {
        LpcValue arg1 = arguments.get(0);
        LpcValue arg2 = arguments.get(1);
        List< ? extends LpcValue> args = arguments.subList(2, arguments.size());

        ObjectInstance target = arg1.asObject();
        String name = arg2.asString();

        return callOther(target, name, args);
    }

    public static LpcValue callOther(ObjectInstance target, String name, List< ? extends LpcValue> args)
    {
        Map<String, ? extends MethodDefinition> methods = target.getDefinition().getMethods();
        MethodDefinition method = methods.get(name);
        if (method == null)
        {
            return NilValue.INSTANCE;
        }

        CallStack stack = RuntimeContext.get().callStack();
        stack.pushFrame(CallStack.Origin.CALL_OTHER, target);
        try
        {
            return method.execute(target, args);
        }
        finally
        {
            stack.popFrame();
        }
    }

}