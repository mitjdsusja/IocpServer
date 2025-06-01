#pragma once

class IJobQueue;

class JobScheduler{
public:
	void PushJobQueue(shared_ptr<IJobQueue> JobQueue);
	shared_ptr<IJobQueue> PopJobQueue();

private:
	mutex _jobQueueMutex;
	queue<shared_ptr<IJobQueue>> _scheduledJobQueue;
};

