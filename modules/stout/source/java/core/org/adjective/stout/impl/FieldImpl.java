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

package org.adjective.stout.impl;

import java.util.Set;

import org.adjective.stout.core.AnnotationDescriptor;
import org.adjective.stout.core.ElementModifier;
import org.adjective.stout.core.FieldDescriptor;
import org.adjective.stout.core.UnresolvedType;

/**
 * @author <a href="http://blog.adjective.org/">Tim Vernum</a>
 */
public class FieldImpl implements FieldDescriptor
{
    private final String _name;
    private final UnresolvedType _type;
    private final AnnotationDescriptor[] _annotations;
    private final Set<ElementModifier> _modifiers;

    public FieldImpl(Set<ElementModifier> modifiers, UnresolvedType type, String name, AnnotationDescriptor[] annotations)
    {
        _name = name;
        _type = type;
        _annotations = annotations;
        _modifiers = modifiers;
    }

    public AnnotationDescriptor[] getAnnotations()
    {
        return _annotations;
    }

    public Set<ElementModifier> getModifiers()
    {
        return _modifiers;
    }

    public String getName()
    {
        return _name;
    }

    public UnresolvedType getType()
    {
        return _type;
    }

}
