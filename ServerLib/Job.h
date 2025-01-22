#pragma once
#include <functional>


class Job{
public:
	Job(function<void()> callback) : _callback(move(callback)) {

	}
	virtual ~Job() = default;

	void Execute() {
		_callback();
	}

private:
	function<void()> _callback;

};

