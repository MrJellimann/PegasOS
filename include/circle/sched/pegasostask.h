#ifndef _pegasostask_h
#define _pegasostask_h

#include <circle/actled.h>
#include "task.h"
 
class CPegasosTask : public CTask
{
public:
	CPegasosTask (<T> task);
	~CPegasosTask (void);

	void Run (void);

private:
	<T> *m_task;
};

#endif