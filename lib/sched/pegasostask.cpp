// pegasos task is for the terminal
#include <sched/pegasostask.h>
#include <circle/sched/scheduler.h>

CPegasosTask::CPegasosTask (<T> *task)
:	<T> task
{
}

CPegasosTask::~CPegasosTask (void)
{
}

void CPegasosTask::Run (void)
{
    //deal with diff tasks here (task.run() ??)
}
