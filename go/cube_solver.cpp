/*
    Copyright (c) 2016, Ben de Waal
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    * The names of the contributors may not be used to endorse or promote products
      derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "cube.h"
#include "env.h"

// job
struct Job {


    int depth;
    Cube cube;
};
mutex mtx;
condition_variable cv;
vector<Job> jobs;

// worker
struct Worker {
    enum class Signal { Wait, Terminate };
    atomic<Signal> signal;
    thread t;

    static void start(Worker *self) { self->run(); }

    void run() {
        for (;;) {
            {
                unique_lock<mutex> l(mtx);
                cv.wait(l);
            }
            
        }
    }

    void search() {
        /*
         * for all turns {
         *   make turn
         *   get crc
         *   if hash == target hash, {
         *     print sequence
         *   } else if not in xptable {
         *     add to xptable
         *     add to job queue
         *   }
         * }
         */
    }

    Worker(): signal(Signal::Wait) {
        t = thread(Worker::start,this);
    }
};

// context used by multi-threaded solver
struct SolverState {
    // target hash
    qword target;

    // transposition table
    unsigned xpmask;
    qword *xptable;

    // thread pool
    Worker *pool;
} ss;

bool Cube::search(const Cube& start,const Cube& target,int depth,stats_t *stat) {
    // allocate transposition table
    ss.xpmask = env.xpTable - 1;
    ss.xptable = new qword[env.xpTable];
    if (!ss.xptable) {
        cout << "Unable to allocate transposition table. Try a smaller size\n";
        return false;
    }
    memset(ss.xptable,0,sizeof(qword)*env.xpTable);

    // set up worker thread pool
    ss.pool = new Worker[env.threads];

    // set up target
    ss.target = target.state.computeHash();

    // place source in xp

    // use iterative deepening
    for (int d = 1; d <= depth; d++) {
        //THIS ALGO IS BORKED

        // submit source to thread pool

        // wait for completion
    }

    // clean up
//    for (int i = 0; i < env.threads; i++) {
//        ss.signal[i] = Job::Signal::Terminate;
//        ss.pool[i].join();
//    }
    delete[] ss.pool;
    ss.pool = NULL;
    delete[] ss.xptable;
    ss.xptable = NULL;

    // done
    return true;
}