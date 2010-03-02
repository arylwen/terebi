/* ------------------------------------------------------------------------
 * $Id$
 * Copyright 2008 Tim Vernum
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

package us.terebi.lang.lpc.runtime;

import java.util.Set;

/**
 * 
 */
public interface MemberDefinition
{
    public enum Modifier
    {
        PUBLIC, PROTECTED, PRIVATE, NOSAVE, NOMASK, VARARGS, PURE_VIRTUAL;
        // STATIC is returned as PROTECTED or NOSAVE
    }

    public enum Kind
    {
        FIELD, CLASS, METHOD;
    }

    public Set< ? extends Modifier> getModifiers();
    public String getName();
    public UserTypeDefinition getDeclaringType();
    public Kind getKind();
}
