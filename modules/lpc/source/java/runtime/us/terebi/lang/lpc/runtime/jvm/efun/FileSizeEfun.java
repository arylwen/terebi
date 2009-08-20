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

import java.io.IOException;
import java.util.Collections;
import java.util.List;

import org.apache.log4j.Logger;

import us.terebi.lang.lpc.io.Resource;
import us.terebi.lang.lpc.io.ResourceFinder;
import us.terebi.lang.lpc.runtime.ArgumentDefinition;
import us.terebi.lang.lpc.runtime.Callable;
import us.terebi.lang.lpc.runtime.FunctionSignature;
import us.terebi.lang.lpc.runtime.LpcType;
import us.terebi.lang.lpc.runtime.LpcValue;
import us.terebi.lang.lpc.runtime.jvm.LpcConstants;
import us.terebi.lang.lpc.runtime.jvm.context.RuntimeContext;
import us.terebi.lang.lpc.runtime.jvm.context.SystemContext;
import us.terebi.lang.lpc.runtime.jvm.support.ValueSupport;
import us.terebi.lang.lpc.runtime.jvm.type.Types;
import us.terebi.lang.lpc.runtime.util.ArgumentSpec;

/**
 * 
 */
public class FileSizeEfun extends AbstractEfun implements FunctionSignature, Callable
{
    private final Logger LOG = Logger.getLogger(FileSizeEfun.class);

    protected List< ? extends ArgumentDefinition> defineArguments()
    {
        return Collections.singletonList(new ArgumentSpec("file", Types.STRING));
    }

    public LpcType getReturnType()
    {
        return Types.INT;
    }

    public LpcValue execute(List< ? extends LpcValue> arguments)
    {
        checkArguments(arguments);
        String name = arguments.get(0).asString();
        try
        {
            long fs = file_size(name);
            return ValueSupport.intValue(fs);
        }
        catch (IOException e)
        {
            LOG.info("Cannot get size of file " + name + " : " + e.toString());
            return LpcConstants.INT.MINUS_ONE;
        }
    }

    public long file_size(String name) throws IOException
    {
        SystemContext system = RuntimeContext.obtain().system();
        ResourceFinder resourceFinder = system.resourceFinder();
        Resource resource = resourceFinder.getResource(name);
        if (!resource.exists())
        {
            return -1;
        }
        else if (!resource.isFile())
        {
            return -2;
        }
        else
        {
            return resource.getSize();
        }
    }

}
