#include "JobQueue.h"

void JobQueue::Push(Job* job){
	const int32 preCount = _jobCount.fetch_add(1);
	_jobs.Push(job);

	if (preCount == 0) {
		if (LCurrentJobQueue == nullptr) {
			Execute();
		}
		else {
			
		}
	}
}

void JobQueue::Execute()
{
}
