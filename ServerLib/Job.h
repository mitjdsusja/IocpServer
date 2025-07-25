#pragma once
#include <functional>

class Job{
public:
	using TimePoint = std::chrono::system_clock::time_point;
public:
	Job(function<void()>&& callback) :
		_callback(move(callback)),
		_createTimePoint(std::chrono::system_clock::now())
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

	TimePoint GetCreateTimePoint() {
		return _createTimePoint;
	}

private:
	function<void()> _callback;
	TimePoint _createTimePoint;
};

