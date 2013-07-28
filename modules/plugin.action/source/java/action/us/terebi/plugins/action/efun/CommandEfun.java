/* ------------------------------------------------------------------------
 * Copyright 2010 Tim Vernum
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

package us.terebi.plugins.action.efun;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import org.apache.log4j.Logger;

import us.terebi.engine.server.InputHandler;
import us.terebi.engine.server.InputHandlerSet;
import us.terebi.engine.server.ObjectShell;
import us.terebi.lang.lpc.runtime.ArgumentDefinition;
import us.terebi.lang.lpc.runtime.AttributeMap;
import us.terebi.lang.lpc.runtime.Callable;
import us.terebi.lang.lpc.runtime.LpcType;
import us.terebi.lang.lpc.runtime.LpcValue;
import us.terebi.lang.lpc.runtime.ObjectInstance;
import us.terebi.lang.lpc.runtime.jvm.LpcConstants;
import us.terebi.lang.lpc.runtime.jvm.context.CallStack.Origin;
import us.terebi.lang.lpc.runtime.jvm.context.RuntimeContext;
import us.terebi.lang.lpc.runtime.jvm.efun.AbstractEfun;
import us.terebi.lang.lpc.runtime.jvm.efun.Efun;
import us.terebi.lang.lpc.runtime.jvm.efun.ThisObjectEfun;
import us.terebi.lang.lpc.runtime.jvm.exception.LpcRuntimeException;
import us.terebi.lang.lpc.runtime.jvm.support.MiscSupport;
import us.terebi.lang.lpc.runtime.jvm.type.Types;
import us.terebi.lang.lpc.runtime.jvm.value.NilValue;
import us.terebi.lang.lpc.runtime.jvm.value.StringValue;
import us.terebi.lang.lpc.runtime.util.ArgumentSpec;
import us.terebi.lang.lpc.runtime.util.BoundMethod;
import us.terebi.lang.lpc.runtime.util.StackCall;
import us.terebi.plugins.action.handler.ActionHandler;
import us.terebi.plugins.interactive.efun.ThisPlayerEfun;

/**
 * 
 */
public class CommandEfun extends AbstractEfun implements Efun
{
	final Logger LOG = Logger.getLogger(CommandEfun.class);
	
    protected List< ? extends ArgumentDefinition> defineArguments()
    {
        return Collections.singletonList(new ArgumentSpec("str", Types.STRING));
    }

    public LpcType getReturnType()
    {
        return Types.INT;
    }

    public LpcValue execute(List< ? extends LpcValue> arguments)
    {
        checkArguments(arguments, 1);
        
        String line = arguments.get(0).asString();
        
    	LOG.info("Handling: "+line);
        if (line == null)
        {
            return NilValue.INSTANCE;
        }
                
        ObjectInstance user = ThisPlayerEfun.this_player();
        ObjectInstance other = ThisObjectEfun.this_object();
        
        if(user.equals(other))
        {
        	LOG.debug("Executing command "+ line + " for " + user + " [" + user.getDefinition() + "]");

           	InputHandlerSet handlers = ObjectShell.getInputHandlers(user);
        	// clone the handlers to avoid concurrent modification exceptions..
        	ArrayList<InputHandler> hdlers = new ArrayList<InputHandler>(handlers.handlers());
        	LOG.info("handlers as array "+hdlers );
        	for (InputHandler handler : hdlers)
        	{
        		if(handler instanceof ActionHandler){
        			//do not bother to process the input, just execute the command...
        			LOG.debug("Processing handler: "+handler);
        			try
        			{
        				String result = handler.inputReceived(user, null, line);
        				LOG.debug("Passed input through " + handler + " and received " + result);
        				if (result == null)
        				{
        					return NilValue.INSTANCE;
        				}
        				line = result;
        			}
        			catch (LpcRuntimeException e)
        			{
        				LOG.error("Unhandled exception ", e);
        			}
        		}
        	}
        } else {

        	LOG.debug("Executing command "+ line + " for " + other + " [" + other.getDefinition() + "]");

        	line = line.trim();
            int space = line.indexOf(' ');
            String verb = (space < 0) ? line : line.substring(0, space);
            String rest = (space < 0) ? "" : line.substring(space + 1);

            //apply cmdAll on other?
            /*Apply apply = new Apply("cmdAll");
           	LOG.info("cmdAll: " + line + " for "+ other );
            LpcValue result = apply.invoke(other, new StringValue(line));
            
        	LOG.debug("Executed command "+ line + " for " + other + " [" + other.getDefinition() + "]"+
        	          " and obtained "+ result);
        	*/            

            AttributeMap attributes;
            synchronized (RuntimeContext.lock())
            {
                attributes = RuntimeContext.obtain().attributes();
            }
            
            attributes.set("action.verb", verb);
            Callable handler = getFunctionReference("cmdAll", other);
            if(handler != null)
            {
                LpcValue result = null;
                result = new StackCall(handler, Origin.DRIVER).execute(new StringValue(line));            	
                attributes.remove("action.verb");
            	LOG.debug("Executed command "+ line + " for " + other + " [" + other.getDefinition() + "]"+
          	          " and obtained "+ result);
                if (result.asBoolean())
                {
                    LOG.debug("Input '" + line + "' handled by " + handler);
                    return LpcConstants.INT.ONE;
                }
            }
            
        }
        return LpcConstants.INT.ONE;
    }

    
    protected Callable getFunctionReference(LpcValue func, LpcValue object)
    {
        if (MiscSupport.isFunction(func))
        {
            return func.asCallable();
        }
        else if (MiscSupport.isString(func))
        {
            ObjectInstance instance = (object == null ? ThisObjectEfun.this_object() : object.asObject());
            return new BoundMethod(func.asString(), instance);
        }
        else
        {
            throw new UnsupportedOperationException("Bad argument to " + getName() + " - " + func + " must be a string or a function");
        }
    }

}
