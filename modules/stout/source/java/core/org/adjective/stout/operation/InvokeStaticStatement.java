/* ------------------------------------------------------------------------
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

package org.adjective.stout.operation;

import org.adjective.stout.core.ExecutionStack;
import org.adjective.stout.core.InstructionCollector;
import org.adjective.stout.core.MethodSignature;
import org.adjective.stout.core.UnresolvedType;

/**
 * @author <a href="http://blog.adjective.org/">Tim Vernum</a>
 */
public class InvokeStaticStatement extends SmartStatement
{
    private final InvokeStaticOperation _operation;

    public InvokeStaticStatement(UnresolvedType targetType, MethodSignature method, Expression... arguments)
    {
        _operation = new InvokeStaticOperation(targetType, method, arguments);
    }

    public void getInstructions(ExecutionStack stack, InstructionCollector collector)
    {
        _operation.getInstructions(stack, collector);
        if (_operation.hasReturnValue())
        {
            PopExpression.INSTANCE.getInstructions(stack, collector);
        }
    }

}
