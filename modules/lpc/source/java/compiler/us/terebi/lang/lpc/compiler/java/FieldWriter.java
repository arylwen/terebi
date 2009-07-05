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

package us.terebi.lang.lpc.compiler.java;

import us.terebi.lang.lpc.compiler.java.context.CompileContext;
import us.terebi.lang.lpc.parser.ast.ASTFields;
import us.terebi.lang.lpc.parser.ast.ASTVariable;
import us.terebi.lang.lpc.parser.ast.ParserVisitor;
import us.terebi.lang.lpc.runtime.MemberDefinition;

/**
 * 
 */
public class FieldWriter extends MemberWriter implements ParserVisitor
{
    public FieldWriter(CompileContext context)
    {
        super(context);
    }

    public Object visit(ASTFields node, Object data)
    {
        return node.childrenAccept(this, data);
    }

    public Object visit(ASTVariable node, Object data)
    {
        CharSequence modifiers = getModifierList(MemberDefinition.Kind.FIELD);
        new VariableWriter(getContext(), getType()).writeField(node, modifiers);
        return data;
    }

}