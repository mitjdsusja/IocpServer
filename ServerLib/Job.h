#pragma once
#include <functional>

enum JobType {

	NORMALJOB = 1,
	TIMEDJOB = 2,
};

class Job{
public:
	Job(function<void()> callback, JobType jobType = JobType::NORMALJOB)
		: _callback(move(callback)), _jobType(jobType),
		_createTime(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count())
	{
		
	}
	virtual ~Job() = default;

	void Execute() {
		try {
			if (_callback) {
				//cout << "Execute callback " << endl;
				_callback();
			}
			else {
				//cout << "inVALID CALLBACK" << endl;
			}
		}
		catch(exception& e){
			cout << "Exception int Job Execute " << e.what() << endl;
		}
		catch(...){
			cout << "Unknown Exception in Job Execute" << endl;
		}
	}

	JobType GetJobType() {
		return _jobType;
	}

	uint64 GetCreateTime() {
		return _createTime;
	}

private:
	function<void()> _callback;
	uint64 _createTime = 0;
	JobType _jobType;
};

