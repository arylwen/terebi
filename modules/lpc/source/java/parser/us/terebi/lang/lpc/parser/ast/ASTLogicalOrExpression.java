/* Generated By:JJTree: Do not edit this line. ASTLogicalOrExpression.java Version 4.1 */
/* JavaCCOptions:MULTI=true,NODE_USES_PARSER=false,VISITOR=true,TRACK_TOKENS=true,NODE_PREFIX=AST,NODE_EXTENDS=,NODE_FACTORY=,SUPPORT_CLASS_VISIBILITY_PUBLIC=true */
package us.terebi.lang.lpc.parser.ast;

import us.terebi.lang.lpc.parser.jj.*;

public
class ASTLogicalOrExpression extends ExpressionNode {
  public ASTLogicalOrExpression(int id) {
    super(id);
  }

  public ASTLogicalOrExpression(Parser p, int id) {
    super(p, id);
  }


  /** Accept the visitor. **/
  public Object jjtAccept(ParserVisitor visitor, Object data) {
    return visitor.visit(this, data);
  }
}
/* JavaCC - OriginalChecksum=3df2ba2e311f178a1e532e5feb4ed248 (do not edit this line) */
