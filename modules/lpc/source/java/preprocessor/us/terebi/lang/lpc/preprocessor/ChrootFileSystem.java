/*
 * Anarres C Preprocessor
 * Copyright (c) 2007-2008, Shevek
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
 * or implied.  See the License for the specific language governing
 * permissions and limitations under the License.
 */

package us.terebi.lang.lpc.preprocessor;

import java.io.File;
import java.io.IOException;

import us.terebi.lang.lpc.preprocessor.FileLexerSource;
import us.terebi.lang.lpc.preprocessor.Source;

/**
 * A virtual filesystem implementation using java.io in a virtual
 * chroot.
 */
public class ChrootFileSystem implements VirtualFileSystem
{
    File _root;

    public ChrootFileSystem(File root)
    {
        this._root = root;
    }

    public VirtualFile getFile(String path)
    {
        return new ChrootFile(path);
    }

    public VirtualFile getFile(String dir, String name)
    {
        return new ChrootFile(dir, name);
    }

    private class ChrootFile extends File implements VirtualFile
    {
        public ChrootFile(String path)
        {
            super(path);
        }

        public ChrootFile(String dir, String name)
        {
            super(dir, name);
        }

        /* private */
        public ChrootFile(File dir, String name)
        {
            super(dir, name);
        }

        public ChrootFile getParentFile()
        {
            return new ChrootFile(getParent());
        }

        public ChrootFile getChildFile(String name)
        {
            return new ChrootFile(this, name);
        }

        public Source getSource() throws IOException
        {
            return new FileLexerSource(getFile(), getPath());
        }

        private File getFile()
        {
            return new File(_root, getPath());
        }

        public boolean isFile()
        {
            return getFile().isFile();
        }

    }

}
