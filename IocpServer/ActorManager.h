#pragma once

class JobQueueBase;

class ActorManager{
public:
	void RegisterActor();
	void UnRegisterActor();

	void RequestAllLatency();

private:
	unordered_map<uint64, shared_ptr<JobQueueBase>> _jobQueueMap;
};

