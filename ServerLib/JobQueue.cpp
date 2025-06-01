#include "pch.h"
#include "JobQueue.h"
#include "JobScheduler.h"

void JobQueueBase::PushJob(Job* job){

	{
		lock_guard<mutex> lock(_jobQueueMutex);
		_jobQueue.push(job);
	}

	bool expected = false;
	if (_pendingJob.compare_exchange_strong(expected, true)) {
		GJobScheduler->PushJobQueue(shared_from_this());
	}
}

void JobQueueBase::ExecuteJob(){

	queue<Job*> jobsToExecute;

	// Swap JobQueue
	{
		lock_guard<mutex> lock(_jobQueueMutex);

		swap(jobsToExecute, _jobQueue);
	}

	while (jobsToExecute.empty() == false) {
		Job* job = jobsToExecute.front();
		jobsToExecute.pop();

		job->Execute();

		delete job;
	}

	// Execute�� �۾��� queue�� �߰� ������ �� ����.
	bool scheduleNeeded = false;
	{
		lock_guard<mutex> lock(_jobQueueMutex);

		// Execute�� �۾��� �߰��� ���
		if (_jobQueue.empty() == false) {
			scheduleNeeded = true;
		}
		else {
			_pendingJob.store(false);
		}
	}

	if (scheduleNeeded == true) {
		GJobScheduler->PushJobQueue(shared_from_this());
	}
}

bool JobQueueBase::HasJobs(){

	lock_guard<mutex> lock(_jobQueueMutex);

	if (_jobQueue.empty())	return false;
	else return true;
}
