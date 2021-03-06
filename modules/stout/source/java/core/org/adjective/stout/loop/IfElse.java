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

package org.adjective.stout.loop;

import org.objectweb.asm.Label;
import org.objectweb.asm.Opcodes;

import org.adjective.stout.core.ExecutionStack;
import org.adjective.stout.core.InstructionCollector;
import org.adjective.stout.core.ExecutionStack.Block;
import org.adjective.stout.instruction.JumpInstruction;
import org.adjective.stout.instruction.LabelInstruction;
import org.adjective.stout.operation.SmartStatement;
import org.adjective.stout.operation.Statement;

/**
 * @author <a href="http://blog.adjective.org/">Tim Vernum</a>
 */
public class IfElse extends SmartStatement
{
    private final Condition _condition;
    private final Statement[] _whenTrue;
    private final Statement[] _whenFalse;

    public IfElse(Condition condition, Statement[] whenTrue, Statement[] whenFalse)
    {
        _condition = condition;
        _whenTrue = whenTrue;
        _whenFalse = whenFalse;
    }

    public void getInstructions(ExecutionStack stack, InstructionCollector collector)
    {
        Block block = stack.pushBlock();

        Label endLabel = new Label();
        Label elseLabel = (_whenFalse == null ? endLabel : new Label());

        _condition.jumpWhenFalse(elseLabel).getInstructions(stack, collector);

        for (Statement stmt : _whenTrue)
        {
            stmt.getInstructions(stack, collector);
        }

        if (_whenFalse != null)
        {
            addInstruction(collector,new JumpInstruction(Opcodes.GOTO, endLabel));
            addInstruction(collector,new LabelInstruction(elseLabel));

            for (Statement stmt : _whenFalse)
            {
                stmt.getInstructions(stack, collector);
            }
        }
        
        addInstruction(collector,new LabelInstruction(endLabel));

        stack.popBlock(block);
    }

}
