#pragma once
#include <functional>

class Job {
public:
	using TimePoint = std::chrono::steady_clock::time_point;
public:
	Job(function<void()>&& callback) :
		_callback(move(callback)),
		_createTimePoint(std::chrono::steady_clock::now())
	{

	}
	virtual ~Job() = default;

	void Execute() {
		if (_callback) {
			//cout << "Execute callback " << endl;
			_callback();
		}
		else {
			//cout << "inVALID CALLBACK" << endl;
		}
	}

	void SetEnqueueTimePoint() {

		_enqueueTimePoint = chrono::steady_clock::now();
	}

	TimePoint GetCreateTimePoint() {

		return _createTimePoint;
	}

	TimePoint GetEuqueueTimePoint() {

		return _enqueueTimePoint;
	}

private:
	function<void()> _callback;
	TimePoint _createTimePoint;
	TimePoint _enqueueTimePoint;
};

