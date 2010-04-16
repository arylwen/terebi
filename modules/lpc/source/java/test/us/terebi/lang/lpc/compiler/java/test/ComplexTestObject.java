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

package us.terebi.lang.lpc.compiler.java.test;

import java.util.List;

import us.terebi.lang.lpc.runtime.LpcValue;
import us.terebi.lang.lpc.runtime.jvm.InheritedObject;
import us.terebi.lang.lpc.runtime.jvm.LpcField;
import us.terebi.lang.lpc.runtime.jvm.LpcFunction;
import us.terebi.lang.lpc.runtime.jvm.LpcInherited;
import us.terebi.lang.lpc.runtime.jvm.LpcMember;
import us.terebi.lang.lpc.runtime.jvm.LpcObject;
import us.terebi.lang.lpc.runtime.jvm.LpcMemberType;
import us.terebi.lang.lpc.runtime.jvm.support.ComparisonSupport;
import us.terebi.lang.lpc.runtime.jvm.support.MathSupport;

/** Automatically generated by us.terebi.lang.lpc.compiler.java.JavaCompiler at Sun May 10 23:22:17 EST 2009 for source/lpc/test/us/terebi/lang/lpc/compiler/java/complex.c */
public class ComplexTestObject extends LpcObject
{
    public @LpcInherited(name = "sword", lpc = "/std/lib/weapons/sword.c", implementation = "us.terebi.lang.lpc.compiler.java.test.SwordTestObject")
    InheritedObject<us.terebi.lang.lpc.compiler.java.test.SwordTestObject> inherit_sword;

    public final LpcField dwarf_kill = new LpcField("dwarf_kill",
            withType(
                    us.terebi.lang.lpc.runtime.LpcType.Kind.INT, 0));

    public @LpcMember(name = "create", modifiers = { us.terebi.lang.lpc.runtime.MemberDefinition.Modifier.PUBLIC })
    @LpcMemberType(kind = us.terebi.lang.lpc.runtime.LpcType.Kind.VOID, depth = 0)
    LpcValue create()
    {
        final LpcValue _lpc_v1 = this.inherit_sword.get().create();
        final LpcValue _lpc_v2 = makeValue("shortsword");
        final LpcValue _lpc_v3 = this.inherit_sword.get().inherit_object.get().set_name(_lpc_v2);
        final LpcValue _lpc_v4 = makeValue("dwarven shortsword");
        final LpcValue _lpc_v5 = this.inherit_sword.get().inherit_object.get().add_alias(_lpc_v4);
        final LpcValue _lpc_v6 = makeValue("dwarven");
        final LpcValue _lpc_v7 = this.inherit_sword.get().inherit_object.get().add_alias(_lpc_v6);
        final LpcValue _lpc_v8 = makeValue("dwarven shortsword");
        final LpcValue _lpc_v9 = this.inherit_sword.get().inherit_object.get().set_short(_lpc_v8);
        final LpcValue _lpc_v10 = makeValue("This is a small iron sword\u002e There are strange dwarven markings on the blade\u002c but they are too faint to read");
        final LpcValue _lpc_v11 = this.inherit_sword.get().inherit_object.get().set_long(_lpc_v10);
        final LpcValue _lpc_v12 = makeValue(7);
        final LpcValue _lpc_v13 = this.inherit_sword.get().inherit_object.get().set_weight(_lpc_v12);
        final LpcValue _lpc_v14 = makeValue(10);
        final LpcValue _lpc_v15 = this.inherit_sword.get().set_wc(_lpc_v14);
        final LpcValue _lpc_v16 = new LpcFunction(getObjectInstance(), 1)
        {
            public LpcValue invoke(LpcValue[] args)
            {
                final LpcValue _lpc_v17 = makeValue("get_race");
                final LpcValue _lpc_v18 = efun("call_other").execute(getArg(args, 1), _lpc_v17);
                final LpcValue _lpc_v19 = makeValue("dwarf");
                final LpcValue _lpc_v20 = ComparisonSupport.equal(_lpc_v18, _lpc_v19);
                final LpcValue _lpc_v21;
                if (_lpc_v20.asBoolean())
                {
                    final LpcValue _lpc_v22 = makeValue(4);
                    _lpc_v21 = _lpc_v22;
                }
                else
                {
                    final LpcValue _lpc_v23 = MathSupport.negate(dwarf_kill.get());
                    _lpc_v21 = _lpc_v23;
                }
                return _lpc_v21;
            }
        };
        final LpcValue _lpc_v24 = this.inherit_sword.get().add_wc_bonus(_lpc_v16);
        return makeValue();
    }
}
