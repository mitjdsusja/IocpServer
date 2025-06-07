#pragma once

template<typename T>
class LockQueue {
public:
	void Push(T item) {

		lock_guard<mutex> lock(_queueMutex);
			
		_items.push(move(item));
	}

	T Pop() {

		lock_guard<mutex> lock(_queueMutex);

		if (_items.empty() == true) {
			return T();
		}
			
		T ret = move(_items.front());
		_items.pop();
		return ret;
	}

	void PopAll(vector<T>& items) {

		lock_guard<mutex> lock(_queueMutex);

		while (_items.empty() == false) {
			items.push_back(move(_items.front()));
			_items.pop();
		}
	}

	bool Empty() {
		
		lock_guard<mutex> lock(_queueMutex);

		return _items.empty();
	}

	void Clear() {

		lock_guard<mutex> lock(_queueMutex);

		_items = queue<T>();
	}
private:
	mutex _queueMutex;
	queue<T> _items;
};