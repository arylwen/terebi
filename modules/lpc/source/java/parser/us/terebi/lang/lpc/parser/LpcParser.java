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

package us.terebi.lang.lpc.parser;

import java.io.File;
import java.io.IOException;
import java.io.StringReader;

import org.apache.commons.io.IOUtils;

import us.terebi.lang.lpc.io.FileFinder;
import us.terebi.lang.lpc.io.Resource;
import us.terebi.lang.lpc.io.ResourceLexerSource;
import us.terebi.lang.lpc.io.ResourceFinder;
import us.terebi.lang.lpc.io.SourceFinderFileSystem;
import us.terebi.lang.lpc.parser.ast.ASTObjectDefinition;
import us.terebi.lang.lpc.parser.jj.ParseException;
import us.terebi.lang.lpc.parser.jj.Parser;
import us.terebi.lang.lpc.preprocessor.CppReader;
import us.terebi.lang.lpc.preprocessor.Feature;
import us.terebi.lang.lpc.preprocessor.LexerException;
import us.terebi.lang.lpc.preprocessor.Preprocessor;
import us.terebi.lang.lpc.preprocessor.PreprocessorListener;
import us.terebi.lang.lpc.preprocessor.Source;

/*
 * @TODO
 *    Add predefined macros (constants), e.g. __SAVE_EXTENSION__ (Partially done)
 *    Auto-include file
 */
public class LpcParser
{
    public class Listener extends PreprocessorListener
    {
        public void handleWarning(Source source, int line, int column, String msg)
        {
            System.out.println("Preprocessor warning: " + source.toString() + ":" + line + ":" + column + " - " + msg);
        }
    }

    private boolean _debug;
    private Preprocessor _preprocessor;
    private ResourceFinder _sourceFinder;

    public LpcParser()
    {
        _preprocessor = new Preprocessor();
        _preprocessor.addFeature(Feature.LINEMARKERS);
        _sourceFinder = new FileFinder(new File("/"));
    }

    public void setDebug(boolean debug)
    {
        _debug = debug;
    }

    public void addSystemIncludeDirectory(String dir)
    {
        _preprocessor.getSystemIncludePath().add(dir);
    }

    public void addUserIncludeDirectory(String dir)
    {
        _preprocessor.getQuoteIncludePath().add(dir);
    }

    public void addAutoIncludeFile(String filename) throws IOException
    {
        ResourceLexerSource source = getSource(filename);
        _preprocessor.addInput(source);
    }

    public void setFileSystemRoot(File root)
    {
        _sourceFinder = new FileFinder(root);
    }

    public void setSourceFinder(ResourceFinder sourceFinder)
    {
        _sourceFinder = sourceFinder;
    }

    /**
     * @deprecated Use {@link #parse(Resource)} instead
     */
    public ASTObjectDefinition parse(String filename) throws IOException, LexerException
    {
        Resource resource = _sourceFinder.getResource(filename);
        return parse(resource);
    }

    public String preprocess(Resource resource) throws IOException, LexerException
    {
        if (_sourceFinder != null)
        {
            _preprocessor.setFileSystem(new SourceFinderFileSystem(_sourceFinder));
        }

        String dirname = resource.getParent().getPath();

        _preprocessor.addMacro("__SAVE_EXTENSION__", "\".o\"");
        _preprocessor.addMacro("__DIR__", "\"" + dirname + "\"");
        _preprocessor.addInput(getSource(resource));

        _preprocessor.setListener(new Listener());
        CppReader reader = new CppReader(_preprocessor);
        String content = IOUtils.toString(reader);
        return content;
    }

    private ResourceLexerSource getSource(String filename) throws IOException
    {
        return getSource(_sourceFinder.getResource(filename));
    }

    private ResourceLexerSource getSource(Resource resource) throws IOException
    {
        return new ResourceLexerSource(resource);
    }

    public ASTObjectDefinition parse(Resource resource) throws IOException, LexerException
    {
        LineMapping mapping = new LineMapping(resource.getPath());
        new ParserState(this, mapping);

        String content = preprocess(resource);

        Parser parser = new Parser(new StringReader(content));
        parser.setDebug(_debug);
        try
        {
            return parser.ObjectDefinition();
        }
        catch (ParseException pe)
        {
            if (pe.currentToken != null)
            {
                StringBuilder err = new StringBuilder("Syntax error at ");
                int inputLine = pe.currentToken.beginLine;
                String file = mapping.getFile(inputLine);
                if (file != null)
                {
                    err.append(file).append(':');
                }
                else
                {
                    err.append("line ");
                }
                int line = mapping.getLine(inputLine);
                err.append(line);

                System.err.println(err);
            }
            pe.printStackTrace(System.err);
            return null;
        }

    }

    public void addDefine(String name, String value) throws LexerException
    {
        _preprocessor.addMacro(name, value);
    }

}
