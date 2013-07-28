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

package us.terebi.lang.lpc.runtime.jvm.efun.environment;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import us.terebi.lang.lpc.runtime.ArgumentDefinition;
import us.terebi.lang.lpc.runtime.LpcType;
import us.terebi.lang.lpc.runtime.LpcValue;
import us.terebi.lang.lpc.runtime.ObjectInstance;
import us.terebi.lang.lpc.runtime.jvm.context.CallStack;
import us.terebi.lang.lpc.runtime.jvm.context.ObjectManager;
import us.terebi.lang.lpc.runtime.jvm.context.RuntimeContext;
import us.terebi.lang.lpc.runtime.jvm.context.SystemContext;
import us.terebi.lang.lpc.runtime.jvm.efun.AbstractEfun;
import us.terebi.lang.lpc.runtime.jvm.efun.CallOtherEfun;
import us.terebi.lang.lpc.runtime.jvm.efun.LivingEfun;
import us.terebi.lang.lpc.runtime.jvm.efun.ThisObjectEfun;
import us.terebi.lang.lpc.runtime.jvm.exception.LpcRuntimeException;
import us.terebi.lang.lpc.runtime.jvm.support.MiscSupport;
import us.terebi.lang.lpc.runtime.jvm.type.Types;
import us.terebi.lang.lpc.runtime.jvm.value.VoidValue;
import us.terebi.lang.lpc.runtime.util.ArgumentSpec;

/**
 * 
 */
public class MoveObjectEfun extends AbstractEfun
{
    protected List< ? extends ArgumentDefinition> defineArguments()
    {
        return Collections.singletonList(new ArgumentSpec("dest", Types.MIXED));
    }

    public LpcType getReturnType()
    {
        return Types.VOID;
    }

    public LpcValue execute(List< ? extends LpcValue> arguments)
    {
        checkArguments(arguments);
        LpcValue arg1 = arguments.get(0);

        ObjectInstance destination;
        if (MiscSupport.isString(arg1))
        {
            SystemContext system = RuntimeContext.obtain().system();
            ObjectManager objectManager = system.objectManager();
            destination = objectManager.findObject(arg1.asString(), true).getMasterInstance();
        }
        else
        {
            destination = arguments.get(0).asObject();
        }

        if (destination == null)
        {
            throw new LpcRuntimeException("No destination");
        }

        ObjectInstance object = ThisObjectEfun.this_object();
        if (object == null)
        {
            throw new LpcRuntimeException("No current object");
        }
        Environment.move(object, destination);

        //TODO
        //we also need to remove the commands from its previous env
        
        //TODO
        //When the mudlib moves an object "A" inside another object "B", the driver (the move_object() efunction) does the following:
        //	1.	if "A" is living, causes "A" to call the init() in "B"
        // 	2.	causes each living object in the inventory of "B" to call init() in "A". regardless of whether "A" is living or not.
        //	3.	if "A" is living, causes "A" to call the init() in each object in the inventory of "B".
        //	Note: an object is considered to be living if enable_commands has been called by that object.
        //	Typically, the init function in an object is used to call add_command for each command that the object offers.
        
        List<LpcValue> args = new ArrayList<LpcValue>();
        List<ObjectInstance> inv = Environment.getInventory(destination, false);
        if(LivingEfun.isLiving(object))
        {
        	//1.
        	CallOtherEfun.callOther(destination, "init", args);
        	//3
        	for(ObjectInstance obj: inv)
        	{
        		CallOtherEfun.callOther(obj, "init", args);
        	}
        }
        //2.
    	for(ObjectInstance obj: inv)
    	{
    		if(LivingEfun.isLiving(obj)){
    	        CallStack stack = RuntimeContext.obtain().callStack();
    	        stack.pushFrame(CallStack.Origin.DRIVER, obj);
    	        //should we use InCOntext????
    	        try
    	        {
   		           CallOtherEfun.callOther(object, "init", args);
    	        }
    	        finally
    	        {
    	            stack.popFrame();
    	        }   
    		}
    	}
        
        
        return VoidValue.INSTANCE;
    }

}
