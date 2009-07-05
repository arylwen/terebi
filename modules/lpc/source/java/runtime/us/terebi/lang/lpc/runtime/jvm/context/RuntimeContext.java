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

package us.terebi.lang.lpc.runtime.jvm.context;

/**
 * 
 */
public class RuntimeContext
{
    private static final ThreadLocal<RuntimeContext> CONTEXT = new ThreadLocal<RuntimeContext>();

    public static RuntimeContext get()
    {
        return CONTEXT.get();
    }

    public static void set(RuntimeContext context)
    {
        CONTEXT.set(context);
    }

    private final Functions _functions;
    private final ObjectManager _objectManager;
    private  final CallStack _callStack;

    public RuntimeContext(Functions functions, ObjectManager objectManager)
    {
        _functions = functions;
        _objectManager = objectManager;
        _callStack = new CallStack();
    }

    public Functions functions()
    {
        return _functions;
    }

    public ObjectManager objectManager()
    {
        return _objectManager;
    }

    public CallStack callStack()
    {
        return _callStack;
    }

}
