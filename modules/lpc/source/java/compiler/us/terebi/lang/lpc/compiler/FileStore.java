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

package us.terebi.lang.lpc.compiler;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.OutputStream;

/**
 * 
 */
public class FileStore implements ClassStore
{
    private final File _directory;

    public FileStore(File directory)
    {
        _directory = directory;
    }

    public OutputStream open(String packageName, String className) throws FileNotFoundException
    {
        String path = packageName.replace('.', '/');
        String fileName = path + '/' + className + ".class";
        File classFile = new File(_directory, fileName);
        classFile.getParentFile().mkdirs();
        return new FileOutputStream(classFile);
    }

}