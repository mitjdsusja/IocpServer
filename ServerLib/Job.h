#pragma once
#include <functional>

class Job{
public:
	Job(function<void()> callback)
		: _callback(move(callback)),
		_createTime(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count())
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

	uint64 GetCreateTime() {
		return _createTime;
	}

private:
	function<void()> _callback;
	uint64 _createTime = 0;
};

