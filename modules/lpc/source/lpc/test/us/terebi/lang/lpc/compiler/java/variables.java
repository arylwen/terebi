package us.terebi.lang.lpc.test;

import java.util.*;
import us.terebi.lang.lpc.runtime.*;
import us.terebi.lang.lpc.runtime.jvm.*;
import us.terebi.lang.lpc.runtime.jvm.support.*;

/** Automatically generated by us.terebi.lang.lpc.compiler.java.JavaCompiler at %%% %%% ## ##:##:## %%% #### for source/lpc/test/us/terebi/lang/lpc/compiler/java/variables.c */
public class variables extends LpcObject
{
/* Inheritance */
/* Members */

/* Field global_int (Line:3) */
    public @us.terebi.lang.lpc.runtime.jvm.LpcMember(name="global_int", modifiers={us.terebi.lang.lpc.runtime.MemberDefinition.Modifier.PRIVATE}) 
    @us.terebi.lang.lpc.runtime.jvm.LpcMemberType(kind=us.terebi.lang.lpc.runtime.LpcType.Kind.INT, depth=0) 
    final LpcField _f_global_int = new LpcField( "global_int", withType( us.terebi.lang.lpc.runtime.LpcType.Kind.INT, 0) );

/* Field global_string (Line:4) */
    public @us.terebi.lang.lpc.runtime.jvm.LpcMember(name="global_string", modifiers={us.terebi.lang.lpc.runtime.MemberDefinition.Modifier.NOSAVE}) 
    @us.terebi.lang.lpc.runtime.jvm.LpcMemberType(kind=us.terebi.lang.lpc.runtime.LpcType.Kind.STRING, depth=0) 
    final LpcField _f_global_string = new LpcField( "global_string", withType( us.terebi.lang.lpc.runtime.LpcType.Kind.STRING, 0) );

/* Method func (Line:6) */
    public @us.terebi.lang.lpc.runtime.jvm.LpcMember(name="func", modifiers={us.terebi.lang.lpc.runtime.MemberDefinition.Modifier.PUBLIC}) 
    @us.terebi.lang.lpc.runtime.jvm.LpcMemberType(kind=us.terebi.lang.lpc.runtime.LpcType.Kind.VOID, depth=0) LpcValue 
    func_( 
@us.terebi.lang.lpc.runtime.jvm.LpcParameter(kind=us.terebi.lang.lpc.runtime.LpcType.Kind.INT, depth=0, name="param_int", 
                                             semantics=us.terebi.lang.lpc.runtime.ArgumentSemantics.BY_VALUE) LpcValue _p_param_int_v ,
@us.terebi.lang.lpc.runtime.jvm.LpcParameter(kind=us.terebi.lang.lpc.runtime.LpcType.Kind.OBJECT, depth=0, name="param_object", 
                                             semantics=us.terebi.lang.lpc.runtime.ArgumentSemantics.BY_VALUE) LpcValue _p_param_object_v ,
@us.terebi.lang.lpc.runtime.jvm.LpcParameter(kind=us.terebi.lang.lpc.runtime.LpcType.Kind.STRING, depth=0, name="param_string", 
                                             semantics=us.terebi.lang.lpc.runtime.ArgumentSemantics.BY_VALUE) LpcValue _p_param_string_v )
    {
        final LpcVariable _p_param_int = new LpcVariable( "param_int", withType( us.terebi.lang.lpc.runtime.LpcType.Kind.INT, 0) , _p_param_int_v);
        final LpcVariable _p_param_object = new LpcVariable( "param_object", withType( us.terebi.lang.lpc.runtime.LpcType.Kind.OBJECT, 0) , _p_param_object_v);
        final LpcVariable _p_param_string = new LpcVariable( "param_string", withType( us.terebi.lang.lpc.runtime.LpcType.Kind.STRING, 0) , _p_param_string_v);

        final LpcVariable _l_local_mixed = new LpcVariable( "local_mixed", withType( us.terebi.lang.lpc.runtime.LpcType.Kind.MIXED, 0) );
        final LpcVariable _l_local_int_array = new LpcVariable( "local_int_array", withType( us.terebi.lang.lpc.runtime.LpcType.Kind.INT, 1) );
        final LpcVariable _l_local_function = new LpcVariable( "local_function", withType( us.terebi.lang.lpc.runtime.LpcType.Kind.FUNCTION, 0) );
        final LpcVariable _l_local_mapping = new LpcVariable( "local_mapping", withType( us.terebi.lang.lpc.runtime.LpcType.Kind.MAPPING, 0) );

        final LpcValue _lpc_v1 = makeValue(0.5);
        final LpcVariable _l_local_float_1 = new LpcVariable( "local_float_1", withType( us.terebi.lang.lpc.runtime.LpcType.Kind.FLOAT, 0) , _lpc_v1);

        final LpcValue _lpc_v2 = makeValue(2.5);
        final LpcVariable _l_local_float_2 = new LpcVariable( "local_float_2", withType( us.terebi.lang.lpc.runtime.LpcType.Kind.FLOAT, 0) , _lpc_v2);

        final LpcValue _lpc_v3 = makeValue(1.0);
        final LpcVariable _l_local_float_3 = new LpcVariable( "local_float_3", withType( us.terebi.lang.lpc.runtime.LpcType.Kind.FLOAT, 0) , _lpc_v3);

        final LpcValue _lpc_v4 = makeArray( _p_param_int.get() , _f_global_int.get() );
        _l_local_int_array.set(_lpc_v4);

        _l_local_mixed.set(_p_param_string.get());
    
        final LpcValue _lpc_v5 = makeMapping( _p_param_string.get() , _p_param_object.get() , _f_global_string.get() , _l_local_int_array.get() );
        _l_local_mapping.set(_lpc_v5);

        final LpcValue _lpc_v6 = new us.terebi.lang.lpc.runtime.jvm.LpcFunction(getObjectInstance(),2)
        {
            public LpcValue execute(List<? extends LpcValue> args) 
            {
                final LpcValue _lpc_v7 = MathSupport.multiply(getArg(args, 1),_l_local_float_1.get());
                final LpcValue _lpc_v8 = MathSupport.multiply(getArg(args, 2),_l_local_float_2.get());
                final LpcValue _lpc_v9 = MathSupport.add(_lpc_v7,_lpc_v8);
                final LpcValue _lpc_v10 = MathSupport.add(_lpc_v9,_l_local_float_3.get());
                return _lpc_v10;
            } /* _lpc_v6.execute() */
        }; /* _lpc_v6 */

        _l_local_function.set(_lpc_v6);

        return makeValue();
    } 
}