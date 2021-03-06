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

package us.terebi.lang.lpc.runtime.jvm.support;

import us.terebi.lang.lpc.runtime.LpcValue;
import us.terebi.lang.lpc.runtime.jvm.LpcConstants;

/**
 * 
 */
public class LogicSupport
{
    public static LpcValue not(LpcValue value)
    {
        if (value == null)
        {
            throw new NullPointerException("Internal Error - Null value passed to " + LogicSupport.class.getSimpleName());
        }
        if (value.asBoolean())
        {
            return LpcConstants.INT.FALSE;
        }
        else
        {
            return LpcConstants.INT.TRUE;
        }
    }

}
