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

package us.terebi.lang.lpc.runtime.jvm.object;

import java.lang.reflect.Field;
import java.util.Arrays;
import java.util.HashSet;
import java.util.Set;

import us.terebi.lang.lpc.compiler.java.context.ClassFinder;
import us.terebi.lang.lpc.compiler.java.context.CompiledInstance;
import us.terebi.lang.lpc.runtime.ClassDefinition;
import us.terebi.lang.lpc.runtime.FieldDefinition;
import us.terebi.lang.lpc.runtime.LpcType;
import us.terebi.lang.lpc.runtime.LpcValue;
import us.terebi.lang.lpc.runtime.UserTypeDefinition;
import us.terebi.lang.lpc.runtime.UserTypeInstance;
import us.terebi.lang.lpc.runtime.jvm.LpcMember;
import us.terebi.lang.lpc.runtime.jvm.LpcMemberType;
import us.terebi.lang.lpc.runtime.jvm.LpcReference;
import us.terebi.lang.lpc.runtime.jvm.exception.InternalError;
import us.terebi.lang.lpc.runtime.jvm.exception.LpcRuntimeException;
import us.terebi.lang.lpc.runtime.jvm.type.Types;

/**
 * 
 */
public class CompiledField implements FieldDefinition
{
    private final UserTypeDefinition _declaringObject;
    private final Field _field;
    private final String _name;
    private final Set<Modifier> _modifiers;
    private final LpcType _type;

    public CompiledField(UserTypeDefinition declaringObject, Field field)
    {
        // @TODO - Optimise for when field is "final" and "LpcReference"
        _declaringObject = declaringObject;
        _field = field;

        LpcMember member = _field.getAnnotation(LpcMember.class);
        if (member == null)
        {
            throw new InternalError("Field " + field + " is not annotated with " + LpcMember.class.getName());
        }

        _name = member.name();
        _modifiers = new HashSet<Modifier>(Arrays.asList(member.modifiers()));

        LpcMemberType type = _field.getAnnotation(LpcMemberType.class);

        ClassDefinition cls = null;
        String className = type.className();
        if (!isBlank(className))
        {
            cls = new ClassFinder(declaringObject).find(className);
        }
        _type = Types.getType(type.kind(), cls, type.depth());
    }

    private boolean isBlank(String className)
    {
        return className == null || className.length() == 0;
    }

    public LpcType getType()
    {
        return _type;
    }

    public LpcValue getValue(UserTypeInstance instance)
    {
        if (instance instanceof CompiledInstance)
        {
            CompiledInstance ci = (CompiledInstance) instance;
            try
            {
                Object value = _field.get(ci.getImplementingObject());
                if (value instanceof LpcValue)
                {
                    return (LpcValue) value;
                }
                if (value instanceof LpcReference)
                {
                    return ((LpcReference) value).get();
                }
                throw new InternalError("Field " + value + " is not a valid LpcValue type");
            }
            catch (LpcRuntimeException e)
            {
                throw e;
            }
            catch (Exception e)
            {
                throw new LpcRuntimeException("Internal Error - " + e.getMessage(), e);
            }
        }
        else
        {
            throw new LpcRuntimeException("Internal Error - Cannot get value from instance of type " + instance.getClass());
        }
    }

    public void initialise(UserTypeInstance instance)
    {
        // @TODO ?
    }

    public void setValue(UserTypeInstance instance, LpcValue value)
    {
        if (instance instanceof CompiledInstance)
        {
            CompiledInstance ci = (CompiledInstance) instance;
            try
            {
                if (_field.getType().isAssignableFrom(LpcValue.class))
                {
                    _field.set(ci.getImplementingObject(), value);
                }
                else if (_field.getType().isAssignableFrom(LpcReference.class))
                {
                    LpcReference ref = (LpcReference) _field.get(ci.getImplementingObject());
                    ref.set(value);
                }
                else
                {
                    throw new InternalError("Field " + _field + " cannot be assigned from " + LpcValue.class.getSimpleName());
                }
            }
            catch (LpcRuntimeException e)
            {
                throw e;
            }
            catch (Exception e)
            {
                throw new LpcRuntimeException("Internal Error - " + e.getMessage(), e);
            }
        }
        else
        {
            throw new LpcRuntimeException("Internal Error - Cannot get value from instance of type " + instance.getClass());
        }
    }

    public UserTypeDefinition getDeclaringType()
    {
        return _declaringObject;
    }

    public Set<Modifier> getModifiers()
    {
        return _modifiers;
    }

    public String getName()
    {
        return _name;
    }

    public Kind getKind()
    {
        return Kind.FIELD;
    }

}