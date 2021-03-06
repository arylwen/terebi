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

package us.terebi.lang.lpc.runtime.jvm.efun;

import java.util.Collection;
import java.util.Collections;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.apache.log4j.Logger;

import us.terebi.lang.lpc.runtime.ArgumentDefinition;
import us.terebi.lang.lpc.runtime.Callable;
import us.terebi.lang.lpc.runtime.FunctionSignature;
import us.terebi.lang.lpc.runtime.LpcType;
import us.terebi.lang.lpc.runtime.LpcValue;
import us.terebi.lang.lpc.runtime.ObjectInstance;
import us.terebi.lang.lpc.runtime.jvm.type.Types;
import us.terebi.lang.lpc.runtime.util.ArgumentSpec;

/**
 * 
 */
public class LivingEfun extends AbstractEfun implements FunctionSignature, Callable
{
	final Logger LOG = Logger.getLogger(LivingEfun.class);
	
    protected List< ? extends ArgumentDefinition> defineArguments()
    {
        return Collections.singletonList(new ArgumentSpec("ob", Types.OBJECT));
    }

    public LpcType getReturnType()
    {
        return Types.INT;
    }
    
    public boolean acceptsLessArguments()
    {
        return true;
    }

    public LpcValue execute(List< ? extends LpcValue> arguments)
    {
    	 checkArguments(arguments, 1);
    	 ObjectInstance obj = arguments.get(0).asObject();
       
    	 //Map<String, ? extends ObjectInstance>  inherited = obj.getInheritedObjects();
    	 //Set<String> classes = inherited.keySet();
    	 
    	 Set<String> classes = getAllInheritedClasses(obj);
    	 
    	 LOG.debug("inherited classes for: "+ obj +"  are "+ classes);
    	 
    	 
    	 if(classes.contains("living")){
    		 return getValue(true);
    	 } else {
    		 return getValue(false);
    	 }        
    }
    
    private static Set<String> getAllInheritedClasses( ObjectInstance obj )
    {
      	Map<String, ? extends ObjectInstance>  inherited = obj.getInheritedObjects();
      	Set<String> classes = inherited.keySet();
      	//copy to avoid the UnmodifiableException
      	Set<String> classescopy = new HashSet<String>(classes);
      	Collection<? extends ObjectInstance> supers = inherited.values();
        
      	for( ObjectInstance superobj : supers )	{
        	Set<String> supersupers = getAllInheritedClasses(superobj);
        	classescopy.addAll(supersupers);
        }
        
        return classescopy;
    }

    public static boolean isLiving(ObjectInstance obj)
    {
    	return getAllInheritedClasses(obj).contains("living");
    }
    
}
