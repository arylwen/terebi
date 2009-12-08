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

package us.terebi.lang.lpc.compiler.bytecode;

import java.util.Set;

import org.adjective.stout.core.UnresolvedType;

import us.terebi.lang.lpc.runtime.util.type.DynamicClassDefinition;

/**
 * 
 */
public class CompiledClassDefinition extends DynamicClassDefinition
{
    private final UnresolvedType _implementation;

    public CompiledClassDefinition(String name, Set< ? extends Modifier> modifiers, UnresolvedType implementation)
    {
        super(name, modifiers);
        _implementation = implementation;
    }
    
    public UnresolvedType getImplementatingClass()
    {
        return _implementation;
    }
}