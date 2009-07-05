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
import java.lang.reflect.Method;
import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

import us.terebi.lang.lpc.compiler.CompilerObjectManager;
import us.terebi.lang.lpc.compiler.java.context.CompiledObjectDefinition;
import us.terebi.lang.lpc.compiler.java.context.CompiledObjectInstance;
import us.terebi.lang.lpc.compiler.java.context.ScopeLookup;
import us.terebi.lang.lpc.runtime.ClassDefinition;
import us.terebi.lang.lpc.runtime.CompiledMethodDefinition;
import us.terebi.lang.lpc.runtime.FieldDefinition;
import us.terebi.lang.lpc.runtime.ObjectDefinition;
import us.terebi.lang.lpc.runtime.ObjectInstance;
import us.terebi.lang.lpc.runtime.jvm.InheritedObject;
import us.terebi.lang.lpc.runtime.jvm.LpcClass;
import us.terebi.lang.lpc.runtime.jvm.LpcField;
import us.terebi.lang.lpc.runtime.jvm.LpcInherited;
import us.terebi.lang.lpc.runtime.jvm.LpcMember;
import us.terebi.lang.lpc.runtime.jvm.LpcObject;
import us.terebi.lang.lpc.runtime.jvm.exception.InternalError;
import us.terebi.lang.lpc.runtime.jvm.exception.LpcRuntimeException;

/**
 * 
 */
public class CompiledDefinition<T extends LpcObject> implements CompiledObjectDefinition
{
    private final ScopeLookup _lookup;
    private final Class< ? extends T> _implementation;
    private final Map<String, CompiledObjectDefinition> _inherited;
    private final Map<String, CompiledField> _fields;
    private final Map<String, ClassDefinition> _classes;
    private final Map<String, CompiledMethod> _methods;
    private final String _name;

    private CompiledObjectInstance _master;

    public CompiledDefinition(CompilerObjectManager manager, ScopeLookup lookup, String name, Class< ? extends T> implementation)
    {
        _lookup = lookup;
        _name = name;
        _implementation = implementation;
        _inherited = new HashMap<String, CompiledObjectDefinition>();
        _fields = new HashMap<String, CompiledField>();
        _classes = new HashMap<String, ClassDefinition>();
        _methods = new HashMap<String, CompiledMethod>();
        introspect(manager);
    }

    @SuppressWarnings("unchecked")
    private void introspect(CompilerObjectManager manager)
    {
        Method[] methods = _implementation.getDeclaredMethods();
        for (Method method : methods)
        {
            LpcMember annotation = method.getAnnotation(LpcMember.class);
            if (annotation != null)
            {
                _methods.put(annotation.name(), new CompiledMethod(this, method, _lookup));
            }
        }

        Field[] fields = _implementation.getDeclaredFields();
        for (Field field : fields)
        {
            if (LpcField.class.isAssignableFrom(field.getType()))
            {
                _fields.put(field.getName(), new CompiledField(this, field));
            }
            else if (field.getAnnotation(LpcInherited.class) != null)
            {
                LpcInherited inherited = field.getAnnotation(LpcInherited.class);
                assert (inherited != null);
                _inherited.put(inherited.name(), findInherited(manager, inherited));
            }
        }

        Class[] classes = _implementation.getDeclaredClasses();
        for (Class cls : classes)
        {
            if (LpcClass.class.isAssignableFrom(cls))
            {
                ClassDefinition classDefinition = LpcClass.getClassDefinition(cls, this);
                _classes.put(classDefinition.getName(), classDefinition);
            }
        }
    }

    private CompiledObjectDefinition findInherited(CompilerObjectManager manager, LpcInherited inherited)
    {
        String lpc = inherited.lpc();
        CompiledObjectDefinition object = manager.findObject(lpc);
        if (object == null)
        {
            throw new LpcRuntimeException("Internal error - Object manager "
                    + manager
                    + " has no object for  "
                    + lpc
                    + " but "
                    + _implementation
                    + " expects it to be implemented by "
                    + inherited.implementation());
        }
        if (inherited.implementation().equals(object.getImplementationClass().getName()))
        {
            return object;
        }
        throw new LpcRuntimeException("Internal error - Object manager "
                + manager
                + " thinks "
                + lpc
                + " is implemented by "
                + object
                + " but "
                + _implementation
                + " expected "
                + inherited.implementation());
    }

    public Class< ? extends LpcObject> getImplementationClass()
    {
        return _implementation;
    }

    public Map<String, ClassDefinition> getDefinedClasses()
    {
        return Collections.unmodifiableMap(_classes);
    }

    public CompiledObjectInstance getInheritableInstance()
    {
        return newInstance(0, false);
    }

    public Map<String, ? extends ObjectDefinition> getInheritedObjects()
    {
        return Collections.unmodifiableMap(_inherited);
    }

    public CompiledObjectInstance getMasterInstance()
    {
        if (_master == null)
        {
            _master = newInstance(0, true);
        }
        return _master;
    }

    private CompiledObject<T> newInstance(long id, boolean register)
    {
        T object = createObject();

        Map<String, ObjectInstance> parents = new HashMap<String, ObjectInstance>();

        Field[] fields = object.getClass().getDeclaredFields();
        for (Field field : fields)
        {
            InheritedObject< ? > inherited = setInheritedField(object, field);
            if (inherited != null)
            {
                parents.put(inherited.getName(), inherited.getObjectInstance());
            }
        }

        CompiledObject<T> instance = new CompiledObject<T>(this, id, object, parents);
        object.setDefinition(this);
        object.setInstance(instance);
        if (register)
        {
            objectManager().registerObject(instance);
        }
        return instance;
    }

    private InheritedObject< ? > setInheritedField(LpcObject object, Field field)
    {
        LpcInherited annotation = field.getAnnotation(LpcInherited.class);
        if (annotation == null)
        {
            return null;
        }
        CompiledObjectDefinition definition = _inherited.get(annotation.name());
        InheritedObject< ? > inherited = new InheritedObject<Object>(annotation.name(), definition.getImplementationClass(),
                definition);
        try
        {
            field.set(object, inherited);
        }
        catch (Exception e)
        {
            throw new InternalError(e);
        }
        return inherited;
    }

    private T createObject()
    {
        try
        {
            return _implementation.newInstance();
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

    public Map<String, ? extends CompiledMethodDefinition> getMethods()
    {
        return Collections.unmodifiableMap(_methods);
    }

    public CompiledObject<T> newInstance()
    {
        return newInstance(objectManager().allocateObjectIdentifier(), true);
    }

    private CompilerObjectManager objectManager()
    {
        return _lookup.objectManager();
    }

    public Map<String, ? extends FieldDefinition> getFields()
    {
        return Collections.unmodifiableMap(_fields);
    }

    public String getName()
    {
        return _name;
    }

    public String toString()
    {
        return getClass().getSimpleName() + "{" + _name + ":" + _implementation + "}";
    }

}