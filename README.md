TEREBI LPMUD DRIVER
===================

Changes - arylwen - Aug 11 2013

1. ExpressionCompiler - removed more sythetic modifier, made generated synthetic fucntion public, to conform with the interfcae
2. TelnetChannelConnecitonHandler - fixed the event processing thread to accept a larger stack size; on android the default stack size is too small to compile certain classes
3. 

Changes - arylwen - Aug 3rd 2013

1. ConfigNames - Added auto.compiling.class.loader property; it would support configuring a different class loader for a different platform, e.g. Android.
2. EngineInitializer Added windows path support for loading sefuns and master. Added the auto.compiling.class.loader property
3. ds.terebi.config - added autocompiling classloader property
4. lpc.classpath - upgraded to asm 3.3.1
5. ObjectBuilder - added the refelxive instantiation of the class loader
6. ObjectBuilderFactory - added the classloadername property
7. ExpressionCompiler - removed the synthetic property for the function generated for the catch statement; the android dexer does not index synthetic functions unless they are inner-outer acessors
8. CallOutManager - removed chatty log statements
9. Fixed associated test classes
10. AbstractConfig - the android compiler doesn't accept two methods with the same signature and different visibility while desktop java doesn't complain
11. LpcCompilerObjectManager - reverted back to throwing an error when compiler cicles are detected
12. AbstractObjectDefinition - reverted back to throwing an error when compiler cicles are detected
13. FileResource - android compiler doesn't box very well - double quotes needed
14. LpcValue - the dexer complains if equals is not defined on the interface also when we have a cast to an interface
15. renamed the stout project


Changes - arylwen - July 2013

1. Removed hg from the build files
2. Upgraded Ivy to 2.3.0
3. EngineInitialiser - activated the call-out manager
4. ObjectShell - added debug info
5. ProcessInputHandler - added debug info
6. ds.terebi.config - added lib/include to the global include directories
7. LpcCompilerObjectManager - do not compile a class if already added, however do not stop processing. This allowd to load master.c, that has circular dependencies
8. StandardEFuns - added acos, pow and sqrt
9. AbstractObjectDefinition - do not load master for master (prevent a cycle here)
10. LpcParameter - do not generate varargs by default, as the annotation would generate the parameter anyway; the android dexer complain about a parameter defined twice (while desktop java doesn't)
11.CallOutManager - fixed the deadlock between add and processNextCallout
12. CallOtherEfun - call other doesn't always have 3 arguments - fixed the nullpointer exception
13. FileNameEFun - support for windows file names
14. LivingEfun - provided implementation - nit compatible with the classic definition, needs revision
15. MasterEFun - changed to return NIL rather than throw NullPointerException - masteris not available until is loaded
16. TerminalColourEfun - changed the regex to strip only the color and not the text
17. CallOutEfun - added debug info
18. MoveObjectEfun - added support for add_action - call the init in the other objects per mud rules; TODO - implement removing the actions from the previous env
19. PresentEfun - changed to return an object insetad of int
20. GetDirectoryInfoEfun - added support for both arguments
21. ExecutionTImeCheck - ignore if not generated instead of interrupting the mud - TODO review the compiler and find out why it is not generated on getLocation
22. Apply - debug info
23. TelnetChannelConnection - debug info
24. CommandEfun - provided implementation, including for commands in other objects
25. ActionHandler - provided support for cmdAll - I opted for this rather than implementing process_command in the driver
26. InteractiveEFun - support for the nil argument (rather than combing the lib for the concrete instances)
27. ReceiveEFun - debug info
28. WriteEfun - debug info
29. ObjectSerializer - fixed the array and map serialization; TODO fix the function serialization

-------------------------------------------------------

Based on http://www.terebi.us/

Terebi is a reimplementation of LPMUD in Java

The main public (unstable) repository is available at https://bitbucket.org/tvernum/terebi
A git mirror is available at https://github.com/tvernum/terebi


