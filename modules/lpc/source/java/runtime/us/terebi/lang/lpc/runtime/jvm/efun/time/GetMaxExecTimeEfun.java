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

package us.terebi.lang.lpc.runtime.jvm.efun.time;

import java.util.Collections;
import java.util.List;

import us.terebi.lang.lpc.runtime.ArgumentDefinition;
import us.terebi.lang.lpc.runtime.LpcType;
import us.terebi.lang.lpc.runtime.LpcValue;
import us.terebi.lang.lpc.runtime.jvm.efun.AbstractEfun;
import us.terebi.lang.lpc.runtime.jvm.efun.Efun;
import us.terebi.lang.lpc.runtime.jvm.support.ExecutionTimeCheck;
import us.terebi.lang.lpc.runtime.jvm.support.ValueSupport;
import us.terebi.lang.lpc.runtime.jvm.type.Types;

/**
 * 
 */
public class GetMaxExecTimeEfun extends AbstractEfun implements Efun
{
    protected List< ? extends ArgumentDefinition> defineArguments()
    {
        return Collections.emptyList();
    }

    public LpcType getReturnType()
    {
        return Types.INT;
    }

    public LpcValue execute(List< ? extends LpcValue> arguments)
    {
        long t = ExecutionTimeCheck.get().getMaximumTime();
        return ValueSupport.intValue(t);
    }

}
