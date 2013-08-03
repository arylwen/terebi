/* ------------------------------------------------------------------------
 * Copyright 2010 Tim Vernum
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

package us.terebi.lang.lpc.runtime.jvm.context;

import java.util.Collections;
import java.util.PriorityQueue;

import org.apache.log4j.Logger;

import us.terebi.lang.lpc.runtime.Callable;
import us.terebi.lang.lpc.runtime.LpcValue;
import us.terebi.lang.lpc.runtime.ObjectInstance;
import us.terebi.lang.lpc.runtime.jvm.context.CallStack.Origin;
import us.terebi.lang.lpc.runtime.jvm.efun.callout.CallOutEfun;
import us.terebi.lang.lpc.runtime.util.InContext;
import us.terebi.util.IdGenerator;

/**
 * 
 */
public class CallOutManager
{
	private final Logger LOG = Logger.getLogger(CallOutManager.class);
	
    public class Entry implements Comparable<Entry>
    {
        public final long time;
        public final int id;
        public final Callable callable;
        public final ObjectInstance owner;

        public Entry(long at, int entryId, Callable call, ObjectInstance instance)
        {
            this.time = at;
            this.id = entryId;
            this.callable = call;
            this.owner = instance;
        }

        /**
         * @see Long#hashCode()
         */
        public int hashCode()
        {
            return id;
        }

        public boolean equals(Object obj)
        {
            if (obj == this)
            {
                return true;
            }
            if (obj == null)
            {
                return false;
            }
            if (!(obj instanceof Entry))
            {
                return false;
            }
            Entry other = (Entry) obj;
            return this.id == other.id;
        }

        public int compareTo(Entry other)
        {
            if (this.id == other.id)
            {
                return 0;
            }
            if (this.time < other.time)
            {
                return -1;
            }
            if (this.time > other.time)
            {
                return +1;
            }
            if (this.id < other.id)
            {
                return -1;
            }
            if (this.id > other.id)
            {
                return +1;
            }
            return 0;
        }
    }

    private final SystemContext _context;
    private final PriorityQueue<Entry> _queue;
    private final IdGenerator _id;
    private final Object _lock;

    public CallOutManager(SystemContext context)
    {
        _context = context;
        _queue = new PriorityQueue<Entry>();
        _id = new IdGenerator();
        _lock = new Object();
    }

    public void start()
    {
        new Thread("call-out")
        {
            public void run()
            {
            	LOG.info("CallOutManager started");
                poll();
            }
        }.start();
    }

    public int add(long at, Callable callable, ObjectInstance owner)
    {
        int id = _id.next();
        synchronized (_lock)
        {
            Entry entry = new Entry(at, id, callable, owner);
            _queue.offer(entry);
            LOG.info("Added to queue "+entry);
            _lock.notify();
        }
        return id;
    }

    public Entry find(long id)
    {
        synchronized (_lock)
        {
            for (Entry entry : _queue)
            {
                if (entry.id == id)
                {
                    return entry;
                }
            }
        }
        return null;
    }

    void poll()
    {
        for (;;)
        {
            try
            {
                processNextCallout();
            }
            catch (InterruptedException e)
            {
                Thread.currentThread().interrupt();
            }
        }
    }

    private void processNextCallout() throws InterruptedException
    {
        long now = System.currentTimeMillis();
        Entry entry = null;
        //LOG.info("processNextCallout");
        synchronized (_lock)
        {
        	//LOG.info("After lock");
            //if (_queue.isEmpty())
            //{
            //    _lock.wait();
            //    return;
            //}
            if( !_queue.isEmpty() ){
                Entry peek = _queue.peek();
                LOG.info("queue not empty "+peek.callable);
                if (peek.time > now)
                {
                	LOG.info("too early "+peek.time);
                    long sleep = peek.time - now;
                    _lock.wait(sleep);
                    //Thread.sleep(sleep);
                   return;
                }            

                entry = _queue.poll();
            	LOG.info("Got an entry "+entry.callable);
            }
        }
        
        if( entry != null )execute(entry);
    }

    private void execute(final Entry entry)
    {
    	LOG.info("Firing "+entry.callable);
        RuntimeContext.activate(_context);
        InContext.execute(Origin.CALL_OUT, entry.owner, new InContext.Exec<LpcValue>()
        {
            public LpcValue execute()
            {
                return entry.callable.execute();
            }
        });
        LOG.info("Fired "+entry.callable);
    }

    public Iterable<Entry> all()
    {
        return Collections.unmodifiableCollection(_queue);
    }
}
